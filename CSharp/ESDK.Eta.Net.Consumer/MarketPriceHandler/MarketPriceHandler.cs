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

using ThomsonReuters.Common.Logger;
using ThomsonReuters.Eta.Codec;
using ThomsonReuters.Eta.Rdm;
using ThomsonReuters.Eta.Transports;
using ThomsonReuters.Eta.Transports.Interfaces;

namespace ThomsonReuters.Eta.Net.Consumer
{
    public class MarketPriceHandler
    {
        public static readonly int TRANSPORT_BUFFER_SIZE_REQUEST = 1024;
        public static readonly int TRANSPORT_BUFFER_SIZE_CLOSE = 1024;

        int redirectSrcId;
        protected readonly StreamIdWatchList watchList;
        private IChannel redirectChnl;
        private int domainType;
        private MarketPriceRequest marketPriceRequest;
        private MarketPriceClose closeMessage;
        protected FieldList fieldList = new FieldList();
        protected FieldEntry fieldEntry = new FieldEntry();
        private UInt fidUIntValue = new UInt();
        private Int fidIntValue = new Int();
        private Real fidRealValue = new Real();
        private ThomsonReuters.Eta.Codec.Enum fidEnumValue = new ThomsonReuters.Eta.Codec.Enum();
        private ThomsonReuters.Eta.Codec.Date fidDateValue = new Date();
        private Time fidTimeValue = new Time();
        private ThomsonReuters.Eta.Codec.DateTime fidDateTimeValue = new ThomsonReuters.Eta.Codec.DateTime();
        private ThomsonReuters.Eta.Codec.Float fidFloatValue = new Float();
        private ThomsonReuters.Eta.Codec.Double fidDoubleValue = new ThomsonReuters.Eta.Codec.Double();
        private Qos fidQosValue = new Qos();
        private State fidStateValue = new State();
        private EncodeIterator encIter = new EncodeIterator();
        private List<String> viewFieldList;
        private WriteArgs writeArgs = new WriteArgs();

        public MarketPriceHandler(StreamIdWatchList watchList) : this((int)DomainType.MARKET_PRICE)
        {
            this.watchList = watchList;
        }

        protected MarketPriceHandler(int domainType)
        {
            this.domainType = domainType;
            marketPriceRequest = new MarketPriceRequest();
            closeMessage = new MarketPriceClose();
            viewFieldList = new List<String>
            {
                "6",
                "22",
                "25",
                "32"
            };

            ViewRequest = false;
            SnapshotRequest = false;
        }

        public bool ViewRequest
        {
            private get; set;
        }

        public bool SnapshotRequest
        {
            private get;set;
        }

        protected void RemoveMarketPriceItemEntry(int streamId)
        {
            watchList.Remove(streamId);
        }

        protected CodecReturnCode CloseStream(IChannel chnl, int streamId, out Error error)
        {
            /* get a buffer for the item close */
            ITransportBuffer msgBuf = chnl.GetBuffer(TRANSPORT_BUFFER_SIZE_CLOSE, false,
                                                             out error);
            if (msgBuf == null)
                return CodecReturnCode.FAILURE;

            /* encode item close */
            closeMessage.Clear();
            closeMessage.StreamId = streamId;
            encIter.Clear();
            encIter.SetBufferAndRWFVersion(msgBuf, chnl.MajorVersion, chnl.MinorVersion);

            CodecReturnCode ret = closeMessage.Encode(encIter);
            if (ret != CodecReturnCode.SUCCESS)
            {
                Console.WriteLine($"EncodeMarketPriceClose(): Failed <{ret.GetAsString()}>");
            }

            return (CodecReturnCode)chnl.Write(msgBuf, writeArgs, out error);
        }

        /*
         * this method is used while posting to query the first requested market
         * price item, if any. It will populate the passed in buffer with the name
         * and length information and return the streamId associated with the
         * stream. If mpItemName->length is 0 and streamId is returned as 0, this
         * indicates that there is no valid name available.
         */
        public int GetFirstItem(ThomsonReuters.Eta.Codec.Buffer mpItemName)
        {
            return watchList.GetFirstItem(mpItemName);
        }

        /**
     * Encodes and sends item requests for three market price domains
     * (MarketPrice, MarketByPrice, MarketByOrder).
     * 
     * @param chnl - The channel to send a source directory request to
     * 
     * @param itemNames - List of item names
     * 
     * @param isPrivateStream - flag indicating if requested items are private
     *            stream or not.
     * 
     * @param loginInfo - RDM login information
     * @param serviceInfo - RDM directory response information
     * 
     * @return success if item requests can be made, can be encoded and sent
     *         successfully. Failure if service does not support market price capability
     *         or failure for encoding/sending request.
     */
        public CodecReturnCode SendItemRequests(IChannel channel, List<String> itemNames, bool isPrivateStream,int  serviceId, out Error error)
        {
            error = null;
            if (itemNames == null || itemNames.Count == 0)
                return CodecReturnCode.SUCCESS;

            GenerateRequest(marketPriceRequest, isPrivateStream, serviceId);

            redirectChnl = channel;
            redirectSrcId = serviceId;

            // If there is only one item in the itemList, it is a waste of bandwidth
            // to send a batch request
            if (itemNames.Count == 1)
            {
                return SendRequest(channel, itemNames, out error);
            }

            // batch
            return SendBatchRequest(channel, itemNames, out error);
        }

        private void GenerateRequest(MarketPriceRequest marketPriceRequest, bool isPrivateStream, int serviceId)
        {
            marketPriceRequest.Clear();

            if (!SnapshotRequest)
                marketPriceRequest.ApplyStreaming();
            marketPriceRequest.ApplyHasServiceId();
            marketPriceRequest.ServiceId = serviceId;
            marketPriceRequest.ApplyHasPriority();
            marketPriceRequest.Priority(1, 1);
       
            if (isPrivateStream)
                marketPriceRequest.ApplyPrivateStream();

            if (ViewRequest)
            {
                marketPriceRequest.ApplyHasView();
                marketPriceRequest.ViewFields.AddRange(viewFieldList);
            }
        }


        //sends items as batch request
        private CodecReturnCode SendBatchRequest(IChannel channel, List<String> itemNames, out Error error)
        {
            int batchStreamId = watchList.Add(domainType, "BATCH_" + System.DateTime.UtcNow.ToLongTimeString(), marketPriceRequest.CheckPrivateStream());
            marketPriceRequest.StreamId = batchStreamId;
            int totalBytes = 0;
            foreach (String itemName in itemNames)
            {
                watchList.Add(domainType, itemName, marketPriceRequest.CheckPrivateStream());
                marketPriceRequest.ItemNames.Add(itemName);
                try
                {
                    totalBytes += Encoding.ASCII.GetByteCount(itemName);
                }
                catch (Exception)
                {
                }
            }

            return EncodeAndSendRequest(channel, marketPriceRequest, totalBytes, out error);
        }

        //sends one item at a time
        private CodecReturnCode SendRequest(IChannel channel, List<String> itemNames, out Error error)
        {
            CodecReturnCode ret = CodecReturnCode.SUCCESS;
            error = null;
            foreach (String itemName in itemNames)
            {
                int streamId = watchList.Add(domainType, itemName, marketPriceRequest.CheckPrivateStream());

                marketPriceRequest.ItemNames.Clear();
                marketPriceRequest.ItemNames.Add(itemName);
                marketPriceRequest.StreamId = streamId;
                ret = EncodeAndSendRequest(channel, marketPriceRequest, 0, out error);
                if (ret < CodecReturnCode.SUCCESS)
                    return ret;
            }

            return CodecReturnCode.SUCCESS;
        }

        private CodecReturnCode EncodeAndSendRequest(IChannel chnl, MarketPriceRequest marketPriceRequest, int totalBytes,
            out Error error)
        {
            int totalBytesToAllocate = Math.Min(totalBytes * 2, 2048 * 4);

            //get a buffer for the item request
            ITransportBuffer msgBuf = chnl.GetBuffer(Math.Max(TRANSPORT_BUFFER_SIZE_REQUEST, totalBytesToAllocate), false, out error);

            if (msgBuf == null)
            {
                return CodecReturnCode.FAILURE;

            }
            encIter.Clear();
            encIter.SetBufferAndRWFVersion(msgBuf, chnl.MajorVersion, chnl.MinorVersion);

            CodecReturnCode ret = marketPriceRequest.Encode(encIter);
            if (ret < CodecReturnCode.SUCCESS)
            {
                error = new Error((TransportReturnCode)ret, "MarketPriceRequest.Encode() failed");
                return ret;
            }

            Console.WriteLine(marketPriceRequest.ToString());
       
            return (CodecReturnCode)chnl.Write(msgBuf, writeArgs, out error);
        }

        /**
    * Publicly visible market price response handler
    * 
    * Processes a market price response. This consists of extracting the key,
    * printing out the item name contained in the key, decoding the field list
    * and field entry, and calling decodeFieldEntry() to decode the field entry
    * data.
    * 
    * @param msg - The partially decoded message
    * @param dIter - The decode iterator
    * 
    * @return success if decoding succeeds, failure if it fails.
    */
        public CodecReturnCode ProcessResponse(Msg msg, DecodeIterator dIter, DataDictionary dictionary, out Error error)
        {
            error = null;
            switch (msg.MsgClass)
            {
                case MsgClasses.REFRESH:
                    return HandleRefresh(msg, dIter, dictionary, out error);
                case MsgClasses.UPDATE:
                    return HandleUpdate(msg, dIter, dictionary);
                case MsgClasses.STATUS:
                    return HandleStatus(msg, out error);
                case MsgClasses.ACK:
                    return HandleAck(msg);
                default:
                    Console.WriteLine($"Received Unhandled Item Msg Class: {msg.MsgClass}");
                    break;
            }

            return CodecReturnCode.SUCCESS;
        }

        protected CodecReturnCode HandleAck(Msg msg)
        {
            Console.WriteLine($"Received AckMsg for stream {msg.StreamId}");

            StringBuilder fieldValue = new StringBuilder();
            GetItemName(msg, fieldValue);

            IAckMsg ackMsg = (IAckMsg)msg;

            fieldValue.Append("\tAckId=").Append(ackMsg.AckId);
            if (ackMsg.CheckHasSeqNum())
                fieldValue.Append("\tSeqNum=").Append(ackMsg.SeqNum);
            if (ackMsg.CheckHasNakCode())
                fieldValue.Append("\tNakCode=").Append(ackMsg.NakCode);
            if (ackMsg.CheckHasText())
                fieldValue.Append("\tText=").Append(ackMsg.Text.ToString());

            Console.WriteLine(fieldValue.ToString());
            return CodecReturnCode.SUCCESS;
        }

        protected void GetItemName(Msg msg, StringBuilder fieldValue)
        {
            // get key
            IMsgKey key = msg.MsgKey;

            // print out item name from key if it has it
            if (key != null && key.CheckHasName())
            {
                if (msg.MsgClass == MsgClasses.REFRESH)
                {
                    IRefreshMsg refreshMsg = (IRefreshMsg)msg;
                    fieldValue.Append(key.Name.ToString()).Append(refreshMsg.CheckPrivateStream() ? " (PRIVATE STREAM)" : "").Append("\nDOMAIN: ")
                        .Append(DomainTypes.ToString(msg.DomainType)).Append("\n");
                }
                else
                {
                    fieldValue.Append(key.Name.ToString() + "\nDOMAIN: " +
                            DomainTypes.ToString(msg.DomainType) + "\n");
                    if (msg.MsgClass == MsgClasses.UPDATE)
                    {
                        fieldValue.Append($"UPDATE TYPE: {UpdateEventTypes.ToString((msg).UpdateType)}\n");
                    }
                }

            }
            else
            // cached item name
            {
                WatchListEntry wle = watchList.Get(msg.StreamId);

                if (wle != null)
                {
                    fieldValue.Append(wle.ItemName + (wle.IsPrivateStream ? " (PRIVATE STREAM)" : " ") + "\nDOMAIN: " + DomainTypes.ToString(msg.DomainType) + "\n");
                    if (msg.MsgClass == MsgClasses.UPDATE)
                    {
                        fieldValue.Append($"UPDATE TYPE: {UpdateEventTypes.ToString(((IUpdateMsg)msg).UpdateType)}\n");
                    }
                }
                else
                {
                    // check if this is login stream for offstream posting
                    if (msg.StreamId == 1)
                    {
                        fieldValue.Append($"OFFPOST \nDOMAIN: {DomainTypes.ToString(msg.DomainType)}\n");
                    }
                }
            }
        }

        protected CodecReturnCode HandleStatus(Msg msg, out Error error)
        {
            IStatusMsg statusMsg = (IStatusMsg)msg;
            error = null;
            Console.WriteLine($"Received Item StatusMsg for stream {msg.StreamId}");
            if (!statusMsg.CheckHasState())
                return CodecReturnCode.SUCCESS;

            // get state information
            State state = statusMsg.State;
            Console.WriteLine("	" + state);

            WatchListEntry wle = watchList.Get(msg.StreamId);
            if (wle != null)
            {
                /* update our state table with the new state */
                if (!statusMsg.CheckPrivateStream()) /* non-private stream */
                {
                    /*
                     * check if this response should be on private stream but is not
                     */
                    if (!statusMsg.CheckPrivateStream()) /* non-private stream */
                    {
                        /*
                         * check if this response should be on private stream but is not
                         * batch responses for private stream may be sent on non-private
                         * stream
                         */
                        /* if this is the case, close the stream */
                        if (wle.IsPrivateStream && !wle.ItemName.Contains("BATCH_"))
                        {
                            Console.WriteLine($"Received non-private response for stream {msg.StreamId} that should be private - closing stream");
                            // close stream
                            CloseStream(redirectChnl, msg.StreamId, out error);
                            // remove private stream entry from list
                            RemoveMarketPriceItemEntry(msg.StreamId);
                            return CodecReturnCode.FAILURE;
                        }
                    }
                }
                wle.ItemState.DataState(statusMsg.State.DataState());
                wle.ItemState.StreamState(statusMsg.State.StreamState());
            }

            // redirect to private stream if indicated
            if (statusMsg.State.StreamState() == StreamStates.REDIRECTED
                    && (statusMsg.CheckPrivateStream()))
            {
                CodecReturnCode ret = RedirectToPrivateStream(msg.StreamId, out error);
                if (ret != CodecReturnCode.SUCCESS)
                {
                    return ret;
                }
            }

            return CodecReturnCode.SUCCESS;
        }

        protected CodecReturnCode HandleUpdate(Msg msg, DecodeIterator dIter, DataDictionary dictionary)
        {
           IUpdateMsg updateMsg = msg;
            PostUserInfo pu = updateMsg.PostUserInfo;
            if (pu != null)
            {
                Console.WriteLine($" Received UpdateMsg for stream {updateMsg.StreamId} from publisher with user ID: {pu.UserId} at user address: " + pu.UserAddrToString(pu.UserAddr));
            }
            return Decode(msg, dIter, dictionary);
        }

        protected CodecReturnCode Decode(Msg msg, DecodeIterator dIter, DataDictionary dictionary)
        {
            CodecReturnCode ret = fieldList.Decode(dIter, null);
            if (ret != CodecReturnCode.SUCCESS)
            {
                Console.WriteLine($"DecodeFieldList() failed with return code: {ret}");
                return ret;
            }

            StringBuilder fieldValue = new StringBuilder();
            GetItemName(msg, fieldValue);
            if (msg.MsgClass == MsgClasses.REFRESH)
                fieldValue.Append((((IRefreshMsg)msg).State).ToString() + "\n");

            // decode each field entry in list
            while ((ret = fieldEntry.Decode(dIter)) != CodecReturnCode.END_OF_CONTAINER)
            {
                if (ret != CodecReturnCode.SUCCESS)
                {
                    Console.WriteLine($"DecodeFieldEntry() failed with return code: {ret}");
                    return ret;
                }

                ret = DecodeFieldEntry(fieldEntry, dIter, dictionary, fieldValue);
                if (ret != CodecReturnCode.SUCCESS)
                {
                    Console.WriteLine("DecodeFieldEntry() failed");
                    return ret;
                }
                fieldValue.Append("\n");
            }
            EtaLogger.Instance.Information(fieldValue.ToString());

            return CodecReturnCode.SUCCESS;
        }

        protected CodecReturnCode HandleRefresh(Msg msg, DecodeIterator dIter, DataDictionary dictionary, out Error error)
        {
            IRefreshMsg refreshMsg = (IRefreshMsg)msg;
            PostUserInfo pu = refreshMsg.PostUserInfo;
            error = null;

            if (pu != null)
            {
                Console.WriteLine($"Received RefreshMsg for stream {refreshMsg.StreamId} from publisher with user ID: {pu.UserId} at user address: {pu.UserAddrToString(pu.UserAddr)}");
            }

            WatchListEntry wle = watchList.Get(msg.StreamId);

            /* check if this response should be on private stream but is not */
            /* if this is the case, close the stream */
            if (!refreshMsg.CheckPrivateStream() && wle.IsPrivateStream)
            {
                Console.WriteLine($"Received non-private response for stream {msg.StreamId} that should be private - closing stream");
                // close stream
                CloseStream(redirectChnl, msg.StreamId, out error);

                // remove private stream entry from list
                RemoveMarketPriceItemEntry(msg.StreamId);

                error = new Error(TransportReturnCode.FAILURE, $"Received non-private response for stream {msg.StreamId} that should be private - closing stream");
                return CodecReturnCode.FAILURE;
            }
            /*
             * update our item state list if its a refresh, then process just like
             * update
             */
            wle.ItemState.DataState(refreshMsg.State.DataState());
            wle.ItemState.StreamState(refreshMsg.State.StreamState());

            return this.Decode(msg, dIter, dictionary);
        }

        /*
         * This is used by all market price domain handlers to output field lists.
         * 
         * Decodes the field entry data and prints out the field entry data with
         * help of the dictionary. Returns success if decoding succeeds or failure
         * if decoding fails. fEntry - The field entry data dIter - The decode
         * iterator
         */
        protected CodecReturnCode DecodeFieldEntry(FieldEntry fEntry, DecodeIterator dIter,
                DataDictionary dictionary, StringBuilder fieldValue)
        {
            // get dictionary entry
            IDictionaryEntry dictionaryEntry = dictionary.Entry(fEntry.FieldId);

            // return if no entry found
            if (dictionaryEntry == null)
            {
                fieldValue.Append($"\tFid {fEntry.FieldId} not found in dictionary");
                return CodecReturnCode.SUCCESS;
            }

            // print out fid name
            fieldValue.Append($"\t{fEntry.FieldId}/{dictionaryEntry.Acronym}: ");

            // decode and print out fid value
            int dataType = dictionaryEntry.RwfType;
            CodecReturnCode ret = 0;
            switch (dataType)
            {
                case DataTypes.UINT:
                    ret = fidUIntValue.Decode(dIter);
                    if (ret == CodecReturnCode.SUCCESS)
                    {
                        fieldValue.Append(fidUIntValue.ToLong());
                    }
                    else if (ret != CodecReturnCode.BLANK_DATA)
                    {
                        Console.WriteLine($"DecodeUInt() failed: <{ret.GetAsString()}>");
                        return ret;
                    }
                    break;
                case DataTypes.INT:
                    ret = fidIntValue.Decode(dIter);
                    if (ret == CodecReturnCode.SUCCESS)
                    {
                        fieldValue.Append(fidIntValue.ToLong());
                    }
                    else if (ret != CodecReturnCode.BLANK_DATA)
                    {
                        Console.WriteLine($"DecodeInt() failed: <{ret.GetAsString()}>");

                        return ret;
                    }
                    break;
                case DataTypes.FLOAT:
                    ret = fidFloatValue.Decode(dIter);
                    if (ret == CodecReturnCode.SUCCESS)
                    {
                        fieldValue.Append(fidFloatValue.ToFloat());
                    }
                    else if (ret != CodecReturnCode.BLANK_DATA)
                    {
                        Console.WriteLine($"DecodeFloat() failed: <{ret.GetAsString()}>");

                        return ret;
                    }
                    break;
                case DataTypes.DOUBLE:
                    ret = fidDoubleValue.Decode(dIter);
                    if (ret == CodecReturnCode.SUCCESS)
                    {
                        fieldValue.Append(fidDoubleValue.ToDouble());
                    }
                    else if (ret != CodecReturnCode.BLANK_DATA)
                    {
                        Console.WriteLine($"DecodeDouble() failed: <{ret.GetAsString()}>");

                        return ret;
                    }
                    break;
                case DataTypes.REAL:
                    ret = fidRealValue.Decode(dIter);
                    if (ret == CodecReturnCode.SUCCESS)
                    {
                        fieldValue.Append(fidRealValue.ToString());
                    }
                    else if (ret != CodecReturnCode.BLANK_DATA)
                    {
                        Console.WriteLine($"DecodeReal() failed:  <{ret.GetAsString()}>");

                        return ret;
                    }
                    break;
                case DataTypes.ENUM:
                    ret = fidEnumValue.Decode(dIter);
                    if (ret == CodecReturnCode.SUCCESS)
                    {
                        IEnumType enumType = dictionary.EntryEnumType(dictionaryEntry,fidEnumValue);

                        if (enumType == null)
                        {
                            fieldValue.Append(fidEnumValue.ToInt());
                        }
                        else
                        {
                            fieldValue.Append(enumType.Display.ToString()).Append($"({fidEnumValue.ToInt()})");
                        }
                    }
                    else if (ret != CodecReturnCode.BLANK_DATA)
                    {
                        Console.WriteLine($"DecodeEnum() failed: <{ret.GetAsString()}>");

                        return ret;
                    }
                    break;
                case DataTypes.DATE:
                    ret = fidDateValue.Decode(dIter);
                    if (ret == CodecReturnCode.SUCCESS)
                    {
                        fieldValue.Append(fidDateValue.ToString());

                    }
                    else if (ret != CodecReturnCode.BLANK_DATA)
                    {
                        Console.WriteLine($"DecodeDate() failed: <{ret.GetAsString()}>");

                        return ret;
                    }
                    break;
                case DataTypes.TIME:
                    ret = fidTimeValue.Decode(dIter);
                    if (ret == CodecReturnCode.SUCCESS)
                    {
                        fieldValue.Append(fidTimeValue.ToString());
                    }
                    else if (ret != CodecReturnCode.BLANK_DATA)
                    {
                        Console.WriteLine($"DecodeTime() failed: <{ret.GetAsString()}>");

                        return ret;
                    }
                    break;
                case DataTypes.DATETIME:
                    ret = fidDateTimeValue.Decode(dIter);
                    if (ret == CodecReturnCode.SUCCESS)
                    {
                        fieldValue.Append(fidDateTimeValue.ToString());
                    }
                    else if (ret != CodecReturnCode.BLANK_DATA)
                    {
                        Console.WriteLine($"DecodeDateTime() failed: <{ret.GetAsString()}>");
                        return ret;
                    }
                    break;
                case DataTypes.QOS:
                    ret = fidQosValue.Decode(dIter);
                    if (ret == CodecReturnCode.SUCCESS)
                    {
                        fieldValue.Append(fidQosValue.ToString());
                    }
                    else if (ret != CodecReturnCode.BLANK_DATA)
                    {
                        Console.WriteLine($"DecodeQos() failed: <{ret.GetAsString()}>");

                        return ret;
                    }
                    break;
                case DataTypes.STATE:
                    ret = fidStateValue.Decode(dIter);
                    if (ret == CodecReturnCode.SUCCESS)
                    {
                        fieldValue.Append(fidStateValue.ToString());
                    }
                    else if (ret != CodecReturnCode.BLANK_DATA)
                    {
                        Console.WriteLine($"DecodeState() failed: <{ret.GetAsString()}>");

                        return ret;
                    }
                    break;
                case DataTypes.ARRAY:
                    break;
                case DataTypes.BUFFER:
                case DataTypes.ASCII_STRING:
                case DataTypes.UTF8_STRING:
                case DataTypes.RMTES_STRING:
                    if (fEntry.EncodedData.Length > 0)
                    {
                        fieldValue.Append(fEntry.EncodedData.ToString());
                    }
                    else
                    {
                        ret = CodecReturnCode.BLANK_DATA;
                    }
                    break;
                default:
                    fieldValue.Append($"Unsupported data type ({ret.GetAsString()})");
                    break;
            }
            if (ret == CodecReturnCode.BLANK_DATA)
            {
                fieldValue.Append("<blank data>");
            }

            return CodecReturnCode.SUCCESS;
        }


        /**
         * Close all item streams.
         * 
         * @param chnl - The channel to send a item stream close to
         */
        public CodecReturnCode CloseStreams(IChannel chnl, out Error error)
        {
            CodecReturnCode ret = 0;
            error = null;
            List<int> removeList = new List<int>();

            foreach (KeyValuePair<StreamIdKey, WatchListEntry> entry in watchList)
            {
                if (entry.Value.ItemState.IsFinal())
                    continue;
                if (entry.Value.DomainType == domainType)
                {
                    ret = CloseStream(chnl, entry.Key.StreamId, out error);
                    if (ret != CodecReturnCode.SUCCESS)
                    {
                        return ret;
                    }

                    removeList.Add(entry.Key.StreamId);
                }
            }

            foreach (int streamId in removeList)
                watchList.Remove(streamId);

            return CodecReturnCode.SUCCESS;
        }

        /*
         * Redirect a request to a private stream. streamId - The stream id to be
         * redirected to private stream
         */
        private CodecReturnCode RedirectToPrivateStream(int streamId, out Error error)
        {
            WatchListEntry wle = watchList.Get(streamId);

            /* remove non-private stream entry from list */
            RemoveMarketPriceItemEntry(streamId);

            /* add item name to private stream list */
            int psStreamId = watchList.Add(domainType, wle.ItemName, true);

            GenerateRequest(marketPriceRequest, true, redirectSrcId);
            marketPriceRequest.ItemNames.Add(wle.ItemName);
            marketPriceRequest.StreamId = psStreamId;
            return EncodeAndSendRequest(redirectChnl, marketPriceRequest, 0, out error);
        }
    }

}
