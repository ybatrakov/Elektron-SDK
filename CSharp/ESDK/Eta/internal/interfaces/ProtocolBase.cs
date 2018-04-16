/*|-----------------------------------------------------------------------------
 *|            This source code is provided under the Apache 2.0 license      --
 *|  and is provided AS IS with no warranty or guarantee of fit for purpose.  --
 *|                See the project's LICENSE.md for details.                  --
 *|           Copyright Thomson Reuters 2018. All rights reserved.            --
 *|-----------------------------------------------------------------------------
 */

using System.Collections.Generic;
using ThomsonReuters.Eta.Transports;
using ThomsonReuters.Eta.Transports.Interfaces;

namespace ThomsonReuters.Eta.Internal.Interfaces
{
    internal abstract class ProtocolBase : IProtocol
    {
        protected IList<ChannelBase> Channels = new List<ChannelBase>();

        #region IProtocol

        public abstract IChannel CreateChannel(ConnectOptions connectOptions, out Error error);

        public abstract void Uninitialize(out Error error);

        #endregion

        protected void ConnectChannel(ChannelBase channel, out Error error)
        {
            error = null;

            channel.Connect();

            Channels.Add(channel);
        }
    }
}
