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
    public class mprTest
    {
        [Fact]
        [Category("Unit")]
        public void ConstructorTest()
        {
            MarketPriceRequest mpr = new MarketPriceRequest();

            Assert.NotNull(mpr.Qos());
            Assert.Empty(mpr.ItemNames);
            Assert.Empty(mpr.ViewFields);
            Assert.Equal(1, mpr.PriorityClass());
            Assert.Equal(1, mpr.PriorityCount());
            Assert.Equal(-1, mpr.Identifier);
            Assert.Equal(0, mpr.StreamId);
            Assert.Equal((int)DomainType.MARKET_PRICE, mpr.DomainType);
        }

        [Fact]
        [Category("Unit")]
        public void ClearTest()
        {
            MarketPriceRequest mpr = new MarketPriceRequest();

            mpr.ItemNames.Add("Item1");
            mpr.ItemNames.Add("Item2");
            mpr.ItemNames.Add("Item3");

            mpr.ViewFields.Add("1");
            mpr.ViewFields.Add("2");
            mpr.ViewFields.Add("3");

            mpr.Priority(5, 5);

            mpr.Identifier = 99;

            mpr.Clear();

            Assert.Empty(mpr.ItemNames);
            Assert.Empty(mpr.ViewFields);
            Assert.Equal(1, mpr.PriorityClass());
            Assert.Equal(1, mpr.PriorityCount());
            Assert.Equal(-1, mpr.Identifier);
        }

        [Fact]
        [Category("Unit")]
        public void ApplyAndCheckPrivateStreamTest()
        {
            MarketPriceRequest mpr = new MarketPriceRequest();

            Assert.False(mpr.CheckPrivateStream());
            mpr.ApplyPrivateStream();
            Assert.True(mpr.CheckPrivateStream());
        }

        [Fact]
        [Category("Unit")]
        public void PriorityClassCountTest()
        {
            MarketPriceRequest mpr = new MarketPriceRequest();

            mpr.Priority(4, 5);

            Assert.Equal(4, mpr.PriorityClass());
            Assert.Equal(5, mpr.PriorityCount());
        }

        [Fact]
        [Category("Unit")]
        public void ApplyAndCheckStreamingTest()
        {
            MarketPriceRequest mpr = new MarketPriceRequest();

            Assert.False(mpr.CheckStreaming());
            mpr.ApplyStreaming();
            Assert.True(mpr.CheckStreaming());
        }

        [Fact]
        [Category("Unit")]
        public void ApplyAndCheckQosTest()
        {
            MarketPriceRequest mpr = new MarketPriceRequest();

            Assert.False(mpr.CheckHasQos());
            mpr.ApplyHasQos();
            Assert.True(mpr.CheckHasQos());
        }

        [Fact]
        [Category("Unit")]
        public void ApplyAndCheckPriorityTest()
        {
            MarketPriceRequest mpr = new MarketPriceRequest();

            Assert.False(mpr.CheckHasPriority());
            mpr.ApplyHasPriority();
            Assert.True(mpr.CheckHasPriority());
        }

        [Fact]
        [Category("Unit")]
        public void ApplyAndCheckViewTest()
        {
            MarketPriceRequest mpr = new MarketPriceRequest();

            Assert.False(mpr.CheckHasView());
            mpr.ApplyHasView();
            Assert.True(mpr.CheckHasView());
        }

        [Fact]
        [Category("Unit")]
        public void ApplyAndHasServiceIdTest()
        {
            MarketPriceRequest mpr = new MarketPriceRequest();

            Assert.False(mpr.CheckHasServiceId());
            mpr.ApplyHasServiceId();
            Assert.True(mpr.CheckHasServiceId());
        }

        [Fact]
        [Category("Unit")]
        public void CheckHasIdentifierTest()
        {
            MarketPriceRequest mpr = new MarketPriceRequest();

            Assert.False(mpr.CheckHasIdentifier());
            mpr.Identifier = 5;
            Assert.True(mpr.CheckHasIdentifier());
        }

        [Fact]
        [Category("Unit")]
        public void EncodeTest()
        {
            EncodeIterator encodeIterator = new EncodeIterator();
            Codec.Buffer encodedBuffer = new Codec.Buffer();
            encodedBuffer.Data(new Common.ByteBuffer(1024));


            Assert.Equal(CodecReturnCode.SUCCESS, encodeIterator.SetBufferAndRWFVersion(encodedBuffer, Codec.Codec.MajorVersion(), Codec.Codec.MinorVersion()));

            MarketPriceRequest mpr = new MarketPriceRequest
            {
                StreamId = 5
            };
            mpr.Priority(2, 2);
            mpr.ApplyStreaming();
            mpr.Identifier = 5;
            mpr.ItemNames.Add("TRI.N");

            Assert.Equal(CodecReturnCode.SUCCESS, mpr.Encode(encodeIterator));

            Assert.True(encodedBuffer.Data().WritePosition > 0);
        }

        [Fact]
        [Category("Unit")]
        public void EncodePayloadBatchTest()
        {
            EncodeIterator encodeIterator = new EncodeIterator();
            Codec.Buffer encodedBuffer = new Codec.Buffer();
            encodedBuffer.Data(new Common.ByteBuffer(1024));


            Assert.Equal(CodecReturnCode.SUCCESS, encodeIterator.SetBufferAndRWFVersion(encodedBuffer, Codec.Codec.MajorVersion(), Codec.Codec.MinorVersion()));

            MarketPriceRequest mpr = new MarketPriceRequest
            {
                StreamId = 5
            };
            mpr.Priority(3, 3);
            mpr.Identifier = 6;
            mpr.ItemNames.Add("TRI.N");
            mpr.ItemNames.Add("IBM.N");
            mpr.ItemNames.Add("MSFT.O");

            Assert.Equal(CodecReturnCode.SUCCESS, mpr.Encode(encodeIterator));

            Assert.True(encodedBuffer.Data().WritePosition > 0);
        }

        [Fact]
        [Category("Unit")]
        public void EncodePayloadViewTest()
        {
            EncodeIterator encodeIterator = new EncodeIterator();
            Codec.Buffer encodedBuffer = new Codec.Buffer();
            encodedBuffer.Data(new Common.ByteBuffer(1024));


            Assert.Equal(CodecReturnCode.SUCCESS, encodeIterator.SetBufferAndRWFVersion(encodedBuffer, Codec.Codec.MajorVersion(), Codec.Codec.MinorVersion()));

            MarketPriceRequest mpr = new MarketPriceRequest
            {
                StreamId = 5
            };
            mpr.Priority(3, 3);
            mpr.Identifier = 6;
            mpr.ItemNames.Add("TRI.N");
            mpr.ViewFields.Add("15");
            mpr.ViewFields.Add("16");
            mpr.ViewFields.Add("17");

            Assert.Equal(CodecReturnCode.SUCCESS, mpr.Encode(encodeIterator));

            Assert.True(encodedBuffer.Data().WritePosition > 0);
        }

        [Fact]
        [Category("Unit")]
        public void ToStringTest()
        {
            MarketPriceRequest mpr = new MarketPriceRequest
            {
                StreamId = 5
            };
            mpr.Priority(3, 3);
            mpr.Identifier = 6;
            mpr.ApplyStreaming();
            mpr.ItemNames.Add("TRI.N");
            mpr.ViewFields.Add("15");
            mpr.ViewFields.Add("16");
            mpr.ViewFields.Add("17");

            String expected = "MarketPriceRequest: \n\tStreamId: 5\n\tStreaming: True\n\tDomain: MARKET_PRICE\n\tIsPrivateStream: False\n\tHasView: False\n\tIdentifier: 6\n";

            Assert.Equal(expected, mpr.ToString());

        }

    }
}
