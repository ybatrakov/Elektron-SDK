/*|-----------------------------------------------------------------------------
 *|            This source code is provided under the Apache 2.0 license      --
 *|  and is provided AS IS with no warranty or guarantee of fit for purpose.  --
 *|                See the project's LICENSE.md for details.                  --
 *|           Copyright Thomson Reuters 2018. All rights reserved.            --
 *|-----------------------------------------------------------------------------
 */

using System;
using System.Diagnostics;
using System.IO;
using System.Threading;


using ThomsonReuters.Eta.Transports;
using ThomsonReuters.Eta.Transports.Interfaces;
using ThomsonReuters.Common.Logger;
using ThomsonReuters.Eta.Common;
using ThomsonReuters.Eta.Transports.Internal;
using System.Net.Sockets;

namespace ThomsonReuters.Eta.Internal.Interfaces
{
    /// <summary>
    /// Channel state change delegate.
    /// </summary>
    internal delegate bool ConnectionStateChangeHandler(ChannelBase sender, EventArgs sea);

    internal class ChannelBase : IChannel, IInternalChannel
    {
        public class UserState
        {
            long Stamp;

            long Count;

            public UserState(long count)
            {
                Stamp = DateTime.UtcNow.Ticks;
                Count = count;
            }

            public override string ToString()
            {
                return $"{new DateTime(Stamp):HH:mm:ss.ffffff} {Count}";
            }
        }

        #region SocketChannel

        private readonly ISocketChannel _socketChannel;
        public ISocketChannel SocketChannel { get => _socketChannel; }

        #endregion

        private readonly AutoResetEvent _connectEvent = new AutoResetEvent(false);
        private readonly AutoResetEvent _disconnectedEvent = new AutoResetEvent(false);
        private readonly ManualResetEvent _initializedEvent = new ManualResetEvent(false); // Only set After Ack or Nack received
        internal static readonly int IPC_DATA = 0x2; // normal, uncompressed data 

        private Locker _readLocker;
        private Locker _writeLocker;

        internal ResultObject _resultObject;

        #region Ping Buffer

        private static ByteBuffer _pingBuffer;
        private ByteBuffer PingBuffer
        {
            get
            {
                if (_pingBuffer == null)
                {
                    _pingBuffer = new ByteBuffer(RipcDataMessage.HeaderSize);

                    // pre-populate ping message since always the same
                    _pingBuffer.Write((short)RipcDataMessage.HeaderSize); // ripc header length
                    _pingBuffer.Write((byte)RipcFlags.DATA);              // ripc flag indicating data
                }
                return _pingBuffer;
            }
        }
        #endregion

        private ByteBuffer _readBuffer;

        private TransportBufferPool _transportBufferPool;

        internal ConnectOptions ConnectionOptions { get; }

        internal Guid ChannelId { get; } = Guid.NewGuid();

        internal long ThreadId { get; }

        internal ChannelBase(ConnectOptions connectionOptions, ISocketChannel socketChannel)
        {
            _socketChannel = socketChannel;
            _socketChannel.OnConnected += SocketOnConnected;
            _socketChannel.OnDisconnected += SocketOnDisconnected;
            _socketChannel.OnError += SocketOnError;

            ThreadId = Thread.CurrentThread.ManagedThreadId;
            ConnectionOptions = connectionOptions;
            State = ChannelState.INACTIVE;

            MajorVersion = connectionOptions.MajorVersion;
            MinorVersion = connectionOptions.MinorVersion;

            UserSpecObject = connectionOptions.UserSpecObject;

            _readLocker = ConnectionOptions.ChannelReadLocking
               ? (Locker)new WriteLocker(new ReaderWriterLockSlim(LockRecursionPolicy.SupportsRecursion))
               : (Locker)new NoLocker();

            _writeLocker = ConnectionOptions.ChannelWriteLocking
                         ? (Locker)new WriteLocker(new ReaderWriterLockSlim(LockRecursionPolicy.SupportsRecursion))
                         : (Locker)new NoLocker();

            _transportBufferPool = new TransportBufferPool(this);
        }

        Lazy<BinaryWriter> _logStream = new Lazy<BinaryWriter>(() => {
            var stream = new FileStream($"eta.channelbase.{DateTime.UtcNow:yyyyMMdd-HHmmss}.etastream", FileMode.Create, FileAccess.Write);

            return new BinaryWriter(stream);
        });

        static byte[] _lineGuard = new byte[] { 0xBA, 0xAD, 0xF0, 0x0D };
        private void WriteToLog(ResultObject resultObject)
        {
            _logStream.Value.Write(_lineGuard);
            _logStream.Value.Write(BitConverter.GetBytes(DateTime.UtcNow.Ticks));
            _logStream.Value.Write(BitConverter.GetBytes(resultObject.Buffer.Capacity));
            _logStream.Value.Write(BitConverter.GetBytes(resultObject.Buffer.ReadPosition));
            _logStream.Value.Write(BitConverter.GetBytes(resultObject.Buffer.WritePosition));
            _logStream.Value.Write(resultObject.Buffer.Contents);
        }

        #region IChannel

        public TransportReturnCode Init(out Error error)
        {
            error = null;

            if (State == ChannelState.ACTIVE)
                return TransportReturnCode.SUCCESS;

            ResultObject resultObject = new ResultObject(_socketChannel.Socket, ConnectionOptions.NumInputBuffers, null);

            _socketChannel.Receive(resultObject);

            TryToReceiveAckOrNack(resultObject);

            if (State != ChannelState.ACTIVE)
            {
                error = new Error
                {
                    ErrorId = TransportReturnCode.FAILURE,
                    Text = "RIPC handshake error.",
                    Channel = this
                };

                return TransportReturnCode.FAILURE;
            }
            else
            {
                resultObject.Buffer.Clear();
                _resultObject = resultObject;
                return TransportReturnCode.SUCCESS;
            }
        }

        public ITransportBuffer Read(ReadArgs readArgs, out Error error)
        {
            _readLocker.Enter();

            try
            {
                error = null;
                TransportBuffer transportBuffer = null;
                SocketError socketError;

                if (State != ChannelState.ACTIVE)
                {
                    readArgs.BytesRead = -1;
                    readArgs.ReadRetVal = TransportReturnCode.FAILURE;
                    error = new Error(TransportReturnCode.FAILURE, "Channel is not active", channel: this);
                    return null;
                }

                if (IsDataBufferEmpty(_resultObject))
                {
                    // Read data synchronously from the socket
                    socketError = SocketChannel.Receive(_resultObject);

                    if (HandleSocketReceiveRet(ref socketError, ref readArgs, out error) == false)
                        return null;

                    if (CheckHasNextRipcMessage(_resultObject))
                    {
                        // Parse the RIPC message
                        LoadReadBuffer(_resultObject);
                    }
                    else
                    {
                        IsMessageReady = false;
                        readArgs.ReadRetVal = TransportReturnCode.READ_MORE_DATA;
                        return transportBuffer;
                    }
                }
                else
                {
                    if (CheckHasNextRipcMessage(_resultObject) == false)
                    {
                        //Read data synchronously from the socket
                        socketError = SocketChannel.Receive(_resultObject);

                        if (HandleSocketReceiveRet(ref socketError, ref readArgs, out error) == false)
                            return null;

                        if (CheckHasNextRipcMessage(_resultObject))
                        {
                            // Parse the RIPC message
                            LoadReadBuffer(_resultObject);
                        }
                        else
                        {
                            IsMessageReady = false;
                            readArgs.ReadRetVal = TransportReturnCode.READ_MORE_DATA;
                            return transportBuffer;
                        }
                    }
                }

                try
                {
                    if (IsMessageReady)
                    {
                        EtaLogger.Instance.Trace($"Channelbase::Read ReadBuffer: {{{_readBuffer}}}");

                        // Load a RIPC message into the TransportBuffer
                        transportBuffer = TransportBuffer.Load(ref _readBuffer);
                        readArgs.BytesRead = transportBuffer != null ? transportBuffer.Data.Position : 0;

#if DEBUG
                        if (readArgs.BytesRead > 0)
                            Interlocked.Increment(ref _messagesReadCount);
#endif

                        if (readArgs.BytesRead == RipcDataMessage.HeaderSize)
                        {
                            readArgs.ReadRetVal = TransportReturnCode.READ_PING;
                        }
                        else
                        {
                            if (_readBuffer.ReadPosition == _readBuffer.WritePosition || readArgs.BytesRead == 0)
                            {
                                readArgs.ReadRetVal = TransportReturnCode.SUCCESS;
                            }
                            else
                            {
                                readArgs.ReadRetVal = TransportReturnCode.READ_MORE_DATA;
                            }
                        }

                        readArgs.UncompressedBytesRead = readArgs.BytesRead;
                        // Return the message
                        return transportBuffer;
                    }
                }
                catch (Exception exp)
                {
                    EtaLogger.Instance.Information($"     Read Buffer: {_readBuffer}");
                    EtaLogger.Instance.Information($"Transport Buffer: {transportBuffer?.ToString() ?? "*null*"}");
                    error = new Error(TransportReturnCode.PACKET_GAP_DETECTED, text: exp.Message, channel: this, exception: exp);
                }

                readArgs.UncompressedBytesRead = readArgs.BytesRead;
                readArgs.ReadRetVal = TransportReturnCode.READ_MORE_DATA;
                return transportBuffer;
            }
            finally
            {
                _readLocker.Exit();
            }
        }

        public TransportReturnCode Close(out Error error)
        {
            error = null;

            try
            {
                if (State != ChannelState.CLOSED)
                {
                    _socketChannel.Disconnect();

#if DEBUG
                    EtaLogger.Instance.Information($"ChannelBase.Close MessagesReadCount: {MessagesReadCount}");
#else
                    EtaLogger.Instance.Information($"ChannelBase.Close");
#endif

                    if (_disconnectedEvent.WaitOne(100))
                        return TransportReturnCode.SUCCESS;

                    error = new Error(errorId: TransportReturnCode.FAILURE,
                                         text: "Channel::Close time-out",
                                      channel: this);
                    return error.ErrorId;
                }
            }
            finally
            {
                State = (State == ChannelState.INITIALIZING)
                            ? ChannelState.INACTIVE
                            : ChannelState.CLOSED;

                _socketChannel.Dispose();
            }

            return TransportReturnCode.SUCCESS;
        }

        public ChannelState State
        { get; private set; } = ChannelState.INACTIVE;

#if DEBUG
#region MessagesReadCount

        private long _messagesReadCount = 0;
        public long MessagesReadCount
        { get => Interlocked.Read(ref _messagesReadCount); }

#endregion
#endif

        public bool IsMessageReady { get; private set; }

#if DEBUG
        public long LastReceivedStamp { get; private set; }
#endif

        public int MajorVersion
        {
            get; internal set;
        }

        public int MinorVersion
        {
            get; internal set;
        }

        public Socket Socket { get; private set; }

        public Transports.ProtocolType ProtocolType {get; private set; }

        public int PingTimeOut { get; private set; }

        public object UserSpecObject { get; private set; }

        public bool Blocking { get; private set; }

        public ConnectionType ConnectionType { get; private set; }

#endregion

        internal virtual void Connect()
        {
            if ( _transportBufferPool.InitPool( ConnectionOptions.NumGuaranteedOutputBuffers,
                ConnectionOptions.NumGuaranteedOutputBuffers * ( ( ConnectionOptions.GuaranteedOutputBuffers + RipcDataMessage.HeaderSize) ),
                ( ConnectionOptions.GuaranteedOutputBuffers + RipcDataMessage.HeaderSize ), out Error error ) != TransportReturnCode.SUCCESS )
            {
                throw new TransportException($"{typeof(ISocketChannel).Name}.{error.Text}");
            }

            bool res = _socketChannel.Connect(ConnectionOptions);

            Socket = _socketChannel.Socket;

            if (!res)
                throw new TransportException($"{typeof(ISocketChannel).Name}.Connect failed.");

            SendConnectRequest();
        }

#region SocketChannel Event Handlers

        private bool SocketOnDisconnected(ISocketChannel sender, SocketEventArgs sea)
        {
            State = ChannelState.CLOSED;
            _disconnectedEvent.Set();
            EtaLogger.Instance.Information($"ChannelBase.OnSocketDisconnected {sea}");
            return true;
        }

        /// <summary>
        /// When underlying SocketChannel signals CONNECTED 
        /// </summary>
        private bool SocketOnConnected(ISocketChannel sender, SocketEventArgs sea)
        {
            _connectEvent.Set();
            State = ChannelState.INITIALIZING;
            EtaLogger.Instance.Information($"ChannelBase.SocketOnConnected {sea}");
            return true;
        }

        private bool SocketOnError(ISocketChannel sender, SocketEventArgs sea)
        {
            EtaLogger.Instance.Error($"ChannelBase::SocketError: {{{sea.Context}}}");
            Close(out Error err);
            return true;
        }

#endregion

        private bool IsDataBufferEmpty(ResultObject resultObject)
        {
            if (_resultObject.Buffer.ReadPosition == _resultObject.Buffer.WritePosition)
            {
                _resultObject.Buffer.Compact();
                return true;
            }
            else
            {
                return false;
            }
        }

        private bool CheckHasNextRipcMessage(ResultObject resultObject)
        {
            RipcDataMessage dataMessage = default(RipcDataMessage);

            return _resultObject.Buffer.HasMoreMessage(ref dataMessage, resultObject.Buffer.ReadPosition);
        }

        private void LoadReadBuffer(ResultObject resultObject)
        {
            EtaLogger.Instance.Trace($"{DateTime.UtcNow:HH:mm:ss.ffff} ChannelBase::LoadReadBuffer: UserState: {resultObject?.UserState ?? "*null*"}, Buffer: {resultObject.Buffer} ");

#if DEBUG
            LastReceivedStamp = DateTime.UtcNow.Ticks;
#endif
            RipcDataMessage dataMessage = default(RipcDataMessage);

            if (resultObject.Buffer.ReadAt(ref dataMessage, resultObject.Buffer.ReadPosition, false))
            {
                _readBuffer = resultObject.Buffer;
                IsMessageReady = true;
            }
            else
            {
                throw new InvalidOperationException($"ChannelBase::ReadMessage: *Broken Packe...");
            }
        }

        private void TryToReceiveAckOrNack(ResultObject resultObject)
        {
            ByteBuffer buffer = resultObject.Buffer;

            buffer.Flip();

            // Read the Preamble fom the channel into a ByteBuffer...
            // hot glue - assuming all packet came
            RipcConnectionReply reply = default(RipcConnectionReply);
            try
            {
                buffer.Read(ref reply);

                buffer.Rewind();
                switch (reply.OpCode)
                {
                    case RipcOpCode.CONNECT_ACK:
                        RipcConnectionReplyAck replyAck = default(RipcConnectionReplyAck);
                        buffer.Read(ref replyAck);
                        PingTimeOut = replyAck.PingTimeout;
                        State = ChannelState.ACTIVE;
                        _initializedEvent.Set();
                        EtaLogger.Instance.Trace($"{DateTime.UtcNow:HH:mm:ss.ffff} ChannelBase::TryToReceiveAckOrNack: ReplyAck: {replyAck}");
                        break;

                    case RipcOpCode.CONNECT_NAK:
                        RipcConnectionReplyNak replyNak = default(RipcConnectionReplyNak);
                        buffer.Read(ref replyNak);
                        State = ChannelState.INACTIVE;
                        EtaLogger.Instance.Trace($"{DateTime.UtcNow:HH:mm:ss.ffff} ChannelBase::TryToReceiveAckOrNack: ReplyNak: {replyNak}");
                        break;

                    default:
                        EtaLogger.Instance.Error($"{DateTime.UtcNow:HH:mm:ss.ffff} ChannelBase::TryToReceiveAckOrNack: Invalid Response {resultObject}");
                        break;
                }

                Debug.Assert(!buffer.HasRemaining, "Must not be any messages here."); // no more messages must  be here at this point

                buffer.Compact();
            }
            catch (InvalidOperationException ioex)
            {
                EtaLogger.Instance.Error($"{ioex.Message} - RipcReply: {reply}");
            }
        }

        private void SendConnectRequest()
        {
            RipcConnectionRequest connectionRequest = default(RipcConnectionRequest);
            connectionRequest.Flags = RipcFlags.NONE;
            connectionRequest.ConnectionVersion = RipcVersionInfo.CurrentVersion;
            connectionRequest.PingTimeout = RipcVersionInfo.PingTimeout;
            connectionRequest.SessionFlags = RipcSessionFlag.NONE;
            connectionRequest.ProtocolType = Transports.ProtocolType.RWF;
            connectionRequest.MajorVersion = RipcVersionInfo.MajorVersion;
            connectionRequest.MinorVersion = RipcVersionInfo.MinorVersion;
            connectionRequest.ComponentVersionString = "eta.net preview version";

            string hostName = "";

            try { hostName = System.Net.Dns.GetHostName(); } catch (Exception) { }

            connectionRequest.HostName = hostName;

            ByteBuffer buffer = new ByteBuffer(connectionRequest.MessageLength);

            buffer.Write(connectionRequest);

            _socketChannel.Send(buffer.Contents);
        }

        private bool HandleSocketReceiveRet(ref SocketError socketError, ref ReadArgs readArgs, out Error error)
        {
            error = null;

            if (socketError != SocketError.Success)
            {
                if (socketError == SocketError.WouldBlock)
                {
                    readArgs.BytesRead = -1;
                    readArgs.ReadRetVal = TransportReturnCode.READ_WOULD_BLOCK;
                    return false;
                }
                else
                {
                    readArgs.BytesRead = -1;
                    readArgs.ReadRetVal = TransportReturnCode.FAILURE;
                    error = new Error(TransportReturnCode.FAILURE, "Fails to read data from socket", channel: this);
                    return false;
                }
            }
            return true;
        }

        public override string ToString()
        {
#if DEBUG
            return $"ReceivedCount: {MessagesReadCount}, ConnectionType: {ConnectionOptions.ConnectionType}, Blocking: {ConnectionOptions.Blocking}, State: {State}, IsMessageReady: {IsMessageReady}, ThreadId: {ThreadId}";
#else
            return $"ConnectionType: {ConnectionOptions.ConnectionType}, Blocking: {ConnectionOptions.Blocking}, State: {State}, IsMessageReady: {IsMessageReady}, ThreadId: {ThreadId}";
#endif
        }

        public ITransportBuffer GetBuffer(int size, bool packedBuffer, out Error error)
        {
            error = null;
            ITransportBuffer transportBuffer = null;
 
            transportBuffer = _transportBufferPool.GetTransprotBuffer(size + RipcDataMessage.HeaderSize, out error);

            if(error != null )
            {
                error.Channel = this;
            }
            
            return transportBuffer;
        }

        public TransportReturnCode Write(ITransportBuffer msgBuf, WriteArgs writeArgs, out Error error)
        {
            _writeLocker.Enter();

            try
            {
                writeArgs.Clear();
                TransportReturnCode ret = TransportReturnCode.SUCCESS;
                error = null;

                if (msgBuf.Length == 0)
                {
                    error = new Error
                    {
                        Channel = this,
                        ErrorId = TransportReturnCode.FAILURE,
                        SysError = 0,
                        Text = "Buffer of length zero cannot be written"
                    };

                    return TransportReturnCode.FAILURE;
                }

                if (State != ChannelState.ACTIVE)
                {
                    error = new Error
                    {
                        Channel = this,
                        ErrorId = TransportReturnCode.FAILURE,
                        SysError = 0,
                        Text = "Channel is not in active state for write"
                    };

                    return TransportReturnCode.FAILURE;
                }

                msgBuf.Data.WriteAt(0, (short)(msgBuf.Length + RipcDataMessage.HeaderSize));  // Populate RIPC message length
                msgBuf.Data.WriteAt(2, (byte)RipcFlags.DATA);  // Populate RIPC message header

                int byteWritten = _socketChannel.Send(msgBuf.Data.Contents, msgBuf.DataStartPosition - RipcDataMessage.HeaderSize,
                                                                                msgBuf.Length + RipcDataMessage.HeaderSize);

                if (byteWritten >= 0)
                {
                    writeArgs.BytesWritten = byteWritten - RipcDataMessage.HeaderSize;
                    writeArgs.UncompressedBytesWritten = byteWritten - RipcDataMessage.HeaderSize;

                    if (byteWritten < msgBuf.Length)
                    {
                        ret = (TransportReturnCode)(msgBuf.Length - byteWritten);
                    }
                }
                else
                {
                    error = new Error
                    {
                        ErrorId = TransportReturnCode.FAILURE,
                        Text = "Channel Write failed.",
                        Channel = this,
                    };
                }

                return ret;
            }
            finally
            {
                _writeLocker.Exit();
            }
        }

        public TransportReturnCode Ping(out Error error)
        {
            TransportReturnCode ret = TransportReturnCode.SUCCESS;
            error = null;

            if (State != ChannelState.ACTIVE)
            {
                error = new Error
                {
                    Channel = this,
                    ErrorId = TransportReturnCode.FAILURE,
                    SysError = 0,
                    Text = "Channel is not in the active state for ping"
                };

                return TransportReturnCode.FAILURE;
            }

            int byteWritten = _socketChannel.Send(PingBuffer.Contents, 0, PingBuffer.Limit);

            if (byteWritten >= 0)
            {
                if (byteWritten < PingBuffer.Limit)
                {
                    ret = (TransportReturnCode)(PingBuffer.Limit - byteWritten);
                }
            }
            else
            {
                error = new Error
                {
                    ErrorId = TransportReturnCode.FAILURE,
                    Text = "Channel Ping failed.",
                    Channel = this,
                };
            }

            return ret;
        }

        public TransportReturnCode ReleaesBuffer(ITransportBuffer buffer, out Error error)
        {
            error = null;

            _transportBufferPool.ReturnTransportBuffer(buffer, out error);

            if (error != null)
            {
                error.Channel = this;
                return TransportReturnCode.FAILURE;
            }
            else
            {
                return TransportReturnCode.SUCCESS;
            }
        }
    }
}
