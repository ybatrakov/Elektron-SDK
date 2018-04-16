/*|-----------------------------------------------------------------------------
 *|            This source code is provided under the Apache 2.0 license      --
 *|  and is provided AS IS with no warranty or guarantee of fit for purpose.  --
 *|                See the project's LICENSE.md for details.                  --
 *|           Copyright Thomson Reuters 2018. All rights reserved.            --
 *|-----------------------------------------------------------------------------
 */

using System;
using System.Collections.Generic;
using System.Collections.Specialized;
using System.Linq;
using System.Text;

using Microsoft.Extensions.Configuration;
using Microsoft.Extensions.Logging;

namespace ThomsonReuters.Eta.Net.Consumer
{
    /// <summary>
    /// Configuration to hold and retrieve 
    /// AppSettings settings from settings.json.
    /// </summary>
    public static class EtaConfiguration
    {

        static bool isDebug =
#if DEBUG
                true;
#else
                false;
#endif

        // Properties
        public static string ItemList
        { get => _configuration.Value.GetValue<string>("ItemList", "TRI.N"); }
        public static string Application
        { get => _configuration.Value.GetValue<string>("Application", "EtaConsumer"); }

        public static string Environment
        { get => _configuration.Value.GetValue<string>("Environment", isDebug ? "Debug" : ""); }

        public static int PingInterval
        { get => _configuration.Value.GetValue<int>("PingInterval", 60); }

        public static int MaxMessageLength
        { get => _configuration.Value.GetValue<int>("MaxMessageLength", 8192); }

        public static bool LogToConsole
        { get => _configuration.Value.GetValue<bool>("LogToConsole", false); }

        public static bool LogToFile
        { get => _configuration.Value.GetValue<bool>("LogToFile", true); }

        public static bool Blocking
        { get => _configuration.Value.GetValue<bool>("Blocking", true); }

        public static LogLevel LogLevel
        { get => _configuration.Value.GetValue<LogLevel>("LogLevel", LogLevel.Information); }

        public static bool GlobalLocking
        { get => _configuration.Value.GetValue<bool>("GlobalLocking", true); }

        public static string ServerAddress
        { get => GetConnections().FirstOrDefault().ServerAddress; }

        public static string UserName
        { get => GetConnections().FirstOrDefault().UserName; }

        public static int ServiceId
        { get => GetConnections().FirstOrDefault().ServiceId ?? 1; }

        static Lazy<IConfiguration> _configuration = new Lazy<IConfiguration>(() =>
        {
            var configBuilder = new ConfigurationBuilder()
                .SetBasePath(AppContext.BaseDirectory)
                .AddJsonFile("settings.json");
            return configBuilder.Build();
        });

        static Lazy<Connections> _connections = new Lazy<Connections>(() =>
        {
            var connections = new Connections();
            connections.RegisterAll(_configuration.Value, Environment);
            return connections;
        });

        public static string Connections
        {
            get => _configuration.Value.GetValue<string>("Connections");
        }

        public static IEnumerable<Connection> GetConnections()
                    => _connections.Value.GetConnections(Connections);

    }
}
