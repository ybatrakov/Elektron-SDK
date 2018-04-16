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

namespace ThomsonReuters.Eta.Net.Consumer
{
    public class Connection
    {
        public string ConnectionName { get; set; }

        public string ServerAddress { get; set; }

        public string ServerPort { get; set; }

        public string UserName { get; set; }

        public int? ServiceId { get; set; }

        public string Handler { get; set; }

        public override bool Equals(object obj)
        {
            var connection = obj as Connection;
            if (connection is null)
                return false;

            return ConnectionName.Equals(connection.ConnectionName, StringComparison.OrdinalIgnoreCase) 
                && ServerAddress.Equals(connection.ServerAddress, StringComparison.OrdinalIgnoreCase)
                && ServerPort.Equals(connection.ServerPort, StringComparison.OrdinalIgnoreCase);
        }

        public override int GetHashCode()
        {
            return ConnectionName.GetHashCode() ^ ServerAddress.GetHashCode() ^ ServerPort.GetHashCode();
        }

        public override string ToString()
        {
            return $"ConnectionName: {ConnectionName}, ServerAddress: {ServerAddress}, ServerPort: {ServerPort}, UserName: {UserName}, ServiceId: {ServiceId}, Handler: {Handler}";
        }
    }
}