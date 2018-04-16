/*|-----------------------------------------------------------------------------
 *|            This source code is provided under the Apache 2.0 license      --
 *|  and is provided AS IS with no warranty or guarantee of fit for purpose.  --
 *|                See the project's LICENSE.md for details.                  --
 *|           Copyright Thomson Reuters 2018. All rights reserved.            --
 *|-----------------------------------------------------------------------------
 */

using System;
using System.Collections.Generic;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

using Xunit;
using Xunit.Abstractions;
using Xunit.Categories;

using ThomsonReuters.Common.Logger;
using ThomsonReuters.Eta.Internal.Interfaces;
using ThomsonReuters.Eta.Tests;

namespace ThomsonReuters.Eta.Transports.Tests
{
    [Collection("Channel")]
    public class ChannelTests : IDisposable
    {
        static ChannelTests()
        {
            ThomsonReuters.Eta.Internal.ProtocolRegistry.Instance
                .Register(ConnectionType.UNIDIR_SHMEM, new ThomsonReuters.Eta.Tests.MockProtocol());
        }
        public ChannelTests(ITestOutputHelper output)
        {
            XUnitLoggerProvider.Instance.Output = output;
            EtaLoggerFactory.Instance.AddProvider(XUnitLoggerProvider.Instance);
        }

        public void Dispose()
        {
            XUnitLoggerProvider.Instance.Output = null;
        }

        [Fact(Skip = "Broken")]
        [Category("Unit")]
        [Category("Channel")]
        public void BlockingChannelConnects()
        {
            var cnxnOptions = new ConnectOptions
            {
                ConnectionType = ConnectionType.UNIDIR_SHMEM,
                Blocking = true,
            };
            cnxnOptions.UnifiedNetworkInfo.Address = "localhost";
            cnxnOptions.UnifiedNetworkInfo.ServiceName = MockProtocol.PortActionOnSignal.ToString();
            ChannelBase channel = new ChannelBase(cnxnOptions, new MockChannel());

            // Step through the MockChannel states.
            var stimulator = Task.Factory.StartNew(() =>
            {
                Thread.Sleep(100);
                channel.SocketChannel.IsConnected = true;
                Thread.Sleep(100);
                channel.SocketChannel.IsDataReady = true;
            });

            try
            {
                // Succesful if does not throw.
                channel.Connect();
            }
            finally
            {
                channel.Close(out Error error);
            }
        }
    }
}
