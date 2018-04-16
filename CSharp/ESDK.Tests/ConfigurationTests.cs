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
using System.Text;

using Microsoft.Extensions.Configuration;

using Xunit;
using Xunit.Abstractions;
using Xunit.Categories;

using ThomsonReuters.Common.Logger;
using ThomsonReuters.Eta.Net.Consumer;

namespace ThomsonReuters.Eta.Tests
{
    [Category("Consumer")]
    public class ConfigurationTests
    {
        Connections _connections;

        IConfiguration _configuration;
        public ConfigurationTests(ITestOutputHelper output)
        {
            XUnitLoggerProvider.Instance.Output = output;
            EtaLoggerFactory.Instance.AddProvider(XUnitLoggerProvider.Instance);

            _connections = new Connections();

            var settings = new Dictionary<string, string>
            {
                {"Environments:Debug:Connections:0:ConnectionName",  "TestLoSpeed"},
                {"Environments:Debug:Connections:0:ServerAddress",  "10.91.160.16"},
                {"Environments:Debug:Connections:0:ServerPort",  "14999"},
                {"Environments:Debug:Connections:0:UserName",  "username"},
                {"Environments:Debug:Connections:0:ServiceId",  "1" },
                {"Environments:Debug:Connections:0:Handler",  "ThomsonReuters.Eta.Net.Consumer.DecodingBufferParser"},
                {"Environments:Debug:Connections:1:ConnectionName",  "TestLoSpeed2"},
                {"Environments:Debug:Connections:1:ServerAddress",  "10.91.160.16"},
                {"Environments:Debug:Connections:1:ServerPort",  "14999"},
                {"Environments:Debug:Connections:1:UserName",  "username"},
                {"Environments:Debug:Connections:1:ServiceId",  "1"},
                {"Environments:Debug:Connections:1:Handler",  "ThomsonReuters.Eta.Net.Consumer.DecodingBufferParser"},
                {"Environments:Debug:Connections:2:ConnectionName",  "TestHiSpeed"},
                {"Environments:Debug:Connections:2:ServerAddress",  "10.91.160.16"},
                {"Environments:Debug:Connections:2:ServerPort",  "15001"},
                {"Environments:Debug:Connections:2:UserName",  "username"},
                {"Environments:Debug:Connections:2:ServiceId",  "1" },
                {"Environments:Debug:Connections:2:Handler",  "ThomsonReuters.Eta.Net.Consumer.DecodingBufferParser"},
                {"Environments:Debug:Connections:3:ConnectionName",  "TestEta"},
                {"Environments:Debug:Connections:3:ServerAddress",  "127.0.0.1"},
                {"Environments:Debug:Connections:3:ServerPort",  "14002"},
                {"Environments:Debug:Connections:3:Handler",  "ThomsonReuters.Eta.Net.Consumer.TestBufferParser"},
                {"Environments:Release:Connections:0:ConnectionName",  "MarketFeed"},
                {"Environments:Release:Connections:0:ServerAddress",  "132.88.65.120"},
                {"Environments:Release:Connections:0:ServerPort",  "14002"},
                {"Environments:Release:Connections:0:UserName",  "trep1"},
                {"Environments:Release:Connections:0:ServiceId",  "957"},
                {"Environments:Release:Connections:0:Handler",  "ThomsonReuters.Eta.Net.Consumer.DecodingBufferParser"},
        };

            _configuration = new ConfigurationBuilder()
               .AddInMemoryCollection(settings)
               .Build();
        }

        [Fact]
        [Category("Unit")]
        public void ConfigurationContainsOneRootNode()
        {
            // It got buildt...
            Assert.NotNull(_configuration);

            var iterator = _configuration
                                .GetChildren();
            // It has been populated
            Assert.NotNull(iterator);

            int itemCount = 0;
            foreach (var item in iterator)
            {
                itemCount++;
            }
            // There is one root node...
            Assert.Equal(1, itemCount);
        }

        [Fact]
        [Category("Unit")]
        public void ConfigurationContainsNodesBelowRoot()
        {
            var iterator = _configuration
                                .GetSection("Environments")
                                .GetChildren();

            int itemCount = 0;
            foreach (var item in iterator)
            {
                itemCount++;
            }
            // There are 2 nodes below the root node.
            Assert.Equal(2, itemCount);
        }

        [Fact]
        [Category("Unit")]
        public void ConfigurationBindsConnections()
        {
            Assert.Empty(_connections.GetConnections());

            _connections.RegisterAll(_configuration, "Debug");

            Assert.NotEmpty(_connections.GetConnections());
        }


        [Fact]
        [Category("Unit")]
        public void ConfigurationBindsConnectionsByNameAddressPort()
        {
            _connections.RegisterAll(_configuration, "Debug");

            // There are two named connections with the same Address/Port
            var query = _connections
                            .GetConnections("TestLoSpeed,TestLoSpeed2")
                            .AsQueryable();
            Assert.Equal(2, query.Count());

            int distinctAddresses = query.Select(cnxn => cnxn.ServerAddress)
                                        .Distinct()
                                        .Count();
            Assert.Equal(1, distinctAddresses);
            int distinctPorts = query.Select(cnxn => cnxn.ServerPort)
                                        .Distinct()
                                        .Count();
            Assert.Equal(1, distinctPorts);
        }

        [Fact]
        [Category("Unit")]
        public void ConfigurationConnectionsFetchByConnectionName()
        {
            _connections.RegisterAll(_configuration, "Release");

            // Connections indexed by ConnectionName

            // Red test
            var connection = _connections["TestEta"];
            Assert.Null(connection);

            // Green test
            connection = _connections["MarketFeed"];
            Assert.NotNull(connection);
        }
    }
}
