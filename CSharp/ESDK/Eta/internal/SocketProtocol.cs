/*|-----------------------------------------------------------------------------
 *|            This source code is provided under the Apache 2.0 license      --
 *|  and is provided AS IS with no warranty or guarantee of fit for purpose.  --
 *|                See the project's LICENSE.md for details.                  --
 *|           Copyright Thomson Reuters 2018. All rights reserved.            --
 *|-----------------------------------------------------------------------------
 */

using System.Linq;
using System.Threading;

using ThomsonReuters.Eta.Common;
using ThomsonReuters.Eta.Transports;
using ThomsonReuters.Eta.Internal.Interfaces;
using ThomsonReuters.Eta.Transports.Interfaces;

namespace ThomsonReuters.Eta.Internal
{
    internal class SocketProtocol : ProtocolBase
    {
        static ReaderWriterLockSlim _slimLock = new ReaderWriterLockSlim(LockRecursionPolicy.SupportsRecursion);
        static WriteLocker _locker = new WriteLocker(_slimLock);

        public override IChannel CreateChannel(ConnectOptions connectOptions, out Error error)
        {
            error = null;
            var channel = new ChannelBase(connectOptions, new SocketChannel());
            if (channel != null)
            {
                ConnectChannel(channel, out error);
            }
            return channel;
        }

        public override void Uninitialize(out Error error)
        {
            try
            {
                _locker.Enter();

                var activeChannels = Channels
                    .Where((c) => c.State == ChannelState.ACTIVE || c.State == ChannelState.INITIALIZING);

                foreach (var channel in Channels)
                {
                    channel.Close(out error);
                }

                error = null;
            }
            finally
            {
                _locker.Exit();
            }
        }
    }
}
