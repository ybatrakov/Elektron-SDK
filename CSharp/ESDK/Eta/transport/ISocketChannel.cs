/*|-----------------------------------------------------------------------------
 *|            This source code is provided under the Apache 2.0 license      --
 *|  and is provided AS IS with no warranty or guarantee of fit for purpose.  --
 *|                See the project's LICENSE.md for details.                  --
 *|           Copyright Thomson Reuters 2018. All rights reserved.            --
 *|-----------------------------------------------------------------------------
 */

using System;
using System.Net;
using System.Net.Sockets;

namespace ThomsonReuters.Eta.Transports
{
    internal interface ISocketChannel : IChannelSink, IDisposable
    {
        bool IsConnected { get; set; }
        bool IsDisposed { get; }
        bool IsServer { get; }
        bool IsDataReady { get; set; }

        EndPoint RemoteEP { get; }
        int RemotePort { get; }

        Guid ChannelId { get; }

        bool Connect(ConnectOptions connectOptions);
        bool Connect(ConnectOptions connectOptions, IPAddress remoteAddr, int port);

        void Disconnect();

        SocketError Receive(ResultObject aro);

        void Send(byte[] packet, object user_state = null);

        int Send(byte[] buffer, int position, int length);

        Socket Socket { get; }
    }
}