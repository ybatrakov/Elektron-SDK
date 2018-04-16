/*|-----------------------------------------------------------------------------
 *|            This source code is provided under the Apache 2.0 license      --
 *|  and is provided AS IS with no warranty or guarantee of fit for purpose.  --
 *|                See the project's LICENSE.md for details.                  --
 *|           Copyright Thomson Reuters 2018. All rights reserved.            --
 *|-----------------------------------------------------------------------------
 */

using ThomsonReuters.Eta.Transports;

namespace ThomsonReuters.Eta.Internal.Interfaces
{
   internal interface IRipcBuffer
   {
      TransportReturnCode Pack(out Error error);
   }
}
