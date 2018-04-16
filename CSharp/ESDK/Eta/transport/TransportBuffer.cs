/*|-----------------------------------------------------------------------------
 *|            This source code is provided under the Apache 2.0 license      --
 *|  and is provided AS IS with no warranty or guarantee of fit for purpose.  --
 *|                See the project's LICENSE.md for details.                  --
 *|           Copyright Thomson Reuters 2018. All rights reserved.            --
 *|-----------------------------------------------------------------------------
 */

using System;

using ThomsonReuters.Eta.Common;
using ThomsonReuters.Common.Logger;
using ThomsonReuters.Eta.Transports.Internal;

namespace ThomsonReuters.Eta.Transports
{
    internal class TransportBuffer : EtaNode, ITransportBuffer
    {
        internal static readonly int PACKED_HDR = 2;
        internal static readonly int FRAGMENT_RIPC_FLAGS = 0x04;
        internal static readonly int FRAGMENT_HEADER_RIPC_FLAGS = 0x08;

        internal TransportBuffer()
        {
        }

        internal TransportBuffer(ByteBuffer data, bool skipHeader = true)
        {
            Data = data;

            if(skipHeader)
                Data.WritePosition += RipcDataMessage.HeaderSize;
        }

        internal void SetData(ByteBuffer data, bool skipHeader = true)
        {
            Data = data;

            if (skipHeader)
                Data.WritePosition += RipcDataMessage.HeaderSize;
        }

        internal void Clear()
        {
            Data = null;
        }

        internal static TransportBuffer Load(ref ByteBuffer ioBuffer)
        {
            TransportBuffer transportBuffer = null;

            //----------------------------------------------------------------------
            // Attempt to parse th ioBuffer for a RipcMessage
            RipcDataMessage dataMessage = default(RipcDataMessage);
            if (ioBuffer.ReadAt(ref dataMessage, ioBuffer.ReadPosition, false))
            {
                //------------------------------------------------------------------
                // A complete RipcMessage is available; peel a new TransportBuffer
                // out of the IO Buffer, but...

                ushort messageLength = 0;
                ByteBuffer dataBuffer;

                if (dataMessage.IsPacked)
                {   // ...is it a Packed Message (an envelope and a set of payloads), ...

                    if (dataMessage.MessageLength == RipcDataMessage.HeaderSize)
                        // Empty Packed Message; treat it like a 'ping' packet.
                        dataBuffer = ExtractDataMessage(ref ioBuffer, dataMessage, out messageLength);
                    else
                        // Extract the lead sub-message from the envelope; compact the IO Buffer.
                        dataBuffer  = ExtractPackedMessage(ref ioBuffer, dataMessage, out messageLength);

                }
                else
                {   // ...or is it a Normal Message?
                    dataBuffer = ExtractDataMessage(ref ioBuffer, dataMessage, out messageLength);
                }

                transportBuffer = new TransportBuffer(dataBuffer, false);
            }
            else
            {
                //----------------------------------------------------------------------
                // The IO Buffer contains a partial message; compact the IO Buffer
                // to allow it to be sent around again the RecvBegin/LoadReadBuffer cycle.
                ioBuffer.Compact();
            }

            return transportBuffer;
        }

        private static ByteBuffer ExtractPackedMessage(ref ByteBuffer ioBuffer, RipcDataMessage dataMessage, out ushort messageLength)
        {
            // Remember where we started
            int readPosition = ioBuffer.ReadPosition;
            // Skip past the header
            ioBuffer.ReadPosition += RipcDataMessage.HeaderSize;

            // Read the 1st payload into the dataBuffer
            messageLength = (ushort)ioBuffer.ReadShort();

            ByteBuffer dataBuffer = new ByteBuffer(messageLength + RipcDataMessage.HeaderSize);
            dataBuffer.Write((short)(messageLength + RipcDataMessage.HeaderSize));
            dataBuffer.Write((byte)RipcFlags.DATA);
            dataBuffer.Put(ioBuffer.Contents, ioBuffer.ReadPosition, dataBuffer.Limit - RipcDataMessage.HeaderSize);

            //------------------------------------------------------------
            // Rewrite the IO Buffer
            // Shift the first message to the head of the payload area
            Buffer.BlockCopy(ioBuffer.Contents,
                             ioBuffer.ReadPosition + messageLength,
                             ioBuffer.Contents,
                             ioBuffer.ReadPosition - sizeof(ushort),
                             ioBuffer.WritePosition - (ioBuffer.ReadPosition + messageLength));

            // Decrement the IO Buffer's MessageLength
            dataMessage.MessageLength -= (ushort)(messageLength + sizeof(ushort));
            ioBuffer.WriteAt(readPosition, (short)dataMessage.MessageLength);

            // Decrement the IO Buffer Write Position, as well
            ioBuffer.WritePosition -= (ushort)(messageLength + sizeof(ushort));

            // Backfill the abandoned portion of the IO Buffer
            int writePosition = ioBuffer.WritePosition;
            while (ioBuffer.WritePosition < ioBuffer.Capacity)
                ioBuffer.Write((byte)0xAA);
            ioBuffer.WritePosition = writePosition;

            // Reset the Read Position to it's original value
            ioBuffer.ReadPosition = (ioBuffer.WritePosition == readPosition + RipcDataMessage.HeaderSize)
                                            ? ioBuffer.WritePosition
                                            : readPosition;

            return dataBuffer;
        }

        private static ByteBuffer ExtractDataMessage(ref ByteBuffer ioBuffer, RipcDataMessage dataMessage, out ushort messageLength)
        {
            messageLength = dataMessage.MessageLength;
            ByteBuffer dataBuffer = new ByteBuffer(ioBuffer.Contents, false)
            {
                ReadPosition = ioBuffer.ReadPosition,
                WritePosition = messageLength
            };

            ioBuffer.ReadPosition += dataBuffer.WritePosition;
            return dataBuffer;
        }

        public ByteBuffer Data { get; private set; }

        public int Length => Data.WritePosition - RipcDataMessage.HeaderSize;

        public int Capacity => Data.Capacity - RipcDataMessage.HeaderSize;

        public int DataStartPosition => Data.ReadPosition + RipcDataMessage.HeaderSize;

        public TransportReturnCode Copy(ByteBuffer destination)
        {
            if (destination is null)
                throw new ArgumentNullException(nameof(destination));

            if (destination.Capacity < (Length))
                throw new InsufficientMemoryException(nameof(destination));

            TransportReturnCode result = TransportReturnCode.FAILURE;

            try
            {
                destination.Put(Data.Contents, destination.WritePosition, Length);
                result = TransportReturnCode.SUCCESS;
            }
            catch (Exception exp)
            {
                EtaLogger.Instance.Error(exp);
            }

            return result;
        }

        public override string ToString()
        {
            return $"Data: {{{Data}}}";
        }

        internal bool IsRipcMessage
        {
            get
            {
                // Data contains at least a Header
                bool isRipcMessage = Data.WritePosition >= RipcDataMessage.HeaderSize;
                if (isRipcMessage)
                {
                    short msgLength = Data.ReadShortAt(0);
                    // Payload is complete
                    isRipcMessage = isRipcMessage && (msgLength == Length + RipcDataMessage.HeaderSize);

                    // RipcFlags.Data 
                    RipcFlags flags = (RipcFlags)Data.ReadByteAt(sizeof(short));
                    isRipcMessage = isRipcMessage && ((flags & RipcFlags.DATA) == RipcFlags.DATA);
                }

                return isRipcMessage;
            }
        }
    }
}

