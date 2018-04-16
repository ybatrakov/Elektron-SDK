/*|-----------------------------------------------------------------------------
 *|            This source code is provided under the Apache 2.0 license      --
 *|  and is provided AS IS with no warranty or guarantee of fit for purpose.  --
 *|                See the project's LICENSE.md for details.                  --
 *|           Copyright Thomson Reuters 2018. All rights reserved.            --
 *|-----------------------------------------------------------------------------
 */

namespace ThomsonReuters.Eta.Transports
{
    /// <summary>
    /// ETA Initialize Arguments used in the <see cref="Transport.Initialize(InitArgs, out Error)"/> call.
    /// 
    /// <seealso cref="Transport"/>
    /// </summary>
    public class InitArgs
    {
        /// <summary>
        /// If locking is <c>true</c>, the global locking will be used by <see cref="Transport"/>.
        /// </summary>
        /// <value>The global locking</value>
        public bool GlobalLocking;
    }
}
