/*|-----------------------------------------------------------------------------
 *|            This source code is provided under the Apache 2.0 license      --
 *|  and is provided AS IS with no warranty or guarantee of fit for purpose.  --
 *|                See the project's LICENSE.md for details.                  --
 *|           Copyright Thomson Reuters 2018. All rights reserved.            --
 *|-----------------------------------------------------------------------------
 */

using System;

namespace ThomsonReuters.Eta.Transports.Internal
{
    internal enum ConnectionsVersions
    {
        VERSION14 = 0x0017,
        VERSION13 = 0x0016,
        VERSION12 = 0x0015,
        VERSION11 = 0x0014
    }

    internal struct RipcVersionInfo
    {
        public const int CurrentVersion = (int)ConnectionsVersions.VERSION13;
        public const int MaxUserMsgSize = 8192;
        public const byte PingTimeout = 0xFF;
        public const byte MajorVersion = 14;
        public const byte MinorVersion = 0;

        public const int ComponentVersionStringLenMax = 253;
    }

    [Flags]
    internal enum RipcFlags : byte
    {
        NONE = 0x00,
        HAS_OPTIONAL_FLAGS = 0x01,
        DATA = 0x02,
        COMPRESSED_DATA = 0x04,
        COMP_FRAGMENT = 0x08,
        PACKING = 0x10
    }

    internal enum RipcOpCode : byte
    {
        CONNECT_ACK = 0x01,
        CONNECT_NAK = 0x02
    }

    internal enum Unused : byte
    {

    }

    internal enum RipcSessionFlag : byte
    {
        NONE = 0x00,
        CLIENT_TO_SERVER_PING = 0x01,
        SERVER_TO_CLIENT_PING = 0x02
    }

}
