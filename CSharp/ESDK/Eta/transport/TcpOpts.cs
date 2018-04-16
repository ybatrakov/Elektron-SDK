/*|-----------------------------------------------------------------------------
 *|            This source code is provided under the Apache 2.0 license      --
 *|  and is provided AS IS with no warranty or guarantee of fit for purpose.  --
 *|                See the project's LICENSE.md for details.                  --
 *|           Copyright Thomson Reuters 2018. All rights reserved.            --
 *|-----------------------------------------------------------------------------
 */

namespace ThomsonReuters.Eta.Transports
{
    /// <summary>
    /// Options used for configuring TCP specific transport options
    /// (<see cref="ConnectionType.SOCKET"/>), (<see cref="ConnectionType.ENCRYPTED"/>), (<see cref="ConnectionType.HTTP"/>).
    /// <seealso cref="ConnectOptions"/>
    /// </summary>
    public class TcpOpts
    {
        /// <summary>
        /// Only used with connectionType of <see cref="ConnectionType.SOCKET"/>. If true, disables Nagle's Algorithm. 
        /// </summary>
        public bool TcpNoDelay { get; set; }
    }
}
