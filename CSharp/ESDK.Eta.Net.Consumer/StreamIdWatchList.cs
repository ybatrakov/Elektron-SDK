/*|-----------------------------------------------------------------------------
 *|            This source code is provided under the Apache 2.0 license      --
 *|  and is provided AS IS with no warranty or guarantee of fit for purpose.  --
 *|                See the project's LICENSE.md for details.                  --
 *|           Copyright Thomson Reuters 2018. All rights reserved.            --
 *|-----------------------------------------------------------------------------
 */

using System;
using System.Collections;
using System.Collections.Generic;
using System.Threading;
using ThomsonReuters.Eta.Codec;


namespace ThomsonReuters.Eta.Net.Consumer
{
    public class StreamIdWatchList : IEnumerable<KeyValuePair<StreamIdKey, WatchListEntry>>
    {
        private Dictionary<StreamIdKey, WatchListEntry> watchList = new Dictionary<StreamIdKey, WatchListEntry>();

        private int nextStreamId = 4;// stream id for start of market price streams

        // 1 - login, 2- directory, 3-field dictionary, 4 - enum dictionary

        public StreamIdWatchList()
        {
        }

        public bool IsEmpty()
        {
            return watchList.Count == 0;
        }

        public int NoOfItems()
        {
            return watchList.Count;
        }

        public int Add(int domainType, String itemName, bool isPrivateStream)
        {
            WatchListEntry wle = new WatchListEntry
            {
                IsPrivateStream = isPrivateStream,
                ItemState = new State()
            };
            wle.ItemState.DataState(DataStates.NO_CHANGE);
            wle.ItemState.StreamState(StreamStates.UNSPECIFIED);
            wle.ItemName = itemName;
            wle.DomainType = domainType;
            int thisStreamId = Interlocked.Increment(ref nextStreamId);
            StreamIdKey key = new StreamIdKey
            {
                StreamId = thisStreamId
            };
            watchList.Add(key, wle);
            return thisStreamId;
        }

        public WatchListEntry Get(int streamId)
        {
            StreamIdKey key = new StreamIdKey
            {
                StreamId = streamId
            };
            return watchList[key];
        }

        public void RemoveAll()
        {
            watchList.Clear();
            nextStreamId = 4;
        }

        public bool Remove(int streamId)
        {
            StreamIdKey key = new StreamIdKey
            {
                StreamId = streamId
            };
            return watchList.ContainsKey(key) && watchList.Remove(key) == true;
        }

        public int GetFirstItem(ThomsonReuters.Eta.Codec.Buffer mpItemName)
        {
            foreach (KeyValuePair<StreamIdKey, WatchListEntry> entry in watchList)
            {
                WatchListEntry wle = entry.Value;
                State itemState = wle.ItemState;
                if (itemState.DataState() == DataStates.OK
                        && itemState.StreamState() == StreamStates.OPEN)
                {
                    mpItemName.Data(wle.ItemName);
                    return entry.Key.StreamId;
                }
            }

            /* no suitable items were found */
            mpItemName.Clear();

            return 0;
        }

        public void Clear()
        {
            watchList.Clear();
            nextStreamId = 4;
        }

        public IEnumerator<KeyValuePair<StreamIdKey, WatchListEntry>> GetEnumerator()
        {
            return watchList.GetEnumerator();
        }

        IEnumerator IEnumerable.GetEnumerator()
        {
            return GetEnumerator();
        }
    }

    public class WatchListEntry
    {
        public bool IsPrivateStream
        {
            get;set;
        }

        public String ItemName
        {
            get;set;
        }
        
        public State ItemState
        {
            get;set;
        }

        public int DomainType
        {
            get;set;
        }

        public override String ToString()
        {
            return $"IsPrivateStream: {IsPrivateStream}, ItemName: {ItemName}, ItemState: {ItemState}" +
                $", DomainType: {Rdm.DomainTypes.ToString(DomainType)}\n";
        }
    }

    /*
     * Stream id key used by StreamIdWatchList. 
     */
    public class StreamIdKey : IEquatable<StreamIdKey>
    {
        public override String ToString()
        {
            return $"StreamId: {StreamId}\n";
        }

        public override int GetHashCode()
        {
            return StreamId;
        }
        public override bool Equals(Object obj)
        {
            return (obj is StreamIdKey) ? Equals((StreamIdKey)obj) : false;
        }

        public bool Equals(StreamIdKey other)
        {
            return (other.StreamId == StreamId);
        }

        public void Clear()
        {
            StreamId = 0;
        }

        public int StreamId
        {
            get;set;
        }
    }
}
