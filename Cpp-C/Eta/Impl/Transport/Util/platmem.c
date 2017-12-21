/*|-----------------------------------------------------------------------------
 *|            This source code is provided under the Apache 2.0 license      --
 *|  and is provided AS IS with no warranty or guarantee of fit for purpose.  --
 *|                See the project's LICENSE.md for details.                  --
 *|           Copyright Thomson Reuters 2018. All rights reserved.            --
 *|-----------------------------------------------------------------------------
 */

#include "rtr/platlog.h"
#include "rtr/platmem.h"


	/* If we are not building kernel code, then
	 * default the memory allocations to malloc and free.
	 */
#ifndef _KERNEL
#include <stdio.h>
#include <stdlib.h>
#endif

void *cutilDfltMemoryAlloc(unsigned long size)
{
#ifndef _KERNEL
	return(malloc(size));
#else
	return(0);
#endif
}

void cutilDfltMemoryDealloc(void *mem)
{
#ifndef _KERNEL
	free(mem);
#endif
}




#if 1
void *(*cutilPlatMemoryAlloc)(unsigned long) = cutilDfltMemoryAlloc;
void (*cutilPlatMemoryDealloc)(void*) = cutilDfltMemoryDealloc;

void setcutilPlatMemory(void *(*alloc)(unsigned long), void (*dealloc)(void*))
{
	cutilPlatMemoryAlloc = alloc;
	cutilPlatMemoryDealloc = dealloc;
}

#else

void *(*cutilPlatMemoryAlloc)() = cutilDfltMemoryAlloc;
void (*cutilPlatMemoryDealloc)() = cutilDfltMemoryDealloc;

void setcutilPlatMemory(void* (*alloc)(), void (*dealloc)())
{
	cutilPlatMemoryAlloc = alloc;
	cutilPlatMemoryDealloc = dealloc;
}

#endif

void clearcutilPlatMemory()
{
	cutilPlatMemoryAlloc=cutilDfltMemoryAlloc;
	cutilPlatMemoryDealloc=cutilDfltMemoryDealloc;
}

