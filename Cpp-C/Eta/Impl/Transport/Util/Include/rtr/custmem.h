/*|-----------------------------------------------------------------------------
 *|            This source code is provided under the Apache 2.0 license      --
 *|  and is provided AS IS with no warranty or guarantee of fit for purpose.  --
 *|                See the project's LICENSE.md for details.                  --
 *|           Copyright Thomson Reuters 2018. All rights reserved.            --
 *|-----------------------------------------------------------------------------
 */

#ifndef	__rtr_custmem_h
#define	__rtr_custmem_h

#include <stdlib.h>
#include <string.h>
#include "intcustmem.h"


/*****************************************/
/* MemCopyByByte() simple while loop */
#define MEM_CHG 42

RTR_C_ALWAYS_INLINE void *MemCopyByByte(void *dest, const void *src, size_t size)
{
#if defined(SparcVersion) && SparcVersion == 10
	return( (size <= 5) ? doSimpleMemCopy(dest,src,size) :
			memcpy(dest, src, size) );
#elif defined(COMPILE_64BITS) && defined(SparcVersion) && SparcVersion == 10
	return( (size <= 3) ? doSimpleMemCopy(dest,src,size) :
			memcpy(dest, src, size) );
#elif defined(Solx86Version) && Solx86Version >= 10
	return( (size <= 9) ? doSimpleMemCopy(dest,src,size) :
			memcpy(dest, src, size) );
#elif defined(COMPILE_64BITS) && defined(Solx86Version) && Solx86Version >= 10
	return( (size <= 2) ? doSimpleMemCopy(dest,src,size) :
			memcpy(dest, src, size) );
#elif defined(LinuxVersion) && LinuxVersion <= 4
	return( (size <= MEM_CHG) ? doSimpleMemCopy(dest,src,size) :
			memcpy(dest, src, size) );
#else
	return memcpy(dest, src, size);
#endif
}
/*****************************************/


/*****************************************/
/* MemCopyByInt() works on quad boundaries */
RTR_C_ALWAYS_INLINE void *MemCopyByInt(void *dest, const void *src, size_t size)
{
#if defined(SparcVersion) && SparcVersion == 10
	return ( (size < 8) ? doSimpleMemCopy(dest,src,size) :
			 ((size < 40) ? doNonSimpleMemCopy4(dest,src,size) :
			 	memcpy(dest, src, size) )  );
#elif defined(COMPILE_64BITS) && defined(SparcVersion) && SparcVersion == 10
	return( (size <= 3) ? doSimpleMemCopy(dest,src,size) :
			memcpy(dest, src, size) );
#elif defined(Solx86Version) && Solx86Version >= 10
	return ( (size < 10) ? doSimpleMemCopy(dest,src,size) :
			 ((size < 38) ? doNonSimpleMemCopy4(dest,src,size) :
			 	memcpy(dest, src, size) )  );
#elif defined(COMPILE_64BITS) && defined(Solx86Version) && Solx86Version >= 10
	return( (size <= 2) ? doSimpleMemCopy(dest,src,size) :
			memcpy(dest, src, size) );
#elif defined(LinuxVersion) && LinuxVersion <= 4
	return ( (size < 8) ? doSimpleMemCopy(dest,src,size) :
			 ((size <= 128) ? doNonSimpleMemCopy4(dest,src,size) :
			 	memcpy(dest, src, size) )  );
#else
	return memcpy(dest, src, size);		// The memcpy with Linux 5 and 6 is much faster, windows memcpy is faster too
#endif
}
/*****************************************/

/*****************************************/
/* MemCopyLongLong() works on quad boundaries, but copies long long 8 bytes */
RTR_C_ALWAYS_INLINE void *MemCopyByLongLong(void *dest, const void *src, size_t size)
{
#if defined(SparcVersion) && SparcVersion == 10
	return ( (size < 2) ? doSimpleMemCopy(dest,src,size) :
			 ((size <= 71) ? doNonSimpleMemCopy8(dest,src,size) :
			 	memcpy(dest, src, size) )  );
#elif defined(COMPILE_64BITS) && defined(SparcVersion) && SparcVersion == 10
	return ( (size < 10) ? doSimpleMemCopy(dest,src,size) :
			 ((size < 120) ? doNonSimpleMemCopy8(dest,src,size) :
			 	memcpy(dest, src, size) )  );
#elif defined(Solx86Version) && Solx86Version >= 10
	return ( (size < 10) ? doSimpleMemCopy(dest,src,size) :
			 (((((unsigned long) dest & 0x07) == 0x00) && (size <= 32)) ?
				 doNonSimpleMemCopy8(dest,src,size) : memcpy(dest, src, size) )  );
#elif defined(COMPILE_64BITS) && defined(Solx86Version) && Solx86Version >= 10
	return ( (size < 2) ? doSimpleMemCopy(dest,src,size) :
				memcpy(dest, src, size) );
#elif defined(LinuxVersion) && LinuxVersion <= 4
	return ( (size < 8) ? doSimpleMemCopy(dest,src,size) :
			 ((size <= 128) ? doNonSimpleMemCopy8(dest,src,size) :
			 	memcpy(dest, src, size) )  );
#else
	return memcpy(dest, src, size);
#endif
}
/*****************************************/




/*****************************************/
/* MemSetByByte() simple while loop */
RTR_C_ALWAYS_INLINE void *MemSetByByte(void *dest, int c, size_t size)
{
#if defined(SparcVersion) && SparcVersion == 10
	return ( (size < 11) ? doSimpleMemSet(dest, c, size) :
				memset(dest, c, size) );
#elif defined(COMPILE_64BITS) && defined(SparcVersion) && SparcVersion == 10
	return ( (size < 8) ? doSimpleMemSet(dest, c, size) :
				memset(dest, c, size) );
#elif defined(Solx86Version) && Solx86Version >= 10
	return ( (size < 8) ? doSimpleMemSet(dest, c, size) :
				memset(dest, c, size) );
#elif defined(COMPILE_64BITS) && defined(Solx86Version) && Solx86Version >= 10
	return ( (size < 7) ? doSimpleMemSet(dest, c, size) :
				memset(dest, c, size) );
#else
	return ( (size <= MEM_CHG) ? doSimpleMemSet(dest, c, size) :
				memset(dest, c, size) );
#endif
}
/*****************************************/

/*****************************************/
/* MemSetByInt() works on quad boundaries and copies int 4 bytes */
RTR_C_ALWAYS_INLINE void *MemSetByInt(void *dest, int c, size_t size)
{
#if defined(SparcVersion) && SparcVersion == 10
	return ( (size < 8) ? doSimpleMemSet(dest,c,size) :
			 ((size <= 48) ? doNonSimpleMemSet4(dest,c,size) :
			 	memset(dest, c, size) )  );
#elif defined(COMPILE_64BITS) && defined(SparcVersion) && SparcVersion == 10
	return ( (size < 60) ? doNonSimpleMemSet4(dest, c, size) :
				memset(dest, c, size) );
#elif defined(Solx86Version) && Solx86Version >= 10
	return ( (size < 40) ? doNonSimpleMemSet4(dest, c, size) :
				memset(dest, c, size) );
#elif defined(COMPILE_64BITS) && defined(Solx86Version) && Solx86Version >= 10
	return ( (size < 38) ? doNonSimpleMemSet4(dest, c, size) :
				memset(dest, c, size) );
#else
	return ( (size < 8) ? doSimpleMemSet(dest,c,size) :
			 ((size <= 128) ? doNonSimpleMemSet4(dest,c,size) :
			 	memset(dest, c, size) )  );
#endif
}
/*****************************************/

/*****************************************/
/* MemSetLongLong() works on quad boundaries, but copies long long 8 bytes */
RTR_C_ALWAYS_INLINE void *MemSetByLongLong(void *dest, int c, size_t size)
{
#if defined(SparcVersion) && SparcVersion == 10
	return ( (size < 8) ? doSimpleMemSet(dest,c,size) :
			 ((size < 112) ? doNonSimpleMemSet8(dest,c,size) :
			 	memset(dest, c, size) )  );
#elif defined(COMPILE_64BITS) && defined(SparcVersion) && SparcVersion == 10
	return ( (size < 10) ? doSimpleMemSet(dest,c,size) :
			 ((size < 120) ? doNonSimpleMemSet8(dest,c,size) :
			 	memset(dest, c, size) )  );
#elif defined(Solx86Version) && Solx86Version >= 10
	return ( (size < 9) ? doSimpleMemSet(dest,c,size) :
			 (((size < 21) || (size > 66)) ? memset(dest, c, size) :
			 doNonSimpleMemSet8(dest,c,size)) );
#elif defined(COMPILE_64BITS) && defined(Solx86Version) && Solx86Version >= 10
	return ( (size < 80) ? doNonSimpleMemSet8(dest,c,size) :
			 	memset(dest, c, size) );
#else
	return ( (size < 8) ? doSimpleMemSet(dest,c,size) :
			 ((size <= 128) ? doNonSimpleMemSet8(dest,c,size) :
			 	memset(dest, c, size) )  );
#endif
}
/*****************************************/



/*****************************************/
/* MemCmpByByte() simple while loop */
RTR_C_ALWAYS_INLINE int MemCmpByByte(void *dest, const void *src, size_t size)
{
#if defined(SparcVersion) && SparcVersion == 10
	return( memcmp(dest, src, size) );
#elif defined(COMPILE_64BITS) && defined(SparcVersion) && SparcVersion == 10
	return( (size < 4) ? doSimpleMemCmp(dest,src,size) :
			memcmp(dest, src, size) );
#elif defined(Solx86Version) && Solx86Version >= 10
	return( (size < 4) ? doSimpleMemCmp(dest,src,size) :
			memcmp(dest, src, size) );
#elif defined(COMPILE_64BITS) && defined(Solx86Version) && Solx86Version >= 10
	return( (size < 8) ? doSimpleMemCmp(dest,src,size) :
			memcmp(dest, src, size) );
#else
	return( (size <= MEM_CHG) ? doSimpleMemCmp(dest,src,size) :
			memcmp(dest, src, size) );
#endif
}
/*****************************************/


/*****************************************/
/* MemCmpByInt() works on quad boundaries */
RTR_C_ALWAYS_INLINE int MemCmpByInt(void *dest, const void *src, size_t size)
{
#if defined(SparcVersion) && SparcVersion == 10
	return ( (size < 9) ? memcmp(dest,src,size) :
			 ((size <= 128) ? doNonSimpleMemCmp4(dest,src,size) :
			 	memcmp(dest, src, size) )  );
#elif defined(COMPILE_64BITS) && defined(SparcVersion) && SparcVersion == 10
	return ( (size < 4) ? doSimpleMemCmp(dest,src,size) :
			 ((size <= 128) ? doNonSimpleMemCmp4(dest,src,size) :
			 	memcmp(dest, src, size) )  );
#elif defined(Solx86Version) && Solx86Version >= 10
	return ( (size < 8) ? doNonSimpleMemCmp4(dest,src,size) :
			 	memcmp(dest, src, size) );
#elif defined(COMPILE_64BITS) && defined(Solx86Version) && Solx86Version >= 10
	return( (size < 8) ? doSimpleMemCmp(dest,src,size) :
				memcmp(dest, src, size) );
#else
	return ( (size < 8) ? doSimpleMemCmp(dest,src,size) :
			 ((size <= 128) ? doNonSimpleMemCmp4(dest,src,size) :
			 	memcmp(dest, src, size) )  );
#endif
}
/*****************************************/

/*****************************************/
/* MemCmpLongLong() works on quad boundaries, but copies long long 8 bytes */
RTR_C_ALWAYS_INLINE int MemCmpByLongLong(void *dest, const void *src, size_t size)
{
#if defined(SparcVersion) && SparcVersion == 10
	return ( (size < 3) ? doNonSimpleMemCmp8(dest,src,size) :
			((size < 8) ? memcmp(dest,src,size) :
			((size < 128) ? doNonSimpleMemCmp8(dest,src,size) :
			 	memcmp(dest, src, size))) );
#elif defined(COMPILE_64BITS) && defined(SparcVersion) && SparcVersion == 10
	return ( (size < 4) ? doNonSimpleMemCmp8(dest,src,size) :
			((size < 8) ? memcmp(dest,src,size) :
			((size < 128) ? doNonSimpleMemCmp8(dest,src,size) :
			 	memcmp(dest, src, size))) );
#elif defined(Solx86Version) && Solx86Version >= 10
	return ( (size < 8) ? doNonSimpleMemCmp8(dest,src,size) :
			 	memcmp(dest, src, size) );
#elif defined(COMPILE_64BITS) && defined(Solx86Version) && Solx86Version >= 10
	return ( (size < 32) ? doNonSimpleMemCmp8(dest,src,size) :
			 	memcmp(dest, src, size) );
#else
	return ( (size < 8) ? doSimpleMemCmp(dest,src,size) :
			 ((size <= 128) ? doNonSimpleMemCmp8(dest,src,size) :
			 	memcmp(dest, src, size) )  );
#endif
}
/*****************************************/

#endif

