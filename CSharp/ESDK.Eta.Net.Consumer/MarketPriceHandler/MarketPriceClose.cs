/*|-----------------------------------------------------------------------------
 *|            This source code is provided under the Apache 2.0 license      --
 *|  and is provided AS IS with no warranty or guarantee of fit for purpose.  --
 *|                See the project's LICENSE.md for details.                  --
 *|           Copyright Thomson Reuters 2018. All rights reserved.            --
 *|-----------------------------------------------------------------------------
 */

using System;
using System.Text;
using ThomsonReuters.Eta.Codec;
using ThomsonReuters.Eta.Rdm;

namespace ThomsonReuters.Eta.Net.Consumer
{
    public class MarketPriceClose
    {
        private static readonly String eolChar = "\n";
        private static readonly String tabChar = "\t";
        private ICloseMsg closeMsg = new Msg();
        private StringBuilder stringBuf = new StringBuilder();

        public MarketPriceClose()
        {
            DomainType = (int)Rdm.DomainType.MARKET_PRICE;
        }

        public void Clear()
        {
            DomainType = (int)Rdm.DomainType.MARKET_PRICE;
        }

        public int DomainType
        {
            get; private set;
        }

        public int StreamId
        {
            get; set;
        }

        public CodecReturnCode Encode(EncodeIterator encodeIter)
        {
            closeMsg.Clear();
            closeMsg.MsgClass = MsgClasses.CLOSE;
            closeMsg.StreamId = StreamId;
            closeMsg.DomainType = DomainType;
            closeMsg.ContainerType = DataTypes.NO_DATA;

            return closeMsg.Encode(encodeIter);
        }

        public CodecReturnCode Decode(DecodeIterator dIter, Msg msg)
        {
            Clear();
            if (msg.MsgClass != MsgClasses.CLOSE)
                return CodecReturnCode.FAILURE;

            StreamId = msg.StreamId;

            return CodecReturnCode.SUCCESS;
        }

        public override String ToString()
        {
            stringBuf.Clear();
            stringBuf.Append(tabChar);
            stringBuf.Append("StreamId: ");
            stringBuf.Append(StreamId);
            stringBuf.Append(eolChar);
            stringBuf.Insert(0, "MarketPriceClose: \n");
            stringBuf.Append(tabChar);
            stringBuf.Append("Domain: ");
            stringBuf.Append(Rdm.DomainTypes.ToString(DomainType));
            stringBuf.Append(eolChar);
            return stringBuf.ToString();
        }
    }
}
