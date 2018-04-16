/*|-----------------------------------------------------------------------------
 *|            This source code is provided under the Apache 2.0 license      --
 *|  and is provided AS IS with no warranty or guarantee of fit for purpose.  --
 *|                See the project's LICENSE.md for details.                  --
 *|           Copyright Thomson Reuters 2018. All rights reserved.            --
 *|-----------------------------------------------------------------------------
 */

using System;
using System.Threading;
using System.Threading.Tasks;

using System.Linq;

using Xunit;
using Xunit.Abstractions;
using Xunit.Categories;

using ThomsonReuters.Common.Logger;
using ThomsonReuters.Eta.Internal;
using ThomsonReuters.Eta.Transports.Interfaces;
using ThomsonReuters.Eta.Internal.Interfaces;
using ThomsonReuters.Eta.Tests;

namespace ThomsonReuters.Eta.Transports.Tests
{
    #region Transport Initialization
    /// <summary>
    /// This set of test will test expected result of each test
    /// Independent of each other, sequence isn't an issue.
    /// </summary>
    [Collection("Transport")]
    public class TransportInitializationTests
    {
        [Fact]
        [Category("Unit")]
        [Category("Transport")]
        public void TransportInitializeNotInitialize()
        {
            Transport.Clear();
            Assert.True(TransportReturnCode.INIT_NOT_INITIALIZED == Transport.Uninitialize());
        }
        [Fact]
        [Category("Unit")]
        [Category("Transport")]
        public void TransportInitializedOK()
        {
            Transport.Clear();
            InitArgs initArgs = new InitArgs { GlobalLocking = true };
            Assert.True(TransportReturnCode.SUCCESS == Transport.Initialize(initArgs, out Error error));
        }
        [Fact]
        [Category("Unit")]
        [Category("Transport")]
        public void TransportInitializeSubsequentInit()
        {
            // transport still open
            Transport.Clear();
            InitArgs initArgs = new InitArgs { GlobalLocking = true };
            Transport.Initialize(initArgs, out Error error);
            Transport.Initialize(initArgs, out error);
            Assert.True(TransportReturnCode.SUCCESS == Transport.Initialize(initArgs, out error));
            Assert.True(TransportReturnCode.SUCCESS == Transport.Uninitialize());
        }

        [Fact]
        [Category("Unit")]
        [Category("Transport")]
        public void TransportUninitializeOK()
        {
            Transport.Clear();
            InitArgs initArgs = new InitArgs { GlobalLocking = true };
            Transport.Initialize(initArgs, out Error error);
            Assert.True(TransportReturnCode.SUCCESS == Transport.Uninitialize());
        }

        [Fact]
        [Category("Unit")]
        [Category("Transport")]
        public void TransportUninitializeMoreThanOne()
        {
            Transport.Clear();
            InitArgs initArgs = new InitArgs { GlobalLocking = true };
            Transport.Initialize(initArgs, out Error error);
            Transport.Uninitialize();
            Transport.Uninitialize();
            Assert.True(TransportReturnCode.INIT_NOT_INITIALIZED == Transport.Uninitialize());
        }

    }
    #endregion

    #region Transport Connect
    /// <summary>
    /// Validate the Transport.Connect and Transport.GlobalLocking behavior:
    /// 1) Transport must be initialized before any connection attempt.
    /// 2) Transport.Connect must request a supported Protocol.
    /// 3) Transport.Connect can deliver a Blocking Channel.
    /// 4) Transport.Connect can deliver a Non-blocking Channel.
    /// 5) Transport.Connect w/ GlobalLocking only allows a single
    ///    Channel.Connect call to be occurring at one time.
    /// 6) Transport.Connect w/out GlobalLocking allows multiple
    ///    Channel.Connect call to be occurring at one time.
    /// </summary>
    [Collection("Transport")]
    public class TransportConnectTests : IDisposable
    {
        public TransportConnectTests(ITestOutputHelper output)
        {
            XUnitLoggerProvider.Instance.Output = output;
            EtaLoggerFactory.Instance.AddProvider(XUnitLoggerProvider.Instance);
        }

        public void Dispose()
        {
            XUnitLoggerProvider.Instance.Output = null;
        }

        static TransportConnectTests()
        {
            ProtocolRegistry.Instance
                .Register(ConnectionType.UNIDIR_SHMEM, new ThomsonReuters.Eta.Tests.MockProtocol());
        }

        /// <summary>
        /// Channel was created, supports IChannel, there was no error, and the Channel.State
        /// is as expected for the ConnectionType.Blocking value.
        /// </summary>
        /// <param name="connectOptions"></param>
        private IChannel AssertChannelExists(ConnectOptions connectOptions)
        {
            if (string.IsNullOrEmpty(connectOptions.UnifiedNetworkInfo.ServiceName))
                connectOptions.UnifiedNetworkInfo.ServiceName = MockProtocol.PortActionAfter50ms.ToString();

            var channel = Transport.Connect(connectOptions, out Error error);
            if (error != null)
                EtaLogger.Instance.Error($"{DateTime.UtcNow:HH:mm:ss.ffff} Test - Error: {error}");
            Assert.Null(error);

            Assert.IsAssignableFrom<IChannel>(channel);

            EtaLogger.Instance.Trace($"{DateTime.UtcNow:HH:mm:ss.ffff} Test - Channel: {channel}");

            Assert.True((((ChannelBase)channel).State == ChannelState.ACTIVE)
                      || ((ChannelBase)channel).State == ChannelState.INITIALIZING && !connectOptions.Blocking);

            return channel;
        }


        [Fact]
        [Category("Unit")]
        [Category("Transport")]
        public void TransportConnectBeforeInitializeFail()
        {
            try
            {
                Transport.Clear();

                var channel = Transport.Connect(new ConnectOptions
                {
                    ConnectionType = ConnectionType.UNIDIR_SHMEM,
                    Blocking = true
                }, out Error error);

                Assert.Null(channel);
                Assert.Equal(TransportReturnCode.INIT_NOT_INITIALIZED,
                    error.ErrorId);
                Assert.Null(error.Channel);
            }
            finally
            {
                Transport.Clear();
            }
        }

        [Fact]
        [Category("Unit")]
        [Category("Transport")]
        public void TransportConnectNotSupportedProtocolFail()
        {
            try
            {
                InitArgs initArgs = new InitArgs { GlobalLocking = true };
                Transport.Initialize(initArgs, out Error error);

                var actual = Transport.Connect(new ConnectOptions { ConnectionType = ConnectionType.HTTP }, out error);

                Assert.Null(actual);
            }
            finally
            {
                Transport.Uninitialize();

            }
        }

        [Fact(Skip = "Broken")]
        [Category("Unit")]
        [Category("Transport")]
        [Category("Channel")]
        public void TransportConnectBlockingProtocolOk()
        {
            IChannel channel = null;
            try
            {
                InitArgs initArgs = new InitArgs { GlobalLocking = true };

                var actual = Transport.Initialize(initArgs, out Error error);
                if (error != null)
                    EtaLogger.Instance.Trace($"{error}");

                Assert.Equal(TransportReturnCode.SUCCESS, actual);

                var cnxnOptions = new ConnectOptions
                {
                    ConnectionType = ConnectionType.UNIDIR_SHMEM,
                    Blocking = true
                };
                cnxnOptions.UnifiedNetworkInfo.Address = "localhost";
                cnxnOptions.UnifiedNetworkInfo.ServiceName = MockProtocol.PortActionAfter50ms.ToString();

                channel = AssertChannelExists(cnxnOptions);
            }
            finally
            {
                if (channel != null)
                {
                    channel.Close(out Error error);
                    Assert.Null(error);
                }
                Transport.Uninitialize();
            }
        }

        [Fact(Skip ="Broken")]
        [Category("Unit")]
        [Category("Transport")]
        [Category("Channel")]
        async public void TransportConnectBlockingOneAtATime()
        {
            try
            {
                InitArgs initArgs = new InitArgs { GlobalLocking = false };
                var result = Transport.Initialize(initArgs, out Error error);

                Assert.Equal(TransportReturnCode.SUCCESS, result);

                Func<IChannel> connect = new Func<IChannel>(() =>
                {
                    var cnxnOptions = new ConnectOptions
                    {
                        ConnectionType = ConnectionType.SOCKET,
                        Blocking = true
                    };
                    cnxnOptions.UnifiedNetworkInfo.Address = "localhost";
                    cnxnOptions.UnifiedNetworkInfo.ServiceName = MockProtocol.PortActionAfter50ms.ToString();

                    IChannel channel = null;
                    try
                    {
                        channel = Transport.Connect(cnxnOptions,
                                                 out Error connectError);
                        if (connectError != null)
                            EtaLogger.Instance.Trace($"{DateTime.UtcNow:HH:mm:ss.ffff} Test ConnectError: {connectError}");

                        return channel;
                    }
                    finally
                    {
                        channel.Close(out Error closeError);
                        Assert.Null(closeError);
                    }
                });

                MockProtocol.TestMultipleConnectsFail = true;

                var taskA = Task.Factory.StartNew(connect);
                var taskB = Task.Factory.StartNew(connect);

                var channelA = taskA.Result;
                var channelB = taskB.Result;

                Assert.NotNull(channelA);
                Assert.IsAssignableFrom<IChannel>(channelA);

                await taskB;
            }
            finally
            {
                Transport.Uninitialize();
            }
        }

        [Fact(Skip = "Broken")]
        [Category("Unit")]
        [Category("Transport")]
        [Category("Channel")]
        public void TransportConnectNonBlockingChannelAsyncConnect()
        {
            Error connectError = null;
            Error initError = null;

            try
            {
                InitArgs initArgs = new InitArgs { GlobalLocking = false };

                var returnCode = Transport.Initialize(initArgs, out Error error);
                Assert.Equal(TransportReturnCode.SUCCESS, returnCode);

                Func<int, IChannel> connect = new Func<int, IChannel>((port) =>
                {
                    IChannel result = null;
                    var connectionOptions = new ConnectOptions
                    {
                        ConnectionType = ConnectionType.UNIDIR_SHMEM,
                        Blocking = false      
                    };
                    connectionOptions.UnifiedNetworkInfo.Address = "localhost";
                    connectionOptions.UnifiedNetworkInfo.ServiceName = port.ToString();
                    result = Transport.Connect(connectionOptions,
                                             out connectError);

                    return result;
                });

                Func<IChannel, TransportReturnCode> initPoll = new Func<IChannel, TransportReturnCode>((chnl) =>
                {
                    TransportReturnCode initReturnCode;
                    do
                    {
                        Thread.Sleep(10);
                        initReturnCode = chnl.Init(out initError);
                    } while (initReturnCode != TransportReturnCode.SUCCESS);
                    return initReturnCode;
                });

                // Create a MockChannel that will pend on external connect signal before
                // changing its state.
                IChannel channel = connect(MockProtocol.PortActionOnSignal);
                var initTask = Task.Factory.StartNew<TransportReturnCode>(() => initPoll(channel));

                // Assert Channel created initializing.
                Assert.Equal(TransportReturnCode.CHAN_INIT_IN_PROGRESS, channel.Init(out connectError));

                // Fondle internals
                IInternalChannel baseChannel = channel as IInternalChannel;
                Assert.NotNull(baseChannel);

                Thread.Sleep(250);
                // Assert Channel created still initializing.
                Assert.Equal(TransportReturnCode.CHAN_INIT_IN_PROGRESS, channel.Init(out connectError));

                // Externally Set Connected Signal
                baseChannel.SocketChannel.IsConnected = true;

                Thread.Sleep(50);
                baseChannel.SocketChannel.IsDataReady = true;

                int waitms = 15000;
                if (initTask.Wait(waitms))
                    Assert.Equal(TransportReturnCode.SUCCESS, initTask.Result);
                else
                    throw new TransportException($"Channel (NonBlocking) Timeout: ({waitms})ms");

            }
            finally
            {
                Transport.Clear();
            }
        }

    }
    #endregion

    #region Transport Initialization ThreadSafety
    /// <summary>
    /// step 1
    /// Test the ability to run TaskB within TaskA
    /// step 2
    /// Test the ability to run TaskA within TaskB
    /// step 3
    /// Created 30 Task random intervals
    /// </summary>
    [Collection("Transport")]
    public class TransportTestsThreadSafety
    {
        [Fact]
        [Category("Unit")]
        [Category("Transport")]
        public void TaskBRunsWithinTaskA()
        {
            object result1 = null;
            object result2 = null;
            var threadId = Thread.CurrentThread.ManagedThreadId;
            Transport.Clear(); // 
            Task taskA = new Task<TransportReturnCode>(
                () =>
                {
                    InitArgs initArgs = new InitArgs { GlobalLocking = true };
                    Transport.Initialize(initArgs, error: out Error error);
                    threadId = Thread.CurrentThread.ManagedThreadId;
                    Thread.Sleep(2000);
                    result1 = Transport.Uninitialize();
                    Assert.Equal(TransportReturnCode.SUCCESS, result1);
                    return (TransportReturnCode)result1;


                });
            Task taskB = new Task<TransportReturnCode>(
                () =>
                {
                    threadId = Thread.CurrentThread.ManagedThreadId;
                    InitArgs initArgs = new InitArgs { GlobalLocking = true };
                    Transport.Initialize(initArgs, error: out Error error);
                    Thread.Sleep(1000);
                    result2 = Transport.Uninitialize();
                    Assert.Equal(TransportReturnCode.SUCCESS, result2);
                    return (TransportReturnCode)result2;
                });
            taskA.Start();
            Thread.Sleep(250);
            taskB.Start();
            Task.WaitAll(taskA, taskB);
            Assert.Equal(TransportReturnCode.INIT_NOT_INITIALIZED, Transport.Uninitialize());
        }

        [Fact]
        [Category("Unit")]
        [Category("Transport")]
        public void TaskARunsWithinTaskB()
        {
            object result1 = null;
            object result2 = null;
            var threadId = Thread.CurrentThread.ManagedThreadId;
            Transport.Clear(); // 
            Task taskA = new Task<TransportReturnCode>(
                () =>
                {
                    InitArgs initArgs = new InitArgs { GlobalLocking = true };
                    Transport.Initialize(initArgs, error: out Error error);
                    threadId = Thread.CurrentThread.ManagedThreadId;
                    Thread.Sleep(2000);
                    result1 = Transport.Uninitialize();
                    Assert.Equal(TransportReturnCode.SUCCESS, result1);
                    return (TransportReturnCode)result1;
                });
            Task taskB = new Task<TransportReturnCode>(
                () =>
                {
                    var threadId2 = Thread.CurrentThread.ManagedThreadId;
                    InitArgs initArgs = new InitArgs { GlobalLocking = true };
                    Transport.Initialize(initArgs, error: out Error error);
                    Thread.Sleep(2000);
                    result2 = Transport.Uninitialize();
                    Assert.Equal(TransportReturnCode.SUCCESS, result2);
                    return (TransportReturnCode)result2;
                });
            taskA.Start();
            Thread.Sleep(500);
            taskB.Start();
            Task.WaitAll(taskA, taskB);
            Assert.Equal(TransportReturnCode.INIT_NOT_INITIALIZED, Transport.Uninitialize());
        }

        [Fact(Skip = "TakesTooLong")]
        [Category("Unit")]
        [Category("Transport")]
        public void MultipleTaskVariableIntervals()
        {
            object[] resultList = new object[30];
            Random randomInteval = new Random();

            Task<TransportReturnCode>[] taskArray = new Task<TransportReturnCode>[30];
            int randomInt;
            var threadId = Thread.CurrentThread.ManagedThreadId;

            Transport.Clear();
            for (int i = 0; i < taskArray.Length; i++)
            {
                randomInt = randomInteval.Next(0, 3000);

                taskArray[i] = Task<TransportReturnCode>.Factory.StartNew(
                            () =>
                            {
                                InitArgs initArgs = new InitArgs { GlobalLocking = true };
                                Transport.Initialize(initArgs, error: out Error error);
                                Thread.Sleep(randomInt);
                                return Transport.Uninitialize();
                            });//).Result;
                resultList[i] = taskArray[i].Result;
                Assert.Equal(TransportReturnCode.SUCCESS, resultList[i]);
            }
            Task.WaitAll(taskArray, 6000);
            Assert.Equal(TransportReturnCode.INIT_NOT_INITIALIZED, Transport.Uninitialize());
        }
    }
    #endregion

    #region Transport Protocol Tests
    [Collection("Transport")]
    public class TransportTestsProtocol : IDisposable
    {
        public TransportTestsProtocol(ITestOutputHelper output)
        {
            XUnitLoggerProvider.Instance.Output = output;
            EtaLoggerFactory.Instance.AddProvider(XUnitLoggerProvider.Instance);
        }

        public void Dispose()
        {
            XUnitLoggerProvider.Instance.Output = null;
        }

        static TransportTestsProtocol()
        {
            ProtocolRegistry.Instance
                .Register(ConnectionType.UNIDIR_SHMEM, new ThomsonReuters.Eta.Tests.MockProtocol());
        }

        /// <summary>
        /// Channel was created, supports IChannel, there was no error, and the Channel.State
        /// is as expected for the ConnectionType.Blocking value.
        /// </summary>
        /// <param name="connectOptions"></param>
        private void AssertChannelExists(ConnectOptions connectOptions)
        {
            var channel = Transport.Connect(connectOptions, out Error error);
            if (error != null)
                EtaLogger.Instance.Trace($"{error}");

            Assert.IsAssignableFrom<IChannel>(channel);
            Assert.Null(error);
            Assert.True((((ChannelBase)channel).State == ChannelState.ACTIVE)
                      || ((ChannelBase)channel).State == ChannelState.INITIALIZING && !connectOptions.Blocking);
        }
    }
    #endregion

    #region Transport Global Locking Tests

    [Collection("Transport")]
    public class TransportGlobalLockingTests : IDisposable
    {

        public TransportGlobalLockingTests(ITestOutputHelper output)
        {
            XUnitLoggerProvider.Instance.Output = output;
            EtaLoggerFactory.Instance.AddProvider(XUnitLoggerProvider.Instance);
        }

        public void Dispose()
        {
            XUnitLoggerProvider.Instance.Output = null;
        }

        [Fact]
        [Category("Unit")]
        [Category("Transport")]
        public void TransportInitializeCannotChangeGlobalLocking()
        {
            InitArgs initArgsTrue = new InitArgs()
            { GlobalLocking = true };
            InitArgs initArgsFalse = new InitArgs()
            { GlobalLocking = false };

            try
            {
                Transport.Initialize(initArgsTrue, out Error error);
                var result = Transport.Initialize(initArgsFalse, out error);

                Assert.Equal(TransportReturnCode.FAILURE, result);
                Assert.Equal(TransportReturnCode.INIT_NOT_INITIALIZED, error.ErrorId);
            }
            finally
            {
                Transport.Clear();
            }
        }


    }

    #endregion

    #region Transport Select

    [Collection("Transport"), Category("Unit"), Category("Transport")]
    public class TransportSelectTests
    {
        ITestOutputHelper _output;

        public TransportSelectTests(ITestOutputHelper output)
        {
            _output = output;
            ProtocolRegistry.Instance
                           .Register(ConnectionType.UNIDIR_SHMEM, new ThomsonReuters.Eta.Tests.MockProtocol());
        }
    }

    #endregion
}
