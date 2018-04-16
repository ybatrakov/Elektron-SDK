/*|-----------------------------------------------------------------------------
 *|            This source code is provided under the Apache 2.0 license      --
 *|  and is provided AS IS with no warranty or guarantee of fit for purpose.  --
 *|                See the project's LICENSE.md for details.                  --
 *|           Copyright Thomson Reuters 2018. All rights reserved.            --
 *|-----------------------------------------------------------------------------
 */

using System;
using System.Collections.Generic;
using ThomsonReuters.Common.Logger;
using ThomsonReuters.Eta.Codec;
using ThomsonReuters.Eta.Rdm;
using ThomsonReuters.Eta.Transports;
using ThomsonReuters.Eta.Transports.Interfaces;

namespace ThomsonReuters.Eta.Net.Consumer
{
    public class DecodingBufferParser : IBufferParser
    {
        DecodeIterator _iterator;

        Msg _msg;

        Lazy<DataDictionary> _dictionary = new Lazy<DataDictionary>(() => 
        {
            var dictionary = new DataDictionary();

            CodecReturnCode codecReturnCode = dictionary.LoadFieldDictionary("RDMFieldDictionary", 
                                                                        out CodecError codecError);
            if (codecReturnCode != CodecReturnCode.SUCCESS)
                throw new CodecErrorException(codecReturnCode, codecError.Text);

            codecReturnCode = dictionary.LoadEnumTypeDictionary("enumtype.def", out codecError);
            if (codecReturnCode != CodecReturnCode.SUCCESS)
                throw new CodecErrorException(codecReturnCode, codecError.Text);

            return dictionary;
        });

        StreamIdWatchList _streamIdWatchList = new StreamIdWatchList();

        public MarketPriceHandler _marketPriceHandler;

        bool _isItemRequetSent = false;

        int _serviceId;

        public DecodingBufferParser(int serviceId)
        {
            _iterator = new DecodeIterator();

            _msg = new Msg();

            _marketPriceHandler = new MarketPriceHandler(_streamIdWatchList);

            _serviceId = serviceId;
        }

        public bool Parse(IChannel channel, ITransportBuffer buffer)
        {
            _iterator.Clear();
            _msg.Clear();

            CodecReturnCode codecReturnCode = _iterator.SetBufferAndRWFVersion(buffer, Codec.Codec.MajorVersion(), Codec.Codec.MinorVersion());
            if (codecReturnCode < CodecReturnCode.SUCCESS)
                throw new CodecErrorException(codecReturnCode);

            codecReturnCode = _msg.Decode(_iterator);
            if (codecReturnCode < CodecReturnCode.SUCCESS)
                throw new CodecErrorException(codecReturnCode);

            if (!_msg.ValidateMsg())
                throw new CodecErrorException(CodecReturnCode.INVALID_DATA, $"{buffer}");

            EtaLogger.Instance.Trace($"Msg DomainType: {(DomainType)_msg.DomainType} Data: {buffer.Data}");

            Error error;

            // Dispatch to Handler
            switch ((DomainType)_msg.DomainType) 
            {
                case DomainType.LOGIN:

                    switch (_msg.MsgClass)
                    {
                        case MsgClasses.REFRESH:
                            {
                                IRefreshMsg refreshMsg = _msg;

                                if ( refreshMsg.State.StreamState() == StreamStates.OPEN &&
                                    refreshMsg.State.DataState() == DataStates.OK)
                                {
                                    if (refreshMsg.CheckHasMsgKey() && refreshMsg.MsgKey.CheckHasName())
                                    {
                                        EtaLogger.Instance.Information($"Successfully connection to the server for \'{refreshMsg.MsgKey.Name}\' with text: {refreshMsg.State.Text()}");
                                    }
                                    else
                                    {
                                        EtaLogger.Instance.Information($"Successfully connection to the server with text: {refreshMsg.State.Text()}");
                                    }
                                }
                                else /* handle error cases */
                                {
                                    if (refreshMsg.State.StreamState() == StreamStates.CLOSED_RECOVER)
                                    {
                                        EtaLogger.Instance.Information($"Login stream is closed recover with text: {refreshMsg.State.Text()}");
                                        return false;
                                    }
                                    else if (refreshMsg.State.StreamState() == StreamStates.CLOSED)
                                    {
                                        EtaLogger.Instance.Information($"Login attempt failed (stream closed) with text: {refreshMsg.State.Text()}");
                                        return false;
                                    }
                                    else if (refreshMsg.State.StreamState() == StreamStates.OPEN && refreshMsg.State.DataState() == DataStates.OK)
                                    {
                                        EtaLogger.Instance.Information($"Login stream is suspect with text: {refreshMsg.State.Text()}");
                                        return false;
                                    }
                                }

                                break;
                            }

                        case MsgClasses.STATUS:
                        {
                                IStatusMsg statusMsg = _msg;

                                if (statusMsg.State.StreamState() == StreamStates.OPEN &&
                                    statusMsg.State.DataState() == DataStates.OK)
                                {
                                    if (statusMsg.CheckHasMsgKey() && statusMsg.MsgKey.CheckHasName())
                                    {
                                        EtaLogger.Instance.Information($"Successfully connection to the server for \'{statusMsg.MsgKey.Name}\'");
                                    }
                                    else
                                    {
                                        EtaLogger.Instance.Information($"Successfully connection to the server with text: {statusMsg.State.Text()}");
                                    }
                                }
                                else /* handle error cases */
                                {
                                    if (statusMsg.State.StreamState() == StreamStates.CLOSED_RECOVER)
                                    {
                                        EtaLogger.Instance.Information($"Login stream is closed recover with text: {statusMsg.State.Text()}");
                                        return false;
                                    }
                                    else if (statusMsg.State.StreamState() == StreamStates.CLOSED)
                                    {
                                        EtaLogger.Instance.Information($"Login attempt failed (stream closed) with text: {statusMsg.State.Text()}");
                                        return false;
                                    }
                                    else if (statusMsg.State.StreamState() == StreamStates.OPEN && statusMsg.State.DataState() == DataStates.OK)
                                    {
                                        EtaLogger.Instance.Information($"Login stream is suspect with text: {statusMsg.State.Text()}");
                                        return false;
                                    }
                                }

                                break;
                        }
                    }

                    string[] itemList = EtaConfiguration.ItemList.Split(',');

                    List<String> itemNames = new List<String>();

                    foreach(string itemName in itemList)
                    {
                        itemNames.Add(itemName);
                    }

                    if (!_isItemRequetSent)
                    {
                        _marketPriceHandler.SendItemRequests(channel, itemNames, false, _serviceId, out error);
                        _isItemRequetSent = true;
                    }

                    break;

                case DomainType.MARKET_PRICE:

                    _marketPriceHandler.ProcessResponse(_msg, _iterator, _dictionary.Value, out error);

                    break;

                default:
                    EtaLogger.Instance.Trace($"Unsupported Msg DomainType: {(DomainType)_msg.DomainType}");
                    return false;
            }

            return true;
        }
    }
}
