/*|-----------------------------------------------------------------------------
 *|            This source code is provided under the Apache 2.0 license      --
 *|  and is provided AS IS with no warranty or guarantee of fit for purpose.  --
 *|                See the project's LICENSE.md for details.                  --
 *|           Copyright Thomson Reuters 2018. All rights reserved.            --
 *|-----------------------------------------------------------------------------
 */

#ifndef __rtr_gettimeofday_h
#define __rtr_gettimeofday_h

#include <time.h>
#include <string.h>
#include "rtr/rtdataty.h"
#include "rtr/rtrdefs.h"

#ifndef _WIN32
#include <sys/time.h>
#endif

#if defined(__cplusplus)
extern "C" {
#endif

#ifdef _WIN32

struct rtr_timeval
{
	time_t tv_sec;
	long   tv_usec;
};
typedef struct rtr_timeval rtr_timeval_t;

void rtr_localtime(const time_t *tm, struct tm* tmstruct);
void rtr_ctime(const time_t* tm, char* buf);

#else

#define rtr_localtime localtime_r
#define rtr_ctime ctime_r
typedef struct timeval rtr_timeval_t;

#endif

int rtr_gettimeofday(rtr_timeval_t *tv);
void setTimestampUsecOffset(const char* format, u_8 *uSecOffset);

#if defined(__cplusplus)
}
#endif

#endif
