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

namespace ThomsonReuters.Eta.Net.Consumer
{
    public class CodecErrorException : Exception
    {
        public CodecReturnCode CodecReturnCode { get; private set; }

        public CodecErrorException(CodecReturnCode codecReturnCode, string message) :
            base(message)
        {
            CodecReturnCode = codecReturnCode;
        }
        public CodecErrorException(CodecReturnCode codecReturnCode) 
        {
            CodecReturnCode = codecReturnCode;
        }

        public override string Message => $"CodecReturnCode: {CodecReturnCode} \n {base.Message}";
    }
}
