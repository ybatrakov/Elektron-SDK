/*|-----------------------------------------------------------------------------
 *|            This source code is provided under the Apache 2.0 license      --
 *|  and is provided AS IS with no warranty or guarantee of fit for purpose.  --
 *|                See the project's LICENSE.md for details.                  --
 *|           Copyright Thomson Reuters 2018. All rights reserved.            --
 *|-----------------------------------------------------------------------------
 */

#ifndef __RTR_HASHFUNCS_H
#define __RTR_HASHFUNCS_H

#include "os.h"

#ifdef __cplusplus
extern "C" {
#endif



/* Used for hash sum definition */
#ifdef COMPILE_64BITS
typedef rtrUInt64 rtrHashSum;
#else
typedef rtrUInt32 rtrHashSum;
#endif


	/* CRC32 table for polynomial 0xF3C5F6A9 */
#ifdef WIN32
	/* To get around global variables exporting from DDL */
RTR_API_EXPORT rtrUInt32* __rtrGetHashCrcTable();
#else
extern rtrUInt32 __rtr_hash_crc_table[];
#endif

	/* In case you need a function pointers. There are inline versions
	 * below for speed. */
RTR_API_EXPORT rtrHashSum rtrPolyHashFunc(const char* buf, const rtrUInt32 length);
RTR_API_EXPORT rtrHashSum rtrRTICHashFunc(const char* buf, const rtrUInt32 length);
RTR_API_EXPORT rtrHashSum rtrMDHHashFunc(const char* buf, const rtrUInt32 length);
RTR_API_EXPORT rtrHashSum rtrSDBMHashFunc(const char* buf, const rtrUInt32 length);
RTR_API_EXPORT rtrHashSum rtrFNV1HashFunc(const char* buf, const rtrUInt32 length);
RTR_API_EXPORT rtrHashSum rtrFNV1aHashFunc(const char* buf, const rtrUInt32 length);
RTR_API_EXPORT rtrHashSum trHashingEntityId(const char *buf, const rtrUInt32 length, const rtrInt32 numberOfHashingEntities);

/* Calculate the hash value for a buffer/len by calculating the crc32
 * using the polynomial (0xF3C5F6A9). This is what is used in IDN
 * today and seems to give the best network traffic segmentation hashing
 */
RTR_C_INLINE rtrUInt32 rtrPolyHash(const char* buf, const rtrUInt32 length) 
{
	register rtrUInt32 crc = 0;
#ifdef WIN32
	register rtrUInt32* __rtr_hash_crc_table = __rtrGetHashCrcTable();
#endif

	register rtrUInt32 len=length;
	while (len-- > 0)
		crc = __rtr_hash_crc_table[(crc ^ *buf++) & 0xFF] ^ (crc >> 8);

	return crc;
}

/* Calculate the hash value for a buffer/len using the algorithm used
 * in the RTIC.
 */
RTR_C_INLINE rtrUInt32 rtrRTICHash(const char* buf, const rtrUInt32 length)
{
	register rtrUInt32 hash = 0;
	register rtrUInt32 len=length;
	int i = 0;

	while (len-- > 0)
	{
		hash ^= ((*buf++) << i);
		if ((i += 5) > 23)
			i -= 23;
	}
	return hash;
}


/* Calculate the hash value for a buffer/len using the algorithm used
 * in TRMDS.
 */
RTR_C_INLINE unsigned long rtrMDHHash(const char* buf, const rtrUInt32 length)
{
	register unsigned long hash = 0;
	register rtrUInt32 len=length;
	while (len-- > 0)
	{
		hash = (hash << 4) + (unsigned long)*buf++; 
		hash ^= (hash >> 12);
	}
	return hash;
}


/* Calculate the hash value for a buffer/len using the SDBM method.
 * Seems to be realy fast and gives a good distribution.
 */
RTR_C_INLINE rtrUInt32 rtrSDBMHash(const char* buf, const rtrUInt32 length)
{
	register rtrUInt32 hash = 0;
	register rtrUInt32 len = length;
	while (len-- > 0)
		hash = (*buf++) + (hash << 6) + (hash << 16) - hash;

	return hash;
}


/* Calculate the hash value for a buffer/len using the FNV1 method.
 * Seems to be the fastest and gives a good distribution.
 */
RTR_C_INLINE rtrHashSum rtrFNV1Hash(const char* buf, const rtrUInt32 length)
{
#ifdef COMPILE_64BITS
	const rtrHashSum fnv_prime = 1099511628211;
	register rtrHashSum hash = RTR_ULL(14695981039346656037);
#else
	const rtrHashSum fnv_prime = 16777619;
	register rtrHashSum hash = RTR_ULL(2166136261);
#endif

	register rtrUInt32 len = length;
	while (len-- > 0)
		hash = (hash * fnv_prime) ^ *buf++;  /* FNV-1 */
	
	return hash;
}

/* Calculate the hash value for a buffer/len using the FNV1a method.
 * Seems to be the fastest and gives a good distribution, somewhat better than FNV1
 */
RTR_C_INLINE rtrHashSum rtrFNV1aHash(const char* buf, const rtrUInt32 length)
{
#ifdef COMPILE_64BITS
	const rtrHashSum fnv_prime = 1099511628211;
	register rtrHashSum hash = RTR_ULL(14695981039346656037);
#else
	const rtrHashSum fnv_prime = 16777619;
	register rtrHashSum hash = RTR_ULL(2166136261);
#endif

	register rtrUInt32 len = length;
	while (len-- > 0)
		hash = (hash ^ *buf++) * fnv_prime;  /* FNV-1a */

	return hash;
}

/* Calculate the hash value for a buffer/len using the algorithm used
 * in RTRString.
 */
RTR_C_INLINE long rtrRTRStringHash(const char* buf, const rtrUInt32 length)
{
	register unsigned long hash = 0;
	unsigned long magic = 8388593;			// the greatest prime lower than 2^23

	register const char *s = buf;
	register rtrUInt32 len = length;
	while (len--)
		hash = ((hash % magic) << 8) + (unsigned long) *s++;
	return hash;
}


#ifdef __cplusplus
}
#endif

#endif

