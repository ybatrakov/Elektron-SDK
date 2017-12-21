/*|-----------------------------------------------------------------------------
 *|            This source code is provided under the Apache 2.0 license      --
 *|  and is provided AS IS with no warranty or guarantee of fit for purpose.  --
 *|                See the project's LICENSE.md for details.                  --
 *|           Copyright Thomson Reuters 2018. All rights reserved.            --
 *|-----------------------------------------------------------------------------
 */

#ifdef _WIN32
#include <Windows.h>
#include <sys/timeb.h> // _ftime_s
#endif

#include "rtr/gettimeofday.h"

#ifdef  _WIN32

struct Win32HiResBaseTime
{
	LARGE_INTEGER PerfCountFreq;
	LARGE_INTEGER PerfCountBaseLine;
	struct _timeb tb_BaseLine;
	RTRBOOL usePerfCounter;
	RTRBOOL initDone;
};

static struct Win32HiResBaseTime gbl_HRBaseTime = {{0,0},{0,0},{0,0,0,0}, RTRTRUE, RTRFALSE};

static void syncHiResSysTime(rtr_timeval_t *tv)
{
	if (gbl_HRBaseTime.usePerfCounter && gbl_HRBaseTime.PerfCountFreq.QuadPart == 0)
		gbl_HRBaseTime.usePerfCounter = QueryPerformanceFrequency(&gbl_HRBaseTime.PerfCountFreq);

	if (gbl_HRBaseTime.usePerfCounter)
		QueryPerformanceCounter(&gbl_HRBaseTime.PerfCountBaseLine);

	_ftime_s(&gbl_HRBaseTime.tb_BaseLine);

	tv->tv_sec  = gbl_HRBaseTime.tb_BaseLine.time;
	tv->tv_usec = gbl_HRBaseTime.tb_BaseLine.millitm*1000;
}

int rtr_gettimeofday(rtr_timeval_t *tv)
{
	if (!gbl_HRBaseTime.initDone)
	{
		syncHiResSysTime(tv);
		gbl_HRBaseTime.initDone = RTRTRUE;
		return 0;
	}

	if (gbl_HRBaseTime.usePerfCounter)
	{
		LARGE_INTEGER PerfCount;
		if (!QueryPerformanceCounter(&PerfCount))
		{
			// This shouldn't ever be hit, since we already did
			// successful QueryPerformanceFrequency on init.
			syncHiResSysTime(tv);
			return 0;
		}
		else
		{
			struct _timeb tb;
			time_t tsecs;
			double dsecs;
			long long int clockUsecs;
			long long int perfUsecs;

			// get system clock time, for checking against drift
			_ftime_s(&tb);

			dsecs = (double) (PerfCount.QuadPart - gbl_HRBaseTime.PerfCountBaseLine.QuadPart)/ (double) gbl_HRBaseTime.PerfCountFreq.QuadPart;
			tsecs = (time_t) dsecs;

			tv->tv_sec  = gbl_HRBaseTime.tb_BaseLine.time + tsecs;
			tv->tv_usec = gbl_HRBaseTime.tb_BaseLine.millitm * 1000 + (int) ((dsecs - tsecs) * 1000000.0);

			if (tv->tv_usec > 1000000)
			{
				tv->tv_usec -= 1000000;
				tv->tv_sec++;
			}

			clockUsecs = tb.time    * 1000000 + tb.millitm * 1000;
			perfUsecs  = tv->tv_sec * 1000000 + tv->tv_usec;

			if (clockUsecs > perfUsecs + 15000 || clockUsecs < perfUsecs - 15000)
			{
				// Reset baseline:
				// already did back-to-back QueryPerfCounter and ftime,
				// so just set these as new baseline.
				gbl_HRBaseTime.tb_BaseLine = tb;
				gbl_HRBaseTime.PerfCountBaseLine = PerfCount;

				tv->tv_sec  = gbl_HRBaseTime.tb_BaseLine.time;
				tv->tv_usec = gbl_HRBaseTime.tb_BaseLine.millitm*1000;
			}
		}
	}
	else // Performance counters not working
	{
		struct _timeb tb;
		_ftime_s(&tb);

		tv->tv_sec  = tb.time;
		tv->tv_usec = tb.millitm*1000;
	}

	return 0;
}

// Thread-safe if using multi-thread crt library
void rtr_localtime(const time_t *tm, struct tm* tmstruct)
{
	localtime_s(tmstruct, tm);
}

// Thread-safe if using multi-thread crt library
// Following Linux convention of assuming the programmer
// knows the buffer is large enough (26 chars).
void rtr_ctime(const time_t* tm, char* buf)
{
	ctime_s(buf, 26, tm);
}


#else //not _WIN32

int rtr_gettimeofday(rtr_timeval_t *tv)
{
	// According to the man page, use of the timezone structure (the second arg) is obsolete,
	// but will be filled if provided.
	return gettimeofday(tv, 0);
}

#endif


void setTimestampUsecOffset(const char* format, u_8 *uSecOffset)
{
	int i;
	int fmtlen = strlen(format);

	// Some formats may lead to variable-length time strings, which we can't use.
	// If the offset can't be determined it is set to zero.
	RTRBOOL bPossVarLenFmt = RTRFALSE;
	for (i = 0; i < fmtlen; i++)
	{
		if (format[i] == '%')
		{
			switch(format[++i])
			{
			case 'A':
			case 'B':

			case 'E':
			case 'O':
			case 'c':
			case 'X':
			case '-': bPossVarLenFmt=RTRTRUE;	break;
			}
		}
	}

	if (bPossVarLenFmt)
		*uSecOffset = 0;
	else
	{
		struct tm stm;
		enum { bufsz =256};
		char buf[bufsz];
		int i;

		// If the second isn't found below , is set to zero to indicate no printing of microseconds
		*uSecOffset=0;

		// Win32 and Solaris do not include tm_gmtoff or tm_zone members.
		// Since those should be zero anyway,
		// just start by zeroing out the struct, whatever size it is.
		memset(&stm, 0, sizeof(stm));

		// set all to determined date 1/1/1970 00:00:02
		stm.tm_sec   = 02;
//		stm.tm_min   = 00;
//		stm.tm_hour  = 00;
		stm.tm_mday  =  1;
//		stm.tm_mon   =  0;
		stm.tm_year  = 70;
		stm.tm_wday  =  4;  // 1/1/1970 was a Thursday
//		stm.tm_yday  =  0;	// 0 days since 1/1
//		stm.tm_isdst =  0;
//		stm.tm_gmtoff=  0;  // linux only
//		stm.tm_zone  =  0;  // linux only

		strftime(buf, bufsz, format, &stm);

		for (i = 0; i < bufsz; i++)
		{
			if (buf[i] == '2')
			{
				*uSecOffset = ++i;
				break;
			}
		}
	}
}

