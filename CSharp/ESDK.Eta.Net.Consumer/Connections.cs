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

using Microsoft.Extensions.Configuration;

namespace ThomsonReuters.Eta.Net.Consumer
{
    public class Connections
    {
        IList<Connection> _connections = new List<Connection>();

        string _environment;

        public Connections Register(Connection connection)
        {
            if (!_connections.Contains(connection))
                _connections.Add(connection);

            return this;
        }

        public Connections RegisterAll(IConfiguration configuration, string environment)
        {
            _environment = environment;

            var configurationSection = configuration.GetSection($"Environments:{environment}:Connections");
            if (configurationSection != null)
            {
                foreach(var item in configurationSection.GetChildren())
                {
                    var connection = item.Get<Connection>();
                    Register(connection);
                }
            }

            return this;
        }

        public IEnumerable<Connection> GetConnections() => _connections;

        public IEnumerable<Connection> GetConnections(string connectionNames)
            => connectionNames.Split(new char[] { ',', ' ' }, StringSplitOptions.RemoveEmptyEntries)
                    .Intersect(_connections.Select(cnxn => cnxn.ConnectionName))
            .Select(s => this[s]);

        public Connection this[string connectionName] 
            =>  _connections.FirstOrDefault(cnxn => 
                    cnxn.ConnectionName.Equals(connectionName,
                                               StringComparison.OrdinalIgnoreCase));
    }
}
