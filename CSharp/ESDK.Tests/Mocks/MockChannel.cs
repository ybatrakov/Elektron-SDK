/*|-----------------------------------------------------------------------------
 *|            This source code is provided under the Apache 2.0 license      --
 *|  and is provided AS IS with no warranty or guarantee of fit for purpose.  --
 *|                See the project's LICENSE.md for details.                  --
 *|           Copyright Thomson Reuters 2018. All rights reserved.            --
 *|-----------------------------------------------------------------------------
 */

using System;
using System.Collections.Generic;
using System.Net;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

using ThomsonReuters.Eta.Common;
using ThomsonReuters.Common.Logger;
using ThomsonReuters.Eta.Internal.Interfaces;
using ThomsonReuters.Eta.Transports;
using ThomsonReuters.Eta.Transports.Internal;
using System.Net.Sockets;

namespace ThomsonReuters.Eta.Tests
{
    internal class MockChannel : ISocketChannel, IDisposable
    {
        public bool IsConnected { get; set; }
        public bool IsDisposed { get; }
        public bool IsServer { get; }
        public bool IsDataReady { get; set; }

        public EndPoint RemoteEP { get; private set; }
        public int RemotePort { get; private set; }

        public Guid ChannelId { get; } = Guid.NewGuid();

        public long ReceivedCount { get; private set; }

        public Socket Socket => throw new NotImplementedException();

        public event Transports.ConnectionStateChangeHandler OnConnected;
        public event DataXferHandler OnDataReady;
        public event DataXferHandler OnDataSent;
        public event Transports.ConnectionStateChangeHandler OnDisconnected;
        public event Transports.ConnectionStateChangeHandler OnError;

        #region Raise Events
        /// <summary>
        /// Fire the OnConnected event.
        /// </summary>
        /// <returns></returns>
        internal void RaiseConnected(ResultObject aro)
        {
            EtaLogger.Instance.Trace($"{DateTime.UtcNow:HH:mm:ss.ffff} MockChannel({ChannelId})::EndConnect AsynchResultObject: {aro}");

            IsConnected = true;
            Recv(aro.UserState);
            OnConnected?.Invoke(this, new SocketEventArgs(this, "Connected"));
        }

        /// <summary>
        /// Fire the OnDisconnected event. <see cref="ResultObject"/>
        /// </summary>
        internal void RaiseDisconnected()
        {
            EtaLogger.Instance.Trace($"{DateTime.UtcNow:HH:mm:ss.ffff} MockChannel({ChannelId})::Channel Disconnecting from {RemoteEP}");

            OnDisconnected?.Invoke(this, new SocketEventArgs(null, ""));
        }

        internal void RaiseDataSent(ResultObject aro)
        {
            EtaLogger.Instance.Trace($"{DateTime.UtcNow:HH:mm:ss.ffff} MockChannel({ChannelId})::DataSent - AsyncResultObject: {aro}, ");
            OnDataSent?.Invoke(aro);
        }

        internal void RaiseDataReady(ResultObject aro)
        {
            ReceivedCount++;

            EtaLogger.Instance.Trace($"{DateTime.UtcNow:HH:mm:ss.ffff} MockChannel({ChannelId})::DataReady - AsyncResultObject: {aro}, ");
            OnDataReady?.Invoke(aro);
        }
        /// <summary>
        /// Fire OnError event.
        /// </summary>
        /// <param name="aro"></param>
        /// <param name="message"></param>
        internal void RaiseError(ResultObject aro, string message)
        {
            StringBuilder sb = new StringBuilder();
            sb.AppendFormat("SocketChannel::RaiseError:  {0}", message);
            EtaLogger.Instance.Trace($"{DateTime.UtcNow:HH:mm:ss.ffff} MockChannel({ChannelId})::RaiseError Message:{sb}");

            OnError?.Invoke(this, new SocketEventArgs(aro.UserState, message));
        }


        #endregion

        #region ISocketChannel

        public virtual bool Connect(ConnectOptions connectOptions, IPAddress remoteAddr, int port)
        {

            // Translate Address:Port from configuration file into a local end-point.
            RemoteEP = new IPEndPoint(remoteAddr, port);
            RemotePort = port;

            EtaLogger.Instance.Trace($"{DateTime.UtcNow:HH:mm:ss.ffff} MockChannel({ChannelId})::BeginConnect Address: {remoteAddr}, Port: {port}");

            Task task = Task.Factory.StartNew(() =>
                {
                    // Pretend to receive a ReplyAck following Connect
                    RipcConnectionReplyAck replyAck = default(RipcConnectionReplyAck);
                    replyAck.RipcReply.MessageLength = replyAck.MessageLength;
                    replyAck.RipcReply.Flags = RipcFlags.HAS_OPTIONAL_FLAGS;
                    replyAck.RipcReply.OpCode = RipcOpCode.CONNECT_ACK;
                    replyAck.RipcVersion = RipcVersionInfo.CurrentVersion;
                    replyAck.MaxUserMsgSize = RipcVersionInfo.MaxUserMsgSize;
                    replyAck.MajorVersion = RipcVersionInfo.MajorVersion;
                    replyAck.MinorVersion = RipcVersionInfo.MinorVersion;
                    ByteBuffer byteBuffer = new ByteBuffer(replyAck.MessageLength);
                    byteBuffer.Write(replyAck);

                    var aro = new ResultObject(null, byteBuffer);

                    if (port == 0)
                        RaiseConnected(aro);

                    if (port == MockProtocol.PortActionAfter50ms)
                    {
                        Thread.Sleep(50);
                        RaiseConnected(aro);
                    }

                    if (port == MockProtocol.PortActionOnSignal)
                    {
                        do
                        {
                            Thread.Sleep(0);
                            if (IsConnected)
                                // Allow external probe to mark "IsConnected"
                                RaiseConnected(aro);
                        } while (!IsConnected);
                    }

                });

            return true;
        }

        public bool Connect(ConnectOptions connectOptions)
        {
            IPHostEntry ipHostInfo = Dns.GetHostEntry(connectOptions.UnifiedNetworkInfo.Address);
            IPAddress ipAddress = ipHostInfo.AddressList[0];
            return Connect(connectOptions, ipAddress, connectOptions.UnifiedNetworkInfo.Port);
        }

        public void Disconnect()
        {
            Task task = Task.Factory.StartNew(() =>
            {
                Thread.Sleep(10);
                RaiseDisconnected();
            });
        }

        public bool Recv(object user_state)
        {
            ResultObject aro = new ResultObject(null, 8192, user_state);
            return Recv(aro);
        }

        public bool Recv(ResultObject aro)
        {
            bool status = false;

            if (!_isDisposed)
            {
                try
                {
                    EtaLogger.Instance.Trace($"{ DateTime.UtcNow:HH:mm:ss.ffff} MockChannel({ChannelId})::BeginRecv UserState: {aro.UserState?.ToString() ?? "*null*"}");

                    // Initiate the return read on this socket.
                    byte[] packet = aro.Buffer.Contents;

                    Task task = Task.Factory.StartNew(() =>
                    {
                        IsDataReady = false;

                        if (RemotePort == 0)
                            RaiseDataReady(aro);

                        if (RemotePort == MockProtocol.PortActionAfter50ms)
                        {
                            Thread.Sleep(50);
                            RaiseDataReady(aro);
                        }

                        if (RemotePort == MockProtocol.PortActionOnSignal)
                        {
                            do
                            {
                                Thread.Sleep(1);
                                if (IsDataReady)
                                    // Allow external probe to mark "IsDataReady"
                                    RaiseDataReady(aro);
                            } while (!IsDataReady);
                        }

                    });

                    status = true;
                }
                catch (Exception exp)
                {
                    EtaLogger.Instance.Error(exp);
                    RaiseError(aro, exp.Message);
                    status = false;
                }
            }
            return status;
        }

        public void Send(byte[] packet, object user_state)
        {
            if (!_isDisposed)
            {
                var aro = new ResultObject(null, packet, user_state);
                EtaLogger.Instance.Trace($"{ DateTime.UtcNow:HH:mm:ss.ffff} MockChannel({ChannelId})::BeginSend AsynchResultObject: {aro}");

                RaiseDataSent(aro);
            }
            return;
        }
        bool _isDisposed = false;

        public void Dispose()
        {
            Dispose(true);

            GC.SuppressFinalize(this);
        }

        public void Dispose(bool disposedCalled)
        {
            if (_isDisposed)
                return;

            _isDisposed = true;

            if (disposedCalled)
            {
                OnConnected = null;
                OnDisconnected = null;
                OnError = null;
                OnDataReady = null;
                OnDataSent = null;
            }
        }

        public int Send(byte[] buffer, int position, int length)
        {
            throw new NotImplementedException();
        }

        public SocketError Receive(ResultObject aro)
        {
            throw new NotImplementedException();
        }
    }


    #endregion
}

