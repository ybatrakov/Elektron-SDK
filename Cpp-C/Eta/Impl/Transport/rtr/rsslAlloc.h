/*|-----------------------------------------------------------------------------
 *|            This source code is provided under the Apache 2.0 license      --
 *|  and is provided AS IS with no warranty or guarantee of fit for purpose.  --
 *|                See the project's LICENSE.md for details.                  --
 *|           Copyright Thomson Reuters 2018. All rights reserved.            --
 *|-----------------------------------------------------------------------------
 */

#ifndef __rsslalloc_h
#define __rsslalloc_h


#include "rtr/os.h"

#include <sys/types.h>
#include <stdlib.h>
#include "rtr/platmem.h"

#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

RTR_C_ALWAYS_INLINE void* _rsslMalloc(size_t size)
{
	return(PLATFORM_MEMORY_ALLOC(size));
}

RTR_C_ALWAYS_INLINE void* _rsslRealloc(char *buf, size_t origSize, size_t newSize)
{
	void *newBuf=PLATFORM_MEMORY_ALLOC(newSize);
	memcpy(newBuf, buf, origSize);
	PLATFORM_MEMORY_DEALLOC(buf);
	return (newBuf);
}

RTR_C_ALWAYS_INLINE void _rsslFree(void *mem)
{
	PLATFORM_MEMORY_DEALLOC(mem);
}


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
