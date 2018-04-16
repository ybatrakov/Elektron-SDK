/*|-----------------------------------------------------------------------------
 *|            This source code is provided under the Apache 2.0 license      --
 *|  and is provided AS IS with no warranty or guarantee of fit for purpose.  --
 *|                See the project's LICENSE.md for details.                  --
 *|           Copyright Thomson Reuters 2018. All rights reserved.            --
 *|-----------------------------------------------------------------------------
 */

namespace ThomsonReuters.Eta.Net.Consumer
{
    class MarketPriceRequestFlags
    {
        private MarketPriceRequestFlags()
        {
            throw new System.NotImplementedException();
        }

        public const int NONE = 0;
        public const int HAS_QOS = 0x001;
        public const int HAS_PRIORITY = 0x002;
        public const int HAS_SERVICE_ID = 0x004;
        public const int HAS_WORST_QOS = 0x008;
        public const int HAS_VIEW = 0x010;
        public const int STREAMING = 0x020;
        public const int PRIVATE_STREAM = 0x040;
    }
}
