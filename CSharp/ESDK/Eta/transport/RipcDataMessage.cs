/*|-----------------------------------------------------------------------------
 *|            This source code is provided under the Apache 2.0 license      --
 *|  and is provided AS IS with no warranty or guarantee of fit for purpose.  --
 *|                See the project's LICENSE.md for details.                  --
 *|           Copyright Thomson Reuters 2018. All rights reserved.            --
 *|-----------------------------------------------------------------------------
 */

using System;
using System.Linq;

using ThomsonReuters.Eta.Transports.Internal;

namespace ThomsonReuters.Eta.Transports
{
    internal struct RipcDataMessage
    {
        public ushort MessageLength;

        public RipcFlags Flags;

        public byte[] Payload;

        public static ushort HeaderSize = sizeof(ushort) + sizeof(byte);

        public bool IsEmptyPayload { get => (Payload == null || Payload.Length == 0); }

        public bool IsPacked { get => (Flags & RipcFlags.PACKING) == RipcFlags.PACKING; }

        public RipcDataMessage(ushort capacity)
        {
            MessageLength = (ushort)(HeaderSize + capacity);
            Flags = RipcFlags.DATA;
            Payload = new byte[capacity];
        }

        internal RipcDataMessage(byte[] payload, RipcFlags flags = RipcFlags.DATA)
        {
            MessageLength = (ushort)(HeaderSize + payload.Length);
            Flags = flags;
            Payload = payload;
        }

        public override string ToString()
        {
            var byteStrings = (Payload.Take((Payload.Length < 32)?Payload.Length:32).Select(b => $"{b:X2}"));
            string payload = String.Join(", ", byteStrings);
            return $"MessageLength: {MessageLength}, Flags: {Flags}, HeaderSize: {HeaderSize}, Payload: byte[{Payload?.Length}] {{ {payload} }}";
        }
    }
}
