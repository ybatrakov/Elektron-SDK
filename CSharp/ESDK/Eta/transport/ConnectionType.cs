/*|-----------------------------------------------------------------------------
 *|            This source code is provided under the Apache 2.0 license      --
 *|  and is provided AS IS with no warranty or guarantee of fit for purpose.  --
 *|                See the project's LICENSE.md for details.                  --
 *|           Copyright Thomson Reuters 2018. All rights reserved.            --
 *|-----------------------------------------------------------------------------
 */

using ThomsonReuters.Eta.Transports.Interfaces;

namespace ThomsonReuters.Eta.Transports
{
    /// <summary>
    /// ETA Connection types are used in several areas of the transport. When
    /// creating a connection an application can specify the connection type to use.
    /// </summary>
    public enum ConnectionType
    {
        /// <summary>
        /// Indicates that the <see cref="IChannel"/> is using a standard TCP-based socket
        /// connection. This type can be used to connect between any ETA Transport
        /// based applications.
        /// </summary>
        SOCKET = 0,


        /// <summary>
        /// Indicates that the <see cref="IChannel"/> is using an SSL/TLS encrypted
        /// HTTP TCP-based socket connection. This type can be used by
        /// a ETA Transport consumer based application.
        /// </summary>
        ENCRYPTED = 1,


        /// <summary>
        /// Indicates that the <see cref="IChannel"/> is using an HTTP TCP-based socket
        /// connection. This type can be used by a ETA Transport
        /// consumer based application.
        /// </summary>
        HTTP = 2,

        /// <summary>
        /// Indicates that the <see cref="IChannel"/> is using a unidirectional shared
        /// memory connection. This type can be used to send from a shared memory
        /// server to one or more shared memory clients.
        /// </summary>
        UNIDIR_SHMEM = 3,

        /// <summary>
        /// Indicates that the <see cref="IChannel"/> is using a reliable multicast based
        /// connection. This type can be used to connect on a unified/mesh network
        /// where send and receive networks are the same or a segmented network where
        /// send and receive networks are different.
        /// </summary>
        RELIABLE_MCAST = 4,

        /// <summary>
        /// The sequence multicast connection
        /// </summary>
        SEQUENCED_MCAST = 6,

        /// <summary>
        /// max defined connectionType
        /// </summary>
        MAX_DEFINED = SEQUENCED_MCAST
    }
}
