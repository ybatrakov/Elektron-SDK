/*|-----------------------------------------------------------------------------
 *|            This source code is provided under the Apache 2.0 license      --
 *|  and is provided AS IS with no warranty or guarantee of fit for purpose.  --
 *|                See the project's LICENSE.md for details.                  --
 *|           Copyright Thomson Reuters 2018. All rights reserved.            --
 *|-----------------------------------------------------------------------------
 */

#include "rtr/rtrdefs.h"
#include "rtr/gettime.h"

#ifdef SOLARIS2
#define RTR_USE_GETHRTIME			/* Use the gethrtime() call */

#elif defined(LINUX)
#if defined(x86_Linux_CLOCK_MONOTONIC)
#define RTR_USE_CLOCK_HIGHRES	/* Use the clock_gettime() system call */
#else
#define RTR_USE_RDTSC				/* Use the cpu clock ticks */
#endif

#elif defined(_WIN32)
#define RTR_USE_GET_PERF_COUNTER	/* Use the QueryPerformanceCounter */

#else
#define RTR_USE_GETTIMEOFDAY		/* Use the getimeofday call */

#endif





	/* Use the get high resolution time function call.
	 * This call uses the high resolution timer which
	 * is the non-adjustable high-res clock for the system.
	 * It is not in any way correlated to the time of day.
	 */
#ifdef RTR_USE_GETHRTIME
#include <sys/time.h>
rtr_time_value rtr_gettime_nano()
{
	return(gethrtime());
}

rtr_time_value rtr_gettime_micro()
{
	return(gethrtime()/1000L);
}

rtr_time_value rtr_gettime_milli()
{
	return(gethrtime()/1000000L);
}

double rtr_getticks_per_nano()	{ return (1.0); }
double rtr_getticks_per_micro()	{ return (1000.0); }
double rtr_getticks_per_milli()	{ return (1000000.0); }

rtr_time_value rtr_getticks()
{
	return (gethrtime());
}

#endif		/* RTR_USE_GETHRTIME */




	/* Use the cpu clock ticks to get the time.
	 * It is not in any way corrleted to the
	 * time of day.
	 */
#ifdef RTR_USE_RDTSC
#include <bits/time.h>
#include <stdio.h>
#include <stdlib.h>
#include "rtr/rtrmutx.h"
#include "rtr/rtrenv.h"

static RTR_STATIC_MUTEX_DECL(rtr_TimeInitMutex);
static int rtr_gettime_first=1;
static double rtr_FreqNanoS=0;
static double rtr_FreqMicroS=0;
static double rtr_FreqMilliS=0;

	/* This is more a question of hardware, but it is easier to check for the OS
	 * since we're not likely to be building / running AS 2.1 on modern hardware.
	 */
#ifdef x86_Linux_2X
inline long long int rtr_rdtsc()
{
	long long int x;
	__asm__ volatile (".byte 0x0f, 0x31" : "=A" (x));
	return x;
}
#else
inline long long int rtr_rdtsc()
{
	unsigned int lo, hi;
	unsigned long long int result;
#ifdef _LP64
#ifdef __pic__
		/* On UNIX -fPIC uses rbx. Need to save and then restore */
	__asm__ __volatile__ ("xorl %%eax,%%eax;push %%rbx;cpuid":::"%rax","rcx","rdx");
	__asm__ __volatile__ ("rdtsc;pop %%rbx" : "=a" (lo), "=d" (hi));
#else
	__asm__ __volatile__ ("xorl %%eax,%%eax;cpuid":::"%rax","rbx","rcx","rdx");
	__asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
#endif
#else
#ifdef __pic__
		/* On UNIX -fPIC uses ebx. Need to save and then restore */
	__asm__ __volatile__ ("xorl %%eax,%%eax;pushl %%ebx;cpuid":::"%eax","ecx","edx");
	__asm__ __volatile__ ("rdtsc;popl %%ebx" : "=a" (lo), "=d" (hi));
#else
	__asm__ __volatile__ ("xorl %%eax,%%eax;cpuid":::"%eax","ebx","ecx","edx");
	__asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
#endif
#endif
	result = (unsigned long long int) hi << 32 | lo;
	return (long long int)result;
}
#endif

static rtr_gettime_do_first()
{
	RTR_STATIC_MUTEX_LOCK(rtr_TimeInitMutex);
	if (rtr_gettime_first)
	{
		FILE		*cpuinfo;
		double		frequency=0.0;
		double		mhertz=0.0;
		char		buf[256];


			/* The only way on Linux to get the speed
			 * of the cpu is by reading it out of the
			 * cpuinfo file. This value is accurate since
			 * it is calculated by the kernel. Also
			 * have an environment variable override
			 * just in case.
			 */
		if (RTRGetEnv("RTR_CPU_CLOCK_MHZ",buf,255) != RTR_ENV_FAILURE)
		{
			mhertz = atof(buf);
			frequency = mhertz * 1000000.0;
		}
		else if ((cpuinfo = fopen("/proc/cpuinfo","r")) != 0)
		{
			while (fgets(buf,255,cpuinfo))
			{
				if (sscanf(buf,"cpu MHz	: %lf\n", &mhertz))
				{
					frequency = mhertz * 1000000.0;
					break;
				}
			}
			fclose(cpuinfo);
		}

			/* If you hit this assert the cpu speed could
			 * not be accessed. Might need to find another
			 * way of getting the cpu speed (bogomips??).
			 */
		RTRASSERT(frequency != 0.0);

			/* Calculate the number of clock ticks per microsecond. */
			/* Get the number of clock ticks per second */
			/* 2259531000 */
		rtr_FreqNanoS = frequency/1000000000.0;
		rtr_FreqMicroS = frequency/1000000.0;
		rtr_FreqMilliS = frequency/1000.0;

		rtr_gettime_first = 0;

		/*
		printf("MHz %lf  Frequency %lf\n",mhertz,frequency);
		*/
	}
	RTR_STATIC_MUTEX_UNLOCK(rtr_TimeInitMutex);
}

rtr_time_value rtr_gettime_nano()
{
	long long int	clocktick;
	double			ret;

	if (rtr_gettime_first)
		rtr_gettime_do_first();

	clocktick = rtr_rdtsc();
	ret = (double)clocktick / rtr_FreqNanoS;
	return( (rtr_time_value)ret );
}

rtr_time_value rtr_gettime_micro()
{
	long long int	clocktick;
	double			ret;

	if (rtr_gettime_first)
		rtr_gettime_do_first();

	clocktick = rtr_rdtsc();
	ret = (double)clocktick / rtr_FreqMicroS;
	return( (rtr_time_value)ret );
}

rtr_time_value rtr_gettime_milli()
{
	long long int	clocktick;
	double			ret;

	if (rtr_gettime_first)
		rtr_gettime_do_first();

	clocktick = rtr_rdtsc();
	ret = (double)clocktick / rtr_FreqMilliS;
	return( (rtr_time_value)ret );
}

double rtr_getticks_per_nano()
{
	if (rtr_gettime_first)
		rtr_gettime_do_first();

	return rtr_FreqNanoS;
}

double rtr_getticks_per_micro()
{
	if (rtr_gettime_first)
		rtr_gettime_do_first();

	return rtr_FreqMicroS;
}

double rtr_getticks_per_milli()
{
	if (rtr_gettime_first)
		rtr_gettime_do_first();

	return rtr_FreqMilliS;
}

rtr_time_value rtr_getticks()
{
	return (rtr_rdtsc());
}

#endif		/* RTR_USE_RDTSC */






	/* Use the clock_gettime to access the clock.
	 * Support to be implemented on Linux but doesn't
	 * work yet. It is not in any way corrleted to the
	 * time of day.
	 */
#ifdef RTR_USE_CLOCK_HIGHRES
#include <time.h>
#include <errno.h>
#include <sys/time.h>

rtr_time_value rtr_gettime_nano()
{
	rtr_time_value	ret;
	struct timespec	ts;

	clock_gettime(CLOCK_MONOTONIC,&ts); 
	ret = (rtr_time_value)ts.tv_sec * 1000000000L + (rtr_time_value)ts.tv_nsec;
	return(ret);
}

rtr_time_value rtr_gettime_micro()
{
	rtr_time_value	ret;
	struct timespec	ts;

	clock_gettime(CLOCK_MONOTONIC,&ts);
	ret = (rtr_time_value)ts.tv_sec * 1000000L + (rtr_time_value)ts.tv_nsec / 1000L;
	return(ret);
}

rtr_time_value rtr_gettime_milli()
{
	rtr_time_value	ret;
	struct timespec	ts;

	clock_gettime(CLOCK_MONOTONIC,&ts);
	ret = (rtr_time_value)ts.tv_sec * 1000L + (rtr_time_value)ts.tv_nsec / 1000000L;
	return(ret);
}

double rtr_getticks_per_nano()	{ return 1.0; }
double rtr_getticks_per_micro()	{ return 1000.0; }
double rtr_getticks_per_milli()	{ return 1000000.0; }

rtr_time_value rtr_getticks()
{
	return rtr_gettime_nano();
}

#endif		/* RTR_USE_CLOCK_HIGHRES */





	/* Use the normal gettimeofday call to get the time.
	 * This call is directly related to the time of day.
	 */
#ifdef RTR_USE_GETTIMEOFDAY
#include <sys/time.h>

rtr_time_value rtr_gettime_nano()
{
	rtr_time_value	ret;
	struct timeval	tb;

	gettimeofday(&tb, 0);
	ret = (rtr_time_value)tb.tv_sec * 1000000000L + (rtr_time_value)tb.tv_usec * 1000L;
	return(ret);
}

rtr_time_value rtr_gettime_micro()
{
	rtr_time_value	ret;
	struct timeval	tb;

	gettimeofday(&tb, 0);
	ret = (rtr_time_value)tb.tv_sec * 1000000L + (rtr_time_value)tb.tv_usec;
	return(ret);
}

rtr_time_value rtr_gettime_milli()
{
	rtr_time_value	ret;
	struct timeval	tb;

	gettimeofday(&tb, 0);
	ret = (rtr_time_value)tb.tv_sec * 1000L + (rtr_time_value)tb.tv_usec / 1000L;
	return(ret);
}

double rtr_getticks_per_nano()	{ return 1.0; }
double rtr_getticks_per_micro()	{ return 1000.0; }
double rtr_getticks_per_milli() { return 1000000.0; }

rtr_time_value rtr_getticks()
{
	return rtr_gettime_nano();
}

#endif		/* RTR_USE_GETTIMEOFDAY */





	/* Use the cpu clock tick and cpu frequence to
	 * calculate the time. It is not in any way correlated
	 * to the time of day.
	 */
#ifdef RTR_USE_GET_PERF_COUNTER
#include "rtr/rtrmutx.h"

static RTR_STATIC_MUTEX_DECL(rtr_TimeInitMutex);

static int rtr_gettime_first=1;
static double rtr_FreqNanoS=0;
static double rtr_FreqMicroS=0;
static double rtr_FreqMilliS=0;


/*
rtr_time_value rtr_gettime_RDTSC()
{
	_asm _emit 0x0F
	_asm _emit 0x31
}
*/

static rtr_gettime_do_first()
{
	RTR_STATIC_MUTEX_LOCK( rtr_TimeInitMutex );

	if (rtr_gettime_first)
	{
		LARGE_INTEGER	m_Freq;
			/* Get the number of clock ticks per second */
		RTRASSERT(QueryPerformanceFrequency(&m_Freq));

			/* Calculate the number of clock ticks per microsecond. */
		rtr_FreqNanoS = (double)m_Freq.QuadPart/1000000000.0;
		rtr_FreqMicroS = (double)m_Freq.QuadPart/1000000.0;
		rtr_FreqMilliS = (double)m_Freq.QuadPart/1000.0;

		rtr_gettime_first = 0;
	}
	RTR_STATIC_MUTEX_UNLOCK( rtr_TimeInitMutex );
}

rtr_time_value rtr_gettime_nano()
{
	LARGE_INTEGER	m_Counter;
	double			ret;

	if (rtr_gettime_first)
		rtr_gettime_do_first();

	RTRASSERT(QueryPerformanceCounter(&m_Counter));
	ret = (double)m_Counter.QuadPart / rtr_FreqNanoS;
	return( (rtr_time_value)ret );
}

rtr_time_value rtr_gettime_micro()
{
	LARGE_INTEGER	m_Counter;
	double			ret;

	if (rtr_gettime_first)
		rtr_gettime_do_first();

	RTRASSERT(QueryPerformanceCounter(&m_Counter));
	ret = (double)m_Counter.QuadPart / rtr_FreqMicroS;
	return( (rtr_time_value)ret );
}

rtr_time_value rtr_gettime_milli()
{
	LARGE_INTEGER	m_Counter;
	double			ret;

	if (rtr_gettime_first)
		rtr_gettime_do_first();

	RTRASSERT(QueryPerformanceCounter(&m_Counter));
	ret = (double)m_Counter.QuadPart / rtr_FreqMilliS;
	return( (rtr_time_value)ret );
}


double rtr_getticks_per_nano()
{
	if (rtr_gettime_first)
		rtr_gettime_do_first();

	return rtr_FreqNanoS;
}

double rtr_getticks_per_micro()
{
	if (rtr_gettime_first)
		rtr_gettime_do_first();

	return rtr_FreqMicroS;
}

double rtr_getticks_per_milli()
{
	if (rtr_gettime_first)
		rtr_gettime_do_first();

	return rtr_FreqMilliS;
}

rtr_time_value rtr_getticks()
{
	LARGE_INTEGER	m_Counter;
	RTRASSERT(QueryPerformanceCounter(&m_Counter));
	return( (rtr_time_value)m_Counter.QuadPart );
}

#endif		/* RTR_USE_GET_PERF_COUNTER */




rtr_time_value rtr_getreltime_nano()
{
	static int				nanofirst=1;
	static rtr_time_value	nanobasetime;

	rtr_time_value	curtime=rtr_gettime_nano();
	if (nanofirst)
	{
		nanofirst=0;
		nanobasetime=curtime;
	}
	return(curtime-nanobasetime);
}

rtr_time_value rtr_getreltime_micro()
{
	static int				microfirst=1;
	static rtr_time_value	microbasetime;

	rtr_time_value	curtime=rtr_gettime_micro();
	if (microfirst)
	{
		microfirst=0;
		microbasetime=curtime;
	}
	return(curtime-microbasetime);
}

rtr_time_value rtr_getreltime_milli()
{
	static int				millifirst=1;
	static rtr_time_value	millibasetime;

	rtr_time_value	curtime=rtr_gettime_milli();
	if (millifirst)
	{
		millifirst=0;
		millibasetime=curtime;
	}
	return(curtime-millibasetime);
}


