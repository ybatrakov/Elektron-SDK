/*|-----------------------------------------------------------------------------
 *|            This source code is provided under the Apache 2.0 license      --
 *|  and is provided AS IS with no warranty or guarantee of fit for purpose.  --
 *|                See the project's LICENSE.md for details.                  --
 *|           Copyright Thomson Reuters 2018. All rights reserved.            --
 *|-----------------------------------------------------------------------------
 */

using System;
using System.Text;
using System.Collections.Generic;
using ThomsonReuters.Eta.Codec;
using ThomsonReuters.Eta.Rdm;

namespace ThomsonReuters.Eta.Net.Consumer
{
    public class MarketPriceRequest
    {
        private readonly Qos qos;
        private int priorityClass;
        private int priorityCount;
        private int flags;

        private static readonly int VIEW_TYPE = ViewTypes.FIELD_ID_LIST;

        private readonly static String eolChar = "\n";
        private readonly static String tabChar = "\t";

        private IRequestMsg requestMsg = new Msg();
        private ElementList elementList = new ElementList();
        private ElementEntry elementEntry = new ElementEntry();
        private ThomsonReuters.Eta.Codec.Array array = new ThomsonReuters.Eta.Codec.Array();
        private ArrayEntry arrayEntry = new ArrayEntry();
        private ThomsonReuters.Eta.Codec.Buffer itemNameBuf = new ThomsonReuters.Eta.Codec.Buffer();

        private Int tempInt = new Int();
        private UInt tempUInt = new UInt();
        private ThomsonReuters.Eta.Codec.Array viewArray = new ThomsonReuters.Eta.Codec.Array();
        private StringBuilder stringBuf = new StringBuilder();

        public MarketPriceRequest() : this((int)Rdm.DomainType.MARKET_PRICE)
        {
        }

        public MarketPriceRequest(int domainType)
        {
            qos = new Qos();
            ItemNames = new List<String>();
            ViewFields = new List<String>();
            priorityClass = 1;
            priorityCount = 1;
            flags = 0;
            Identifier = -1;
            this.DomainType = domainType;
        }

        /**
         * Clears the current contents of this object and prepares it for re-use.
         */
        public void Clear()
        {
            flags = 0;
            qos.Clear();
            ItemNames.Clear();
            priorityClass = 1;
            priorityCount = 1;
            Identifier = -1;
            ViewFields.Clear();
        }

        /**
         * Checks the presence of private stream flag.
         * 
         * @return true - if exists; false if does not exist.
         */
        public bool CheckPrivateStream()
        {
            return (flags & MarketPriceRequestFlags.PRIVATE_STREAM) != 0;
        }

        /**
         * Applies private stream flag.
         */
        public void ApplyPrivateStream()
        {
            flags |= MarketPriceRequestFlags.PRIVATE_STREAM;
        }

        public int StreamId
        {
            get;set;
        }

        /**
         * 
         * @return service id
         */
        public int ServiceId
        {
            get;set;
        }


        /**
         * 
         * @return list of item names
         */
        public List<String> ItemNames
        {
            get;
        }

        /**
         * 
         * @return priority class used by request
         */
        public int PriorityClass()
        {
            return priorityClass;
        }

        /**
         * 
         * @return priority count used by request
         */
        public int PriorityCount()
        {
            return priorityCount;
        }

        /**
         * 
         * @param priorityClass
         * @param priorityCount
         * 
         */
        public void Priority(int priorityClass, int priorityCount)
        {
            this.priorityClass = priorityClass;
            this.priorityCount = priorityCount;
        }

        /**
         * Checks the presence of streaming.
         * 
         * @return true - if exists; false if does not exist.
         */
        public bool CheckStreaming()
        {
            return (flags & MarketPriceRequestFlags.STREAMING) != 0;
        }

        /**
         * Applies streaming flag.
         */
        public void ApplyStreaming()
        {
            flags |= MarketPriceRequestFlags.STREAMING;
        }

        /**
         * Checks the presence of Qos.
         * 
         * @return true - if exists; false if does not exist.
         */
        public bool CheckHasQos()
        {
            return (flags & MarketPriceRequestFlags.HAS_QOS) != 0;
        }

        /**
         * Applies Qos flag.
         */
        public void ApplyHasQos()
        {
            flags |= MarketPriceRequestFlags.HAS_QOS;
        }

        /**
         * Checks the presence of Priority flag.
         * 
         * @return true - if exists; false if does not exist.
         */
        public bool CheckHasPriority()
        {
            return (flags & MarketPriceRequestFlags.HAS_PRIORITY) != 0;
        }

        /**
         * Applies Priority flag.
         */
        public void ApplyHasPriority()
        {
            flags |= MarketPriceRequestFlags.HAS_PRIORITY;
        }

        /**
         * Applies View flag.
         */
        public void ApplyHasView()
        {
            flags |= MarketPriceRequestFlags.HAS_VIEW;
        }

        /**
         * Checks the presence of View flag.
         * 
         * @return true - if exists; false if does not exist.
         */
        public bool CheckHasView()
        {
            return (flags & MarketPriceRequestFlags.HAS_VIEW) != 0;
        }

        /**
         * 
         * @return Qos used by request
         */
        public Qos Qos()
        {
            return qos;
        }

        /**
         * 
         * @return list of view fields
         */
        public List<String> ViewFields
        {
            get;
        }

        /**
         * Checks the presence of service id flag.
         * @return true - if exists; false if does not exist.
         */
        public bool CheckHasServiceId()
        {
            return (flags & MarketPriceRequestFlags.HAS_SERVICE_ID) != 0;
        }

        /**
         * Applies service id flag.
         */
        public void ApplyHasServiceId()
        {
            flags |= MarketPriceRequestFlags.HAS_SERVICE_ID;
        }

        /**
         * 
         * @return Domain type
         */
        public int DomainType
        {
            get;private set;
        }


        /** 
         * @return Identifier
         */
        public int Identifier
        {
            get;set;
        }

        /**
         * Checks the presence of an identifier
         * @return true - if exists; false if does not exist;
         */
        public bool CheckHasIdentifier()
        {
            if (Identifier >= 0)
                return true;
            else
                return false;
        }

        /**
         * Encodes the item request.
         * 
         * @param encodeIter The Encode Iterator
         * @return {@link CodecReturnCode#SUCCESS} if encoding succeeds or failure
         *         if encoding fails.
         * 
         *         This method is only used within the Market By Price Handler and
         *         each handler has its own implementation, although much is similar
         */
        public CodecReturnCode Encode(EncodeIterator encodeIter)
        {
            requestMsg.Clear();
            elementList.Clear();
            elementEntry.Clear();
            array.Clear();
            itemNameBuf.Clear();

            /* set-up message */
            requestMsg.MsgClass = MsgClasses.REQUEST;
            requestMsg.StreamId = StreamId;
            requestMsg.DomainType = DomainType;
            requestMsg.ContainerType = DataTypes.NO_DATA;

            if (CheckHasQos())
            {
                requestMsg.ApplyHasQos();
                requestMsg.Qos.IsDynamic = qos.IsDynamic;
                requestMsg.Qos.Rate(qos.Rate());
                requestMsg.Qos.Timeliness(qos.Timeliness());
                requestMsg.Qos.RateInfo(qos.RateInfo());
                requestMsg.Qos.TimeInfo(qos.TimeInfo());
            }

            if (CheckHasPriority())
            {
                requestMsg.ApplyHasPriority();
                requestMsg.Priority.PriorityClass = PriorityClass();
                requestMsg.Priority.Count = PriorityCount();
            }

            if (CheckStreaming())
            {
                requestMsg.ApplyStreaming();
            }

            bool isBatchRequest = ItemNames.Count > 1;
            ApplyFeatureFlags(isBatchRequest);

            /* specify msgKey members */
            requestMsg.MsgKey.ApplyHasServiceId();
            requestMsg.MsgKey.ServiceId = ServiceId;

            requestMsg.MsgKey.ApplyHasNameType();
            requestMsg.MsgKey.NameType = InstrumentNameTypes.RIC;

            /* If user set Identifier */
            if (CheckHasIdentifier())
            {
                requestMsg.MsgKey.ApplyHasIdentifier();
                requestMsg.MsgKey.Identifier = Identifier;
            }

            if (!isBatchRequest && !CheckHasIdentifier())
            {
                requestMsg.MsgKey.ApplyHasName();
                requestMsg.MsgKey.Name.Data(ItemNames[0]);
            }

            CodecReturnCode ret = requestMsg.EncodeInit(encodeIter, 0);
            if (ret < CodecReturnCode.SUCCESS)
            {
                return ret;
            }

            /* encode request message payload */
            if (CheckHasView() || isBatchRequest)
            {
                ret = EncodeRequestPayload(isBatchRequest, encodeIter);
                if (ret < CodecReturnCode.SUCCESS)
                    return ret;
            }
            ret = requestMsg.EncodeComplete(encodeIter, true);
            if (ret < CodecReturnCode.SUCCESS)
            {
                return ret;
            }

            return CodecReturnCode.SUCCESS;
        }

        private CodecReturnCode EncodeRequestPayload(bool isBatchRequest, EncodeIterator encodeIter)
        {
            requestMsg.ContainerType = DataTypes.ELEMENT_LIST;
            elementList.ApplyHasStandardData();

            CodecReturnCode ret = elementList.EncodeInit(encodeIter, null, 0);
            if (ret < CodecReturnCode.SUCCESS)
            {
                return ret;
            }

            if (isBatchRequest
                    && (EncodeBatchRequest(encodeIter) < CodecReturnCode.SUCCESS))
            {
                return CodecReturnCode.FAILURE;
            }

            if (CheckHasView() && ViewFields != null &&
                    (EncodeViewRequest(encodeIter) < CodecReturnCode.SUCCESS))
            {
                return CodecReturnCode.FAILURE;
            }

            ret = elementList.EncodeComplete(encodeIter, true);
            if (ret < CodecReturnCode.SUCCESS)
            {
                return ret;
            }

            return CodecReturnCode.SUCCESS;
        }

        private void ApplyFeatureFlags(bool isBatchRequest)
        {
            if (CheckPrivateStream())
            {
                requestMsg.ApplyPrivateStream();
            }

            if (CheckHasView() || isBatchRequest)
            {
                requestMsg.ContainerType = DataTypes.ELEMENT_LIST;
                if (CheckHasView())
                {
                    requestMsg.ApplyHasView();
                }
                if (isBatchRequest)
                {
                    requestMsg.ApplyHasBatch();
                }
            }
        }

        private CodecReturnCode EncodeBatchRequest(EncodeIterator encodeIter)
        {
            /*
             * For Batch requests, the message has a payload of an element list that
             * contains an array of the requested items
             */

            elementEntry.Name = ElementNames.BATCH_ITEM_LIST;
            elementEntry.DataType = DataTypes.ARRAY;
            CodecReturnCode ret = elementEntry.EncodeInit(encodeIter, 0);
            if (ret < CodecReturnCode.SUCCESS)
            {
                return ret;
            }

            /* Encode the array of requested item names */
            array.PrimitiveType = DataTypes.ASCII_STRING;
            array.ItemLength = 0;

            ret = array.EncodeInit(encodeIter);
            if (ret < CodecReturnCode.SUCCESS)
            {
                return ret;
            }

            foreach (String itemName in ItemNames)
            {
                arrayEntry.Clear();
                itemNameBuf.Data(itemName);
                ret = arrayEntry.Encode(encodeIter, itemNameBuf);
                if (ret < CodecReturnCode.SUCCESS)
                {
                    return ret;
                }
            }

            ret = array.EncodeComplete(encodeIter, true);
            if (ret < CodecReturnCode.SUCCESS)
            {
                return ret;
            }

            ret = elementEntry.EncodeComplete(encodeIter, true);
            if (ret < CodecReturnCode.SUCCESS)
            {
                return ret;
            }

            return CodecReturnCode.SUCCESS;
        }

        /*
         * Encodes the View Element Entry. This entry contains an array of FIDs that
         * the consumer wishes to receive from the provider.
         * 
         * This method is only used within the Market Price Handler
         */
        private CodecReturnCode EncodeViewRequest(EncodeIterator encodeIter)
        {
            elementEntry.Clear();
            elementEntry.Name = ElementNames.VIEW_TYPE;
            elementEntry.DataType = DataTypes.UINT;
            tempUInt.Value(VIEW_TYPE);
            CodecReturnCode ret = elementEntry.Encode(encodeIter, tempUInt);
            if (ret < CodecReturnCode.SUCCESS)
            {
                return ret;
            }

            elementEntry.Clear();
            elementEntry.Name = ElementNames.VIEW_DATA;
            elementEntry.DataType = DataTypes.ARRAY;
            if ((ret = elementEntry.EncodeInit(encodeIter, 0)) < CodecReturnCode.SUCCESS)
            {
                return ret;
            }

            viewArray.PrimitiveType = DataTypes.INT;
            viewArray.ItemLength = 2;

            if ((ret = viewArray.EncodeInit(encodeIter)) < CodecReturnCode.SUCCESS)
            {
                return ret;
            }

            foreach (String viewField in ViewFields)
            {
                arrayEntry.Clear();
                tempInt.Value(System.Convert.ToInt32(viewField));
                ret = arrayEntry.Encode(encodeIter, tempInt);
                if (ret < CodecReturnCode.SUCCESS)
                {
                    return ret;
                }
            }
            ret = viewArray.EncodeComplete(encodeIter, true);

            if (ret < CodecReturnCode.SUCCESS)
            {
                return ret;
            }

            ret = elementEntry.EncodeComplete(encodeIter, true);
            if (ret < CodecReturnCode.SUCCESS)
            {
                return ret;
            }

            return CodecReturnCode.SUCCESS;
        }

        public override String ToString()
        {
            stringBuf.Clear();
            stringBuf.Append(tabChar);
            stringBuf.Append("StreamId: ");
            stringBuf.Append(StreamId);
            stringBuf.Append(eolChar);
            stringBuf.Insert(0, "MarketPriceRequest: \n");
            stringBuf.Append(tabChar);
            stringBuf.Append("Streaming: ");
            stringBuf.Append(CheckStreaming());
            stringBuf.Append(eolChar);

            stringBuf.Append(tabChar);
            stringBuf.Append("Domain: ");
            stringBuf.Append(Rdm.DomainTypes.ToString(DomainType));
            stringBuf.Append(eolChar);

            stringBuf.Append(tabChar);
            stringBuf.Append("IsPrivateStream: ");
            stringBuf.Append(CheckPrivateStream());
            stringBuf.Append(eolChar);

            stringBuf.Append(tabChar);
            stringBuf.Append("HasView: ");
            stringBuf.Append(CheckHasView());
            stringBuf.Append(eolChar);

            if (!CheckHasIdentifier())
            {
                stringBuf.Append(tabChar);
                stringBuf.Append("ItemNames: ");

                foreach (string name in ItemNames)
                {
                    stringBuf.Append(name).Append(",");
                }

                stringBuf.Append(eolChar);
            }
            else
            {
                stringBuf.Append(tabChar);
                stringBuf.Append("Identifier: ");
                stringBuf.Append(Identifier);
                stringBuf.Append(eolChar);
            }

            if (CheckHasServiceId())
            {
                stringBuf.Append(tabChar);
                stringBuf.Append("ServiceId: ");
                stringBuf.Append(ServiceId);
                stringBuf.Append(eolChar);
            }
            if (CheckHasPriority())
            {
                stringBuf.Append(tabChar);
                stringBuf.Append("Priority class: ");
                stringBuf.Append(PriorityClass());
                stringBuf.Append(", Priority count: ");
                stringBuf.Append(PriorityCount());
                stringBuf.Append(eolChar);
            }
            if (CheckHasQos())
            {
                stringBuf.Append(tabChar);
                stringBuf.Append("Qos: ");
                stringBuf.Append(qos.ToString());
                stringBuf.Append(eolChar);
            }

            return stringBuf.ToString();
        }
    }
}
