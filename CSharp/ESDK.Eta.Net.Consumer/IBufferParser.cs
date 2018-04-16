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
using ThomsonReuters.Eta.Transports.Interfaces;
using ThomsonReuters.Eta.Transports;

namespace ThomsonReuters.Eta.Net.Consumer
{
    public interface IBufferParser
    {
        bool Parse(IChannel channel, ITransportBuffer buffer);
    }
}
