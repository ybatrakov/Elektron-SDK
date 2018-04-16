/*|-----------------------------------------------------------------------------
 *|            This source code is provided under the Apache 2.0 license      --
 *|  and is provided AS IS with no warranty or guarantee of fit for purpose.  --
 *|                See the project's LICENSE.md for details.                  --
 *|           Copyright Thomson Reuters 2018. All rights reserved.            --
 *|-----------------------------------------------------------------------------
 */

using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading;
using System.Net.Sockets;
using ThomsonReuters.Common.Logger;
using ThomsonReuters.Eta.Codec;
using ThomsonReuters.Eta.Rdm;
using ThomsonReuters.Eta.Transports;
using ThomsonReuters.Eta.Transports.Interfaces;

namespace ThomsonReuters.Eta.Net.Consumer
{
    public class EtaConsumer
    {
        static IDictionary<Socket,KeyValuePair<IChannel, IBufferParser>> ChannelSessions 
            = new Dictionary<Socket, KeyValuePair<IChannel, IBufferParser>>();

        static List<Socket> readSocketList;
        static List<Socket> writeSocketList;
        static bool exitApplication = false;

        /// <summary>
        /// Configuration 
        /// </summary>
        static void Main(string[] args)
        {
            TransportReturnCode result = TransportReturnCode.FAILURE;
           
            try
            {
                //------------------------------------------------------------------
                // Configure the Logger
                EtaLoggerFactory.LogToConsole = EtaConfiguration.LogToConsole;
                EtaLoggerFactory.LogToNLog = EtaConfiguration.LogToFile;
                EtaLoggerFactory.LogLevel = EtaConfiguration.LogLevel;
                EtaLogger.LogLevel = EtaConfiguration.LogLevel;
                EtaLogger.LogName = "Eta.Net.Consumer";

                Console.CancelKeyPress += (sender, EventArgs) =>
                {
                    EtaLogger.Instance.Information("!CancelKeyPress!");
                    System.Diagnostics.Trace.WriteLine("!CancelKeyPress!");
                    EventArgs.Cancel = true;
                    exitApplication = true;
                };

                WriteConfigurationToLog();

                result = Run();

            }
            catch (Exception exp)
            {
                EtaLogger.Instance.Error(exp);
            }
            finally
            {
                if (result != TransportReturnCode.SUCCESS)
                {
                    Console.WriteLine($"Return Code: ({result}); press any key to continue...");
                    Console.ReadKey();
                }
            }
        }

        private static void WriteConfigurationToLog()
        {
            EtaLogger.Instance.Information("Eta.Net.Consumer Application Global Settings");
            EtaLogger.Instance.Information($"        Application Name = {EtaConfiguration.Application}");
            EtaLogger.Instance.Information($"             Environment = {EtaConfiguration.Environment}");
            EtaLogger.Instance.Information($"                LogLevel = {EtaLogger.LogLevel}");
            EtaLogger.Instance.Information($"               LogToFile = {EtaLoggerFactory.LogToNLog}");
            EtaLogger.Instance.Information($"            LogToConsole = {EtaLoggerFactory.LogToConsole}");
            EtaLogger.Instance.Information($"      Max Message Length = {EtaConfiguration.MaxMessageLength}");
            EtaLogger.Instance.Information($"      Ping Interval(sec) = {EtaConfiguration.PingInterval}");
            EtaLogger.Instance.Information($"Transport Global Locking = {EtaConfiguration.GlobalLocking}");
            foreach (var connection in EtaConfiguration.GetConnections())
            {
                EtaLogger.Instance.Information($"   Connection: {connection}");
            }
        }

        private static void PerformSocketSelect()
        {
            readSocketList.Clear();
            writeSocketList.Clear();

            foreach (KeyValuePair<IChannel, IBufferParser> kvp in ChannelSessions.Values)
            {
                readSocketList.Add(kvp.Key.Socket);
                writeSocketList.Add(kvp.Key.Socket);
            }

            Socket.Select(readSocketList, writeSocketList, null, 1);
        }

        private static TransportReturnCode Run()
        {
            TransportReturnCode returnCode = TransportReturnCode.SUCCESS;

            long pingCounter = 0;

            EtaLogger.Instance.Information("Eta.Net.Consumer Application running");

            InitArgs initArgs = new InitArgs { GlobalLocking = false };

            try
            {
                returnCode = Transport.Initialize(initArgs, out Error error);
                if (returnCode != TransportReturnCode.SUCCESS)
                {
                    EtaLogger.Instance.Error($"Transport.Initialize ({returnCode})");
                    return returnCode;
                }

                ConnectChannels(out error);
                if (error != null)
                {
                    EtaLogger.Instance.Information("Fails to make a connection");
                    return TransportReturnCode.FAILURE;
                }

                readSocketList = new List<Socket>(ChannelSessions.Count);
                writeSocketList = new List<Socket>(ChannelSessions.Count);
           
                System.Console.WriteLine("Number of channels = {0}", ChannelSessions.Count);

                int pingInterval = EtaConfiguration.PingInterval / 3;

                // Login to server for all Chnnels
                foreach (KeyValuePair<IChannel, IBufferParser> kvp in ChannelSessions.Values)
                {
                    if (EtaConfiguration.Blocking)
                    {
                        LoginToServer(kvp.Key);
                        pingInterval = (kvp.Key.PingTimeOut * 1000)/3;
                    }
                    else
                    {
                        kvp.Key.Socket.Poll(-1, System.Net.Sockets.SelectMode.SelectRead);

                        if (kvp.Key.Init(out error) == TransportReturnCode.SUCCESS)
                        {
                            LoginToServer(kvp.Key);
                           pingInterval = (kvp.Key.PingTimeOut * 1000)/3;
                        }
                    }
                }

                if (ChannelSessions.Count == 0)
                {
                    EtaLogger.Instance.Error($"*Error* There is no active channels.");
                    exitApplication = true;
                }
                else
                {
                    EtaLogger.Instance.Information("Successfully connection to the server");
                }

                Timer pingTimer = new Timer((userState) =>
                {
                    PingAllActiveChannels(ref pingCounter);

                }, null, Timeout.Infinite, pingInterval);

                //--------------------------------------------------
                // Start the PingTimer
                //--------------------------------------------------
                pingTimer.Change(pingInterval, pingInterval);

                ReadArgs args = new ReadArgs();
                IChannel channel;
                Socket socket;
               
                EtaLogger.Instance.Information("Start running in the while loop of the application");

                while (!exitApplication)
                {
                    PerformSocketSelect();

                    for(int i = 0; i < readSocketList.Count; ++i)
                    {
                        socket = readSocketList[i];
                        //-------------------------------------------------------
                        // Channel Events
                        //-------------------------------------------------------
                        channel = ChannelSessions[socket].Key;
                        var messageHandler = ChannelSessions[socket].Value;
                        switch (channel.State)
                        {
                            case ChannelState.INITIALIZING:

                            if (channel.Init(out error) != TransportReturnCode.SUCCESS)
                            {
                                EtaLogger.Instance.Information($"Failed to initialize channel : {error}");
                                exitApplication = true;
                                break;
                            }

                            break;

                            case ChannelState.ACTIVE:
                                do
                                {
                                    var transportBuffer = channel.Read(args, out error);

                                    switch (args.ReadRetVal)
                                    {
                                        case TransportReturnCode.FAILURE:
                                            {
                                                EtaLogger.Instance.Information($"Channel is inactive for Channel {channel}");
                                                exitApplication = true;
                                                break;
                                            }
                                        case TransportReturnCode.READ_PING:
                                            {
                                                EtaLogger.Instance.Trace($"Read ping message from the server");
                                                continue;
                                            }
                                        default:
                                            {
                                                if (args.ReadRetVal < 0 && args.ReadRetVal != TransportReturnCode.READ_WOULD_BLOCK)
                                                {
                                                    EtaLogger.Instance.Information($"Channel Read *Error* {error}. Close the Channel {channel}");
                                                }
                                                else
                                                {
                                                    if (transportBuffer != null)
                                                    {
                                                        if (transportBuffer.Length > 0)
                                                        {
                                                            // Checks whether this application received expected domain and message types.
                                                            if ( messageHandler.Parse(channel, transportBuffer) == false )
                                                            {
                                                                EtaLogger.Instance.Information($"Unexpected message for channel: {channel}. Closes the channel and exits application.");
                                                                channel.Close(out error);
                                                                exitApplication = true;
                                                                break;
                                                            }
                                                        }
                                                    }
                                                }
                                                break;
                                            }
                                    }

                                } while (args.ReadRetVal > (int)TransportReturnCode.SUCCESS);

                            break;

                        case ChannelState.INACTIVE:
                        case ChannelState.CLOSED:
                            if (channel.State == ChannelState.INACTIVE)
                            {   //--------------------------------------------------
                                // FAILED TO INITIALIZE
                                //--------------------------------------------------
                                EtaLogger.Instance.Error($"Channel *Initialization Failed* {channel}");
                                exitApplication = true;
                                break;
                            }
                            else
                            {
                                //--------------------------------------------------
                                // Closed by Server.
                                //--------------------------------------------------
                                EtaLogger.Instance.Information($"Channel Closed: {channel}");
                                exitApplication = true;
                                break;
                            }
                        }
                    }
                };

                pingTimer.Dispose();
                returnCode = Transport.Uninitialize();
                if (returnCode != TransportReturnCode.SUCCESS)
                    EtaLogger.Instance.Information($"Transport.Uninitialize ({returnCode})");
            }
            catch (Exception exp)
            {
                EtaLogger.Instance.Error(exp);
                returnCode = TransportReturnCode.FAILURE;
            }

            return returnCode;
        }

        private static TransportReturnCode LoginToServer(IChannel channel)
        {
            TransportReturnCode returnCode;

            EtaLogger.Instance.Information($"Channel Connected {channel}");

            var writeArgs = new WriteArgs();

            var transportBuffer = CreateLoginRequest(channel, out Error error);
            if (transportBuffer != null)
            {
                returnCode = channel.Write(transportBuffer, writeArgs, out error);

                if (returnCode == TransportReturnCode.SUCCESS)
                {
                    channel.ReleaesBuffer(transportBuffer, out error);
                    EtaLogger.Instance.Information($"LoginToServer WriteArgs: {writeArgs}");
                }
                else
                {
                    return returnCode;
                }
            }
            else
                returnCode = error.ErrorId;
           
            return returnCode;
        }

        private static void PingAllActiveChannels(ref long pingCounter)
        {
            // PING
            Interlocked.Increment(ref pingCounter);

            EtaLogger.Instance.Trace($"Ping[{pingCounter}]");
            System.Diagnostics.Trace.WriteLine($"Ping[{pingCounter}]");

            foreach (var kvp in ChannelSessions
                                .Select(cs => cs)
                                .Where(c => c.Value.Key.State == ChannelState.ACTIVE))
            {
                kvp.Value.Key.Ping(out Error error);
                if (error != null)
                {
                    EtaLogger.Instance.Error($"Channel {{{kvp.Value}}}\n*Error* {error}");
                }
            }
        }

        private static void ConnectChannels(out Error error)
        {
            error = null;

            var connections = EtaConfiguration.GetConnections();
            foreach (var connection in connections)
            {
                ConnectOptions cnxnOptions = new ConnectOptions()
                {
                    Blocking = EtaConfiguration.Blocking,
                    SysRecvBufSize = EtaConfiguration.MaxMessageLength,
                    UnifiedNetworkInfo =
                    {
                        Address = connection.ServerAddress,
                        ServiceName = connection.ServerPort
                    },
                    MajorVersion = Codec.Codec.MajorVersion(),
                    MinorVersion = Codec.Codec.MinorVersion(),
                    GuaranteedOutputBuffers = 500,
                    NumGuaranteedOutputBuffers = 30,
                    NumInputBuffers = EtaConfiguration.MaxMessageLength,
                    PingTimeout = EtaConfiguration.PingInterval,
                    ChannelReadLocking = false,
                    ChannelWriteLocking = false,
                    TcpOpts =
                    {
                        TcpNoDelay = true
                    }
                };

                IChannel channel = Transport.Connect(cnxnOptions, out error);
                if (error == null)
                {
                    var channelSession = ChannelSessions.FirstOrDefault((kvp) => channel.Equals(kvp.Value.Key));
                    if (channelSession.Equals(default(KeyValuePair<Socket, KeyValuePair<IChannel, IBufferParser>>)))
                    {
                        var messageHandler = CreateHandler(connection) ?? new DecodingBufferParser(connection.ServiceId ?? 1);
                        channelSession = new KeyValuePair<Socket, KeyValuePair<IChannel, IBufferParser>>(channel.Socket, new KeyValuePair<IChannel, IBufferParser>(channel, messageHandler));
                        ChannelSessions.Add(channelSession);
                    }

                    EtaLogger.Instance.Information($"Connection initiatiated with ( {channel} )");
                }
                else
                {
                    EtaLogger.Instance.Error($"Connection failed with {error}");
                }
            }
        }

        private static IBufferParser CreateHandler(Connection connection)
        {
            IBufferParser bufferParser = null;

            if (string.IsNullOrEmpty(connection.Handler))
                return null;

            var handlerType = Type.GetType(connection.Handler);
            if (handlerType != null)
            {

                bufferParser = ((connection.ServiceId.HasValue) 
                                    ? Activator.CreateInstance(handlerType, connection.ServiceId ?? 1)
                                    : Activator.CreateInstance(handlerType))
                                    as IBufferParser;                     
            }
            return bufferParser;
        }

        private static ITransportBuffer CreateLoginRequest(IChannel channel, out Error error)
        {
            error = null;
            CodecReturnCode codecReturnCode = CodecReturnCode.SUCCESS;
            ITransportBuffer transportBuffer = channel.GetBuffer(1024, false, out error);

            try
            {
                ThomsonReuters.Eta.Codec.Buffer username = new ThomsonReuters.Eta.Codec.Buffer();
                username.Data(EtaConfiguration.UserName);
                ThomsonReuters.Eta.Codec.Buffer appname = new ThomsonReuters.Eta.Codec.Buffer();
                appname.Data(EtaConfiguration.Application);

                EncodeIterator encodeIterator = new EncodeIterator();

                codecReturnCode = encodeIterator.SetBufferAndRWFVersion(transportBuffer, channel.MajorVersion, channel.MinorVersion);
                if (codecReturnCode < CodecReturnCode.SUCCESS)
                    throw new Exception($"SetBufferAndRWFVersion: {codecReturnCode}");

                Msg requestMsg = new Msg
                {
                    MsgClass = MsgClasses.REQUEST,
                    StreamId = 1,
                    Flags = RequestMsgFlags.STREAMING,
                    DomainType = (int)DomainType.LOGIN,
                    ContainerType = DataTypes.NO_DATA
                };
                requestMsg.MsgKey.Flags = MsgKeyFlags.HAS_NAME | MsgKeyFlags.HAS_NAME_TYPE | MsgKeyFlags.HAS_ATTRIB;
                requestMsg.MsgKey.AttribContainerType = DataTypes.ELEMENT_LIST;
                requestMsg.MsgKey.Name = username;
                requestMsg.MsgKey.NameType = Login.UserIdTypes.NAME;

                codecReturnCode = requestMsg.EncodeInit(encodeIterator, 0);
                if (codecReturnCode < CodecReturnCode.SUCCESS)
                    throw new Exception($"Msg.EncodeInit() fails: {codecReturnCode}");

                ElementList elementList = new ElementList();
                ElementEntry elementEntry = new ElementEntry();

                elementList.Clear();
                elementList.Flags = ElementListFlags.HAS_STANDARD_DATA;
                codecReturnCode = elementList.EncodeInit(encodeIterator, null, 0);
                if (codecReturnCode < CodecReturnCode.SUCCESS)
                    throw new Exception($"Msg.EncodeInit() fails: {codecReturnCode}");

                elementEntry.Name = ElementNames.APPNAME;
                elementEntry.DataType = DataTypes.ASCII_STRING;
                codecReturnCode = elementEntry.Encode(encodeIterator, appname);
                if (codecReturnCode < CodecReturnCode.SUCCESS)
                    throw new Exception($"Msg.encodeIterator() fails: {codecReturnCode}");

                codecReturnCode = elementList.EncodeComplete(encodeIterator, true);
                if (codecReturnCode < CodecReturnCode.SUCCESS)
                    throw new Exception($"Msg.EncodeComplete() fails: {codecReturnCode}");

                codecReturnCode = requestMsg.EncodeKeyAttribComplete(encodeIterator, true);
                if (codecReturnCode < CodecReturnCode.SUCCESS)
                    throw new Exception($"Msg.EncodeKeyAttribComplete() fails: {codecReturnCode}");

                codecReturnCode = requestMsg.EncodeComplete(encodeIterator, true);
                if (codecReturnCode < CodecReturnCode.SUCCESS)
                    throw new Exception($"Msg.EncodeComplete() fails: {codecReturnCode}");

            }
            catch (Exception exp)
            {
                error = new Error(TransportReturnCode.FAILURE,
                                  exp.Message,
                                  channel,
                                  (int)codecReturnCode);
                throw;
            }

            return transportBuffer;
        }
    }
}
