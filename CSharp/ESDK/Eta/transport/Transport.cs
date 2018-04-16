/*|-----------------------------------------------------------------------------
 *|            This source code is provided under the Apache 2.0 license      --
 *|  and is provided AS IS with no warranty or guarantee of fit for purpose.  --
 *|                See the project's LICENSE.md for details.                  --
 *|           Copyright Thomson Reuters 2018. All rights reserved.            --
 *|-----------------------------------------------------------------------------
 */

using System;
using System.Diagnostics;
using System.Reflection;
using System.Linq;
using System.Threading;

using ThomsonReuters.Eta.Common;
using ThomsonReuters.Eta.Internal;
using ThomsonReuters.Eta.Transports.Interfaces;

namespace ThomsonReuters.Eta.Transports
{
    /**
    * ETA transport is used by OMM consumer and to establish outbound
    * connections to a listening socket.
    */
    public static class Transport
    {
        private static long _numInitCalls = 0;
        private static Object _initializationLock = new Object(); // lock used during Initial() and Uninitialize()

        private static Locker _locker;
        private static ReaderWriterLockSlim _slimLock = new ReaderWriterLockSlim(LockRecursionPolicy.SupportsRecursion);

        private static bool _globalLocking = true;

        private static ProtocolRegistry _protocolRegistry = ProtocolRegistry.Instance;

        private static Assembly _assembly = Assembly.GetExecutingAssembly();
        private static FileVersionInfo _fileVersionInfo = FileVersionInfo.GetVersionInfo(_assembly.Location);

        /// <summary>
        /// Initialize the ETA transport API and all internal members.
        /// 
        /// This is the first method called when using the ETA. It initializes internal data structures.
        /// <param name="initArgs">Arguments for initialize</param>
        /// <param name="error">ETA Error, to be set and populated in event of an error</param>
        /// <returns><see cref="TransportReturnCode"/></returns> 
        /// </summary>
        public static TransportReturnCode Initialize(InitArgs initArgs, out Error error)
        {
            TransportReturnCode ret = TransportReturnCode.SUCCESS;
            lock (_initializationLock)
            {
                if (Interlocked.Read(ref _numInitCalls) >= 0)
                {
                    if (_numInitCalls == 0)
                    {
                        Interlocked.Increment(ref _numInitCalls);

                        _globalLocking = initArgs.GlobalLocking;
                        _locker = (_globalLocking)
                            ? (Locker)new WriteLocker(_slimLock)
                            : (Locker)new NoLocker();
                        error = null;
                    }
                    else if (initArgs.GlobalLocking != _globalLocking)
                    {
                        error = new Error(errorId: TransportReturnCode.INIT_NOT_INITIALIZED,
                                             text: $"Transport.Initialize: Attempting to change locking from ({_globalLocking}) to ({initArgs.GlobalLocking}).");
                        ret = TransportReturnCode.FAILURE;
                    }
                    else
                    {
                        Interlocked.Increment(ref _numInitCalls);
                        error = null;
                    }
                }
                else
                {
                    // This should never happen; programming error on part of ETA.
                    throw new InvalidOperationException($"Initialization underflow");
                }
            }
            return ret;

        }

        /// <summary>
        /// Uninitialize the ETA API and all internal members.<para />
        /// 
        /// This is the last method by an application when using the ETA.
        /// If multiple threads call Initialize() on Transport, they have to
        /// call Uninitialize() when the thread finishes.
        /// The last Unitialize() call release all internally pooled resources to GC.
        /// 
        /// </summary>
        /// <returns><see cref="TransportReturnCode"/></returns> 
        public static TransportReturnCode Uninitialize()
        {
            TransportReturnCode returnCode = TransportReturnCode.FAILURE;

            lock (_initializationLock)
            {
                if (_numInitCalls == 0)
                {
                    returnCode = TransportReturnCode.INIT_NOT_INITIALIZED;
                }
                else
                {
                    --_numInitCalls;
                    if (_numInitCalls == 0)
                    {
                        try
                        {
                            _locker.Enter();

                            foreach (var (connectionType, protocol) in _protocolRegistry)
                            {
                                protocol.Uninitialize(out Error error);
                            }
                        }
                        finally
                        {
                            _locker.Exit();

                            _locker = null;
                        }
                    }

                    returnCode = TransportReturnCode.SUCCESS;
                }
                return returnCode;
            }
        }

        /// <summary>
        /// Initialize transport defined in opts if not initialized.
        /// Connects a client to a listening server.
        /// </summary>
        /// <param name="connectOptions">The connection option</param>
        /// <param name="error">The error when an error occurs</param>
        /// <returns><see cref="IChannel"/></returns>
        public static IChannel Connect(ConnectOptions connectOptions, out Error error)
        {
            error = null;
            IChannel channel = null;

            if (Interlocked.Read(ref _numInitCalls) == 0)
            {
                error = new Error(errorId: TransportReturnCode.INIT_NOT_INITIALIZED,
                                     text: "Transport not initialized.");
                return null;
            }

            try
            {
                _locker.Enter();

                if (connectOptions is null)
                    throw new ArgumentNullException($"Parameter ({nameof(connectOptions)}) cannot be null.");
                if (string.IsNullOrWhiteSpace(connectOptions.UnifiedNetworkInfo.Address))
                    throw new TransportException($"{nameof(connectOptions.UnifiedNetworkInfo)}.{nameof(connectOptions.UnifiedNetworkInfo.Address)} must be set to an address.");

                var protocol = _protocolRegistry[connectOptions.ConnectionType];
                if (protocol is null)
                    throw new TransportException($"Unsupported transport type ({connectOptions.ConnectionType})");

                channel = protocol.CreateChannel(connectOptions, out error);

                if (channel == null)
                    throw new TransportException( $"Could not create a channel for ConnectionType: {connectOptions.ConnectionType}", 
                                                   null);

                if (connectOptions.Blocking)
                {
                    channel.Socket.Poll(-1, System.Net.Sockets.SelectMode.SelectRead);

                    var returnCode = channel.Init(out error);
                    if (returnCode == TransportReturnCode.FAILURE)
                    {
                        return null;
                    }
                }
            }
            catch(Exception exp)
            {
                error = new Error(errorId: TransportReturnCode.FAILURE,
                                     text: exp.Message,
                                     channel: channel);
            }
            finally
            {
                _locker.Exit();
            }


            return channel;
        }

        /// <summary>
        /// The library version
        /// </summary>
        public static FileVersionInfo TransportLibrayVersion
        {
            get
            {
                return _fileVersionInfo;
            }
        }

        /// <summary>
        /// Clears ETA Initialize Arguments
        /// </summary>
        internal static void Clear()
        {
            lock (_initializationLock)
            {
                _numInitCalls = 0;

                foreach (var protocol in _protocolRegistry.Select(i => i.protocol))
                    protocol.Uninitialize(out Error error);

                while (_locker != null && _locker.Locked)
                    _locker.Exit();
                _locker = null;
            }
        }

    }
}
