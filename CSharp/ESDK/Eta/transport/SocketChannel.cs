/*|-----------------------------------------------------------------------------
 *|            This source code is provided under the Apache 2.0 license      --
 *|  and is provided AS IS with no warranty or guarantee of fit for purpose.  --
 *|                See the project's LICENSE.md for details.                  --
 *|           Copyright Thomson Reuters 2018. All rights reserved.            --
 *|-----------------------------------------------------------------------------
 */

using System;
using System.Diagnostics;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Text;

using ThomsonReuters.Common.Logger;

namespace ThomsonReuters.Eta.Transports
{
    /////////////////////////////////////////////////////////////////////////
    ///
    /// <summary>
    ///   Aggregates Socket object; maintains connection between this object and event sinks.
    /// </summary>
    /// 
    internal class SocketChannel : IDisposable, ISocketChannel
    {
        /// <summary>
        /// Controls appearence of deep-debugging displays. Manually set during development
        /// by developer.
        /// </summary>
        private static bool debug = true;

        #region Delegates/Events

        /// <summary>
        ///   Raised when connection state of the channel changes to CONNECTED.
        /// </summary>
        public event ConnectionStateChangeHandler OnConnected;

        /// <summary>
        ///   Raised when connection state of the channel changes to DISCONNECTED.
        /// </summary>
        public event ConnectionStateChangeHandler OnDisconnected;

        /// <summary>
        /// Raised when an error has been detected during processing.
        /// </summary>
        public event ConnectionStateChangeHandler OnError;

        #endregion

        #region Properties

        /// <summary>
        /// Returns true if Socket exists and is active. 
        /// </summary>
        public bool IsConnected
        {
            get => m_socket != null && m_socket.Connected;
            set => throw new InvalidOperationException();
        }

        /// <summary>
        /// Returns true if SocketChanel is a 'zombie' object: disposed, but not yet release from GC heap. 
        /// </summary>
        public bool IsDisposed => m_alreadyDisposed;

        /// <summary>
        /// Role served by this channel.
        /// <list>
        ///   <item>
        ///      <term>
        ///         true
        ///      </term>
        ///      <description>
        ///         Created from SocektServer::Accept call.
        ///      </description>
        ///   </item>
        ///   <item>
        ///   <term>
        ///      false
        ///   </term>
        ///   <description>
        ///      Created from SocketChannel::SocketChannel.
        ///   </description>
        ///   </item>
        /// </list>
        /// </summary>
        public bool IsServer
        {
            get;
            //return m_socket;
        }

        public bool IsDataReady
        {
            get => m_socket != null && m_socket.Available > 0;
            set => throw new InvalidOperationException("SocketChannel cannot set IsDataReady");
        }

        /// <summary>
        /// Entry-POint at other end of Socket
        /// </summary>
        public EndPoint RemoteEP => m_remote_ep;

        public Guid ChannelId { get; } = Guid.NewGuid();

        public Socket Socket { get => m_socket; }

        #endregion

        #region ToString

        /// <summary>
        /// Provides some reasonable string representation
        /// </summary>
        /// <returns>presentation string</returns>
        public override string ToString()
        {
            var s = m_remote_ep != null ? m_remote_ep.ToString() : "<Undefined>";
            return s;
        }

        /// <summary>
        /// Port number of remote end-point.
        /// </summary>
        public int RemotePort
        {
            get
            {
                if (m_remote_ep == null)
                    return 0x0;
                SocketAddress address = m_remote_ep.Serialize();
                return Convert.ToInt16(address[2]) * 256 + Convert.ToInt16(address[3]);
            }
        }

        #endregion

        #region Instance Vbls

        /// <summary>
        /// Suppress certain actions if we are in the proces of being torn-down.
        /// </summary>
        private bool m_alreadyDisposed;

        /// <summary>
        /// Holding this will allow us to tear the TCP/IP connection down explicitly.
        /// </summary>
        private Socket m_socket;
        private EndPoint m_remote_ep;

        private int m_receive_packet_size;

        private readonly int defaultSystemBufferSize = 16384;

        #endregion

        /// <summary>
        /// 
        /// </summary>
        public SocketChannel()
        {
            m_receive_packet_size = ResultObject.DefaultBufferSize;
        }

        /// <summary>
        /// Destructor / Finalizer. Because Dispose() calls GC.SupressFinalize(), this method
        /// is only called by the garbage collection process if the consumer of the object does
        /// not call Dispose() as they should.
        /// </summary>
        ~SocketChannel()
        {
            // Call the Dispose() method as opposed to duplicating the code to clean up any unmanaged
            // resources. Use the protected Dispose() overload and pass a value of "false" to indicate
            // that Dispose() is being called during the garbage collection process, not by consumer
            // code.
            Dispose(false);
        }

        /// <summary>
        /// Initiate a connection to the remote node.
        /// </summary>
        /// <param name="connectOptions"></param>
        /// <param name="remoteAddr">
        ///   IP Address in string form. This string will be resolved by DNS into a IPEndPoint object.
        /// </param>
        /// <param name="port">The IP Port number we are trying to reach at the remote node.</param>
        public bool Connect(ConnectOptions connectOptions, IPAddress remoteAddr, int port)
        {
            ResultObject aro = null;
            bool status;
            try
            {
                if (IsServer)
                    return false;

                // Create TCP/IP Socket
                m_socket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, System.Net.Sockets.ProtocolType.Tcp)
                {
                    Blocking = true,
                    NoDelay = connectOptions.TcpOpts.TcpNoDelay,
                    ReceiveBufferSize = connectOptions.SysRecvBufSize == 0 ? defaultSystemBufferSize : connectOptions.SysRecvBufSize * 1000,
                    SendBufferSize = connectOptions.SysSendBufSize == 0 ? defaultSystemBufferSize : connectOptions.SysSendBufSize * 1000
                };

                // Translate Address:Port from configuration file into a local end-point.
                IPEndPoint remote_ep = new IPEndPoint(remoteAddr, port);

                aro = new ResultObject(m_socket, m_receive_packet_size, null);

                status = true;

                m_socket.Connect(remote_ep);

                m_remote_ep = m_socket.RemoteEndPoint;

                RaiseConnected(new SocketEventArgs(aro.UserState, null));
            }
            catch (Exception exp)
            {
                Trace.TraceError(@"{0} : {1}", aro, exp.Message);
                RaiseError(aro, exp.Message);
                status = false;
            }
            return status;
        }

        public bool Connect(ConnectOptions connectOptions)
        {
            IPHostEntry hostEntry = null;

            m_receive_packet_size = (connectOptions.SysRecvBufSize > 0)
                                                        ? connectOptions.SysRecvBufSize 
                                                        : ResultObject.DefaultBufferSize;

            if (!IPAddress.TryParse(connectOptions.UnifiedNetworkInfo.Address, out IPAddress ipAddress))
            {
                try { hostEntry = Dns.GetHostEntry(connectOptions.UnifiedNetworkInfo.Address); } catch (Exception) { }
                if (hostEntry != null)
                {
                    var remoteHost = new System.Net.IPHostEntry
                    {
                        AddressList = (from addr in hostEntry.AddressList
                                       where addr.AddressFamily == System.Net.Sockets.AddressFamily.InterNetwork
                                       select addr).ToArray()
                    };

                    ipAddress = remoteHost.AddressList[0];

                    return Connect(connectOptions, ipAddress, connectOptions.UnifiedNetworkInfo.Port);
                }

                RaiseError(null, $"Could not parse Address [{connectOptions.UnifiedNetworkInfo.Address}]");
                return false;
            }
            else
            {
                return Connect(connectOptions, ipAddress, connectOptions.UnifiedNetworkInfo.Port);
            }
        }

        /// <summary>
        /// Shutdown all outstanding I/O requests.
        /// </summary>
        public void Disconnect()
        {
            try
            {
                if (m_socket != null && m_socket.Connected)
                {
                    m_socket.Shutdown(SocketShutdown.Both);
                    RaiseDisconnected();
                }
            }
            catch (Exception exp)
            {
                Trace.TraceError(exp.Message);
                RaiseError(null, exp.Message);
            }
        }

        #region Send

        /// <summary>
        /// Send a binary packet synchronously
        /// </summary>
        /// <param name="packet">Payload to send out on the socket.</param>
        /// <param name="user_state"></param>
        public void Send(byte[] packet, object user_state)
        {
            // throw an exception if this object has already been disposed
            if (m_alreadyDisposed)
            {
                throw new ObjectDisposedException("ErrorHandler");
            }

            ResultObject wro = null;
            try
            {
                wro = new ResultObject(m_socket, packet, user_state);
                byte[] ar = wro.Buffer.Contents;

                int byteSent = m_socket.Send(ar, wro.Buffer.Position, ar.Length - wro.Buffer.Position, SocketFlags.None);

                OnSocketWrite(wro, byteSent);
            }
            catch (Exception exp)
            {
                Trace.TraceError(@"{0} : {1}", wro, exp.Message);
                RaiseError(wro, exp.Message);
            }
        }

        public int Send(byte[] buffer, int position, int length)
        {
            int byteWritten = -1;

            try
            {
                if (m_socket == null)
                    throw new SocketException(10038); // WSAENOTSOCK

                byteWritten = m_socket.Send(buffer, position, length, SocketFlags.None);
            }
            catch (Exception exp)
            {
                var wro = new ResultObject(m_socket, buffer);
                Trace.TraceError(@"{0} : {1}", wro, exp.Message);
                RaiseError(wro, exp.Message);
            }

            return byteWritten;
        }

        #endregion

        #region Receive

        public SocketError Receive(ResultObject resultObject)
        {
            SocketError retSocketError = SocketError.Success;
            try
            {
                if (m_socket == null)
                    throw new SocketException(10038); // WSAENOTSOCK

                if (resultObject.InitiatingSocket == null)
                    resultObject.InitiatingSocket = this.m_socket;

                EtaLogger.Instance.Trace($"{DateTime.UtcNow:HH:mm:ss.ffff} SocketChannel::Receive UserState: {resultObject.UserState}");

                // Initiate the return read on this socket.
                int retResult = 0;

                retResult = m_socket.Receive(resultObject.Buffer.Contents, resultObject.Buffer.WritePosition, 
                    resultObject.Buffer.Capacity - resultObject.Buffer.WritePosition, SocketFlags.None);

                if (retResult > 0)
                    resultObject.Buffer.WritePosition += retResult;
                else
                {
                    EtaLogger.Instance.Trace($"{DateTime.UtcNow:HH:mm:ss.ffff} SocketChannel::Receive The connection is closed by remote peer.");
                    Disconnect();
                }

                EtaLogger.Instance.Trace($"{DateTime.UtcNow:HH:mm:ss.ffff} SocketChannel::Receive{{ retReturn = {retResult}}}");
            }
            catch(SocketException socketException)
            {
                retSocketError = socketException.SocketErrorCode;

                if (retSocketError != SocketError.WouldBlock)
                {
                    retSocketError = socketException.SocketErrorCode;
                    Trace.TraceError(socketException.Message);
                    RaiseError(resultObject, socketException.Message);
                }
            }
            catch (Exception exception)
            {
                Trace.TraceError(exception.Message);
                RaiseError(resultObject, exception.Message);

                retSocketError = SocketError.SocketError;
            }

            return retSocketError;
        }

        #endregion

        #region Socket Event Handlers


        /// <summary>
        /// Complete 'Send' request.
        /// </summary>
        /// <param name="iar">State object: AsyncResultObject.</param>
        /// <param name="byteSent"></param>
        private void OnSocketWrite(ResultObject iar, int byteSent)
        {
            ResultObject aro = null;
            try
            {
                // Fetch the results.
                aro = iar;

                // Terminate the Send call
                int bytes_sent = byteSent;
                if (m_alreadyDisposed)
                    return;

                if (aro.Buffer.Limit != bytes_sent)
                    throw new ApplicationException($"Error on socket write; Buffer.Length={aro.Buffer.Limit},Sent={bytes_sent}");
            }
            catch (Exception exp)
            {
                m_socket.Shutdown(SocketShutdown.Receive);
                Trace.TraceError(@"{0} : {1}", aro, exp.Message);
                RaiseError(aro, exp.Message);
            }
        }

        #endregion

        #region Raise Events

        /// <summary>
        /// Fire the OnConnected event.
        /// </summary>
        /// <returns></returns>
        private void RaiseConnected(SocketEventArgs sea)
        {
            if (debug)
            {
                StringBuilder sb = new StringBuilder();
                sb.AppendFormat("Channel connecting to {0}", m_remote_ep.ToString());
                Trace.WriteLine(debug, sb.ToString());
            }

            OnConnected?.Invoke(this, sea);
        }

        /// <summary>
        /// Fire the OnDisconnected event. <see cref="ResultObject"/>
        /// </summary>
        private void RaiseDisconnected()
        {
            Trace.WriteLine(String.Format("Channel Disconnecting from {0}",
               m_remote_ep.ToString()));

            OnDisconnected?.Invoke(this, new SocketEventArgs(null, ""));
        }

        /// <summary>
        /// Fire OnError event.
        /// </summary>
        /// <param name="aro"></param>
        /// <param name="message"></param>
        private void RaiseError(ResultObject aro, string message)
        {
            StringBuilder sb = new StringBuilder();
            sb.AppendFormat("SocketChannel::RaiseError:  {0}", message);
            Trace.WriteLine(sb.ToString());

            OnError?.Invoke(this, new SocketEventArgs(aro.UserState, message));
        }

        #endregion

        #region IDisposable Members

        /// <summary>
        /// Public implementation of the IDisposable.Dispose() method, called by the consumer of the
        /// object in order to free unmanaged resources deterministically.
        /// </summary>
        public void Dispose()
        {
            // Call the protected Dispose() overload and pass a value of "true" to indicate that
            // Dispose() is being called by consumer code, not by the garbage collector.
            Dispose(true);

            // Because the Dispose() method performs all necessary cleanup, ensure that the garbage
            // collector does not call the class destructor.
            GC.SuppressFinalize(this);
        }

        /// <summary>
        /// Release any resources held by this object.
        /// </summary>
        /// <param name="isDisposing">
        /// <list>
        /// <listheader>Denotes who called this method:</listheader>
        /// <term>true</term>
        /// <description>Client of this class.</description>
        /// <term>false</term>
        /// <description>GC through the finalizer.</description>
        /// </list>
        /// </param>
        protected virtual void Dispose(bool isDisposing)
        {
            try
            {
                if (m_alreadyDisposed)
                    return;

                StringBuilder sb = new StringBuilder();
                sb.AppendFormat("Channel disposing {0}", ToString());
                Trace.WriteLineIf(debug, sb.ToString());

                m_alreadyDisposed = true;
                if (isDisposing)
                {  // Release the delegates - besides we don't want them firing anyway.
                    OnConnected = null;
                    OnDisconnected = null;
                    OnError = null;

                    if (m_socket != null)
                    {
                        if (m_socket.Connected)
                            m_socket.Shutdown(SocketShutdown.Both);

                        // Dispose the Socket
                        m_socket.Close();
                    }
                }
            }
            catch (Exception exp)
            {
                Trace.TraceError(exp.Message);
            }
        }

        #endregion
    }

}


