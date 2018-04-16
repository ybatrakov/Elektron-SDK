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
using ThomsonReuters.Eta.Codec;
using ThomsonReuters.Eta.Rdm;
using Xunit;
using Xunit.Categories;
using ThomsonReuters.Eta.Net.Consumer;

namespace ThomsonReuters.Eta.Transports.Tests
{
    public class MarketPriceCloseTest
    {
        [Fact]
        [Category("Unit")]
        public void ConstructorTest()
        {
            MarketPriceClose mpc = new MarketPriceClose();

            Assert.Equal((int)DomainType.MARKET_PRICE, mpc.DomainType);
        }

        [Fact]
        [Category("Unit")]
        public void EncodeDecodeTest()
        {
            EncodeIterator encodeIterator = new EncodeIterator();
            Codec.Buffer encodedBuffer = new Codec.Buffer();
            encodedBuffer.Data(new Common.ByteBuffer(1024));

            Assert.Equal(CodecReturnCode.SUCCESS, encodeIterator.SetBufferAndRWFVersion(encodedBuffer, Codec.Codec.MajorVersion(), Codec.Codec.MinorVersion()));

            MarketPriceClose mpc = new MarketPriceClose
            {
                StreamId = 5
            };

            Assert.Equal(CodecReturnCode.SUCCESS, mpc.Encode(encodeIterator));
            Assert.True(encodedBuffer.Data().WritePosition > 0);
        }

        [Fact]
        [Category("Unit")]
        public void ToStringTest()
        {
            MarketPriceClose mpc = new MarketPriceClose
            {
                StreamId = 5
            };

            String expected = "MarketPriceClose: \n\tStreamId: 5\n\tDomain: MARKET_PRICE\n";

            Assert.Equal(expected, mpc.ToString());

        }
    }
}
