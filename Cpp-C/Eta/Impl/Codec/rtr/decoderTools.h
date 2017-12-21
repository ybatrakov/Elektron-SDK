/*|-----------------------------------------------------------------------------
 *|            This source code is provided under the Apache 2.0 license      --
 *|  and is provided AS IS with no warranty or guarantee of fit for purpose.  --
 *|                See the project's LICENSE.md for details.                  --
 *|           Copyright Thomson Reuters 2018. All rights reserved.            --
 *|-----------------------------------------------------------------------------
 */

#ifndef __RSSL_DECODER_TOOLS_H
#define __RSSL_DECODER_TOOLS_H

#include "rtr/rwfNetwork.h"
#include "rtr/intDataTypes.h"
#include "rtr/custmem.h"


#ifdef __cplusplus
extern "C" {
#endif

	
//ETAC_OPEN_SOURCE_DOUBLE_CHECK
///* This function does not perform any buffer size checking.
// * Can use the _rsslValidBufferPtr(), _rsslInvalidBufferPointer(),
// * _rsslValidBufferPointerEndPtr() or _rsslInvalidBufferPointerEndPtr()
// * after to check since no data copy is happening.
// */
//RTR_C_ALWAYS_INLINE char * _rsslDecodeBuffer8(RsslBuffer *pBuf,char *position )
//{
//	RsslUInt8 len;
//
//	position += rwfGet8(len, position);
//	pBuf->length = len;
//
//	pBuf->data = (pBuf->length > 0) ? (char*) position : 0;
//	position += pBuf->length;
//	return position;
//}

/* This function does not perform any buffer size checking.
 * Can use the _rsslValidBufferPtr(), _rsslInvalidBufferPointer(),
 * _rsslValidBufferPointerEndPtr() or _rsslInvalidBufferPointerEndPtr()
 * after to check since no data copy is happening.
 */
RTR_C_ALWAYS_INLINE char * _rsslDecodeBuffer16(RsslBuffer * buffer, char * position)
{
	RsslUInt16 tlen;

	position += rwfGetOptByteU16(&tlen, position);
	buffer->length = tlen;
	buffer->data = (buffer->length > 0) ? (char *) position : 0;
	position += buffer->length;
	return position;
}

//ETAC_OPEN_SOURCE_DOUBLE_CHECK
///* This function does not perform any buffer size checking.
// * Can use the _rsslValidBufferPtr(), _rsslInvalidBufferPointer(),
// * _rsslValidBufferPointerEndPtr() or _rsslInvalidBufferPointerEndPtr()
// * after to check since no data copy is happening.
// */
//RTR_C_ALWAYS_INLINE char * _rsslDecodeBuffer32(RsslBuffer * buffer, char * position)
//{
//	position += RWF_GET_OPTBYTE_U32(&buffer->length, position);
//	buffer->data = (buffer->length > 0) ? (char *) position : 0;
//	position += buffer->length;
//	return position;
//}

/* This function does not perform any buffer size checking.
 * Can use the _rsslValidBufferPtr(), _rsslInvalidBufferPointer(),
 * _rsslValidBufferPointerEndPtr() or _rsslInvalidBufferPointerEndPtr()
 * after to check since no data copy is happening.
 */
RTR_C_ALWAYS_INLINE char * _rsslDecodeBuffer15(RsslBuffer * buffer, char * position)
{
	RsslUInt16 tlen;

	position += rwfGetResBitU15(&tlen, position);
	buffer->length = tlen;
	buffer->data = (buffer->length > 0) ? (char *) position : 0;
	position += buffer->length;
	return position;
}


#ifdef __cplusplus
}
#endif 

#endif
