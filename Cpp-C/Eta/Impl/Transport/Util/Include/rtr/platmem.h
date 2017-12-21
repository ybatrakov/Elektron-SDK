/*|-----------------------------------------------------------------------------
 *|            This source code is provided under the Apache 2.0 license      --
 *|  and is provided AS IS with no warranty or guarantee of fit for purpose.  --
 *|                See the project's LICENSE.md for details.                  --
 *|           Copyright Thomson Reuters 2018. All rights reserved.            --
 *|-----------------------------------------------------------------------------
 */

#ifndef __platmem_h
#define __platmem_h

#include "rtr/platform.h"

#if defined(__cplusplus)
extern "C" {
#endif

#if 1
extern void* (*cutilPlatMemoryAlloc)(unsigned long size);
extern void (*cutilPlatMemoryDealloc)(void *memory);
extern void setcutilPlatMemory( void *(*)(unsigned long), void (*)(void*) );
extern void clearcutilPlatMemory();

#else

extern void* (*cutilPlatMemoryAlloc)();
extern void (*cutilPlatMemoryDealloc)();
extern void setcutilPlatMemory();
extern void clearcutilPlatMemory();

#endif


#define PLATFORM_MEMORY_ALLOC(size) (*cutilPlatMemoryAlloc)(size)
#define PLATFORM_MEMORY_DEALLOC(memory) (*cutilPlatMemoryDealloc)(memory)


#if defined(__cplusplus)
}
#endif


#endif
