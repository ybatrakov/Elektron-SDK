/*|-----------------------------------------------------------------------------
 *|            This source code is provided under the Apache 2.0 license      --
 *|  and is provided AS IS with no warranty or guarantee of fit for purpose.  --
 *|                See the project's LICENSE.md for details.                  --
 *|           Copyright Thomson Reuters 2018. All rights reserved.            --
 *|-----------------------------------------------------------------------------
 */

using System;
using System.ServiceModel.Channels;
using ThomsonReuters.Eta.Common;

namespace ThomsonReuters.Eta.Transports
{
    internal class TransportBufferPool : Pool
    {
        private BufferManager _bufferManager;

        public TransportBufferPool(object owner):
           base(owner)
        {
        }

        public TransportReturnCode InitPool(int numTransportBuffer,long maxBufferPoolSize, int maxBufferSize, out Error error)
        {
            error = null;

            try
            {
                _bufferManager = BufferManager.CreateBufferManager(maxBufferPoolSize, maxBufferSize);

                for(int i = 0; i < numTransportBuffer; i++)
                {
                    Add(new TransportBuffer());
                }
            }
            catch(Exception exp)
            {
                error = new Error
                {
                    ErrorId = TransportReturnCode.FAILURE,
                    Text = exp.Message,
                    SysError = 0,
                };

                return TransportReturnCode.FAILURE;
            }

            return TransportReturnCode.SUCCESS;
        }

        public ITransportBuffer GetTransprotBuffer(int bufferSize, out Error error)
        {
            error = null;
            TransportBuffer transportBuffer = null;
            try
            {
                transportBuffer = (TransportBuffer)Poll();

                if (transportBuffer == null)
                {
                    Byte[] data = _bufferManager.TakeBuffer(bufferSize);
                    transportBuffer = new TransportBuffer(new ByteBuffer(data, true))
                    {
                        Pool = this
                    };
                }
                else
                {
                    Byte[] data = _bufferManager.TakeBuffer(bufferSize);
                    transportBuffer.SetData(new ByteBuffer(data, true));
                }
            }
            catch(Exception exp)
            {
                error = new Error
                {
                    ErrorId = TransportReturnCode.FAILURE,
                    Text = exp.Message,
                };

                return transportBuffer;
            }

            return transportBuffer;
        }

        public void ReturnTransportBuffer(ITransportBuffer buffer, out Error error)
        {
            error = null;
            TransportBuffer transportBuffer = (TransportBuffer)buffer;

            try
            {
                // Ensoure that the Data property is not null
                if (transportBuffer.Data != null)
                {
                    _bufferManager.ReturnBuffer(transportBuffer.Data.Contents);
                }

                transportBuffer.Clear();

                transportBuffer.ReturnToPool();
            }
            catch(Exception exp)
            {
                error = new Error
                {
                    ErrorId = TransportReturnCode.FAILURE,
                    Text = exp.Message,
                };
            }
        }
    }
}
