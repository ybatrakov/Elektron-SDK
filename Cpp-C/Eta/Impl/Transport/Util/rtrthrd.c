/*|-----------------------------------------------------------------------------
 *|            This source code is provided under the Apache 2.0 license      --
 *|  and is provided AS IS with no warranty or guarantee of fit for purpose.  --
 *|                See the project's LICENSE.md for details.                  --
 *|           Copyright Thomson Reuters 2018. All rights reserved.            --
 *|-----------------------------------------------------------------------------
 */
 
#include "rtr/rtrthrd.h"
#include "rtr/rtrmutx.h"

#if defined(DEV_SOL_THR) || defined(DEV_POS_THR)
#include <time.h>
#endif



#if defined(DEV_POS_THR)
int rtr_pthr_set_pri(
			rtr_thread_info* thr_info_ptr,rtr_thr_err *errorp,rtr_thr_pri pri)
{
	struct sched_param	param;
	param.sched_priority = pri;
	*errorp = pthread_attr_setschedparam(&(thr_info_ptr->thread_attr),&param);
	return ((*errorp == 0) ? RTRTRUE : RTRFALSE);
}

int rtr_pthr_get_pri(
			rtr_thread_info* thr_info_ptr,rtr_thr_err *errorp,rtr_thr_pri* prip)
{
	struct sched_param	param;
	*errorp = pthread_attr_getschedparam(&(thr_info_ptr->thread_attr),&param);
	*prip = param.sched_priority;
	return ((*errorp == 0) ? RTRTRUE : RTRFALSE);
}
#endif



#if defined(SOLARIS2)
#if defined(DEV_SOL_THR) || defined(DEV_POS_THR)

#define RT_NAME "RT"
#define TS_NAME "TS"
 
#include <sys/priocntl.h>
#include <sys/rtpriocntl.h>
#include <sys/tspriocntl.h>
#include <errno.h>

int rtr_sched_real_time(rtr_thr_err* error_ptr,int pri)
{
	pcinfo_t 	pcinfo;
	pcparms_t	pcparms;
	rtparms_t	rtparms;
	long 		ret;
	int			loop;

	(void)memset(&pcinfo,0,sizeof(pcinfo));

	/*	Name of Realtime scheduling class */
	strcpy(pcinfo.pc_clname, RT_NAME);
	
	/*	Get the class id corresponding to this class
		NOTE: GETCID returns the total no of classes configured
		It returns -1 if an error occurred/clname is invalid
	*/
	ret = priocntl(P_LWPID, PC_CLNULL, PC_GETCID, (char *)&pcinfo);

	if (ret < 0) 
		return RTRFALSE;

	/*	Set up class dependent parameters */
	pcparms.pc_cid 		= pcinfo.pc_cid;
	rtparms.rt_pri 		= pri;
	rtparms.rt_tqsecs 	= 0;
	rtparms.rt_tqnsecs 	= RT_NOCHANGE;

	memcpy(&pcparms.pc_clparms, &rtparms, sizeof(rtparms));

	loop = 0;
	while( (ret=priocntl(P_LWPID, P_MYID, PC_SETPARMS, (char *)&pcparms)) < 0)
	{
		if(errno == EAGAIN)
		{
			if(++loop > 100)
				return(RTRFALSE);

			RTR_THREAD_YIELD();
			continue;
		}
		return(RTRFALSE);
	}
	return(RTRTRUE);
}

int rtr_sched_system(rtr_thr_err* error_ptr,rtr_thr_pri pri)
{
	return(RTRFALSE);
}

int rtr_sched_timeshare(rtr_thr_err* error_ptr,int pri)
{
	tsinfo_t 	tsinfo;
	pcinfo_t 	pcinfo;
	pcparms_t	pcparms;
	tsparms_t	tsparms;
	long		ret;
	int			loop;

	/* Initialize so memory checker doesn't complain. */
	(void)memset(&pcinfo,0,sizeof(pcinfo));

	/* Name of Realtime scheduling class */
	strcpy(pcinfo.pc_clname, TS_NAME);
	
	/*	Get the class id corresponding to this class
		NOTE: GETCID returns the total no of classes configured
		It returns -1 if an error occurred/clname is invalid
	*/
	ret = priocntl(P_LWPID, PC_CLNULL, PC_GETCID, (char *)&pcinfo);
	if(ret < 0) 
		return(RTRFALSE);

	/*	NOTE: GETCLINFO returns the total no of classes configured
		It returns -1 if an error occurred/clname is invalid
	*/
	ret = priocntl(P_LWPID, PC_CLNULL, PC_GETCLINFO, (char *)&pcinfo);
	if(ret < 0) 
		return(RTRFALSE);

	memcpy(&tsinfo, pcinfo.pc_clinfo, sizeof(tsinfo));

	tsparms.ts_upri    = ((pri > tsinfo.ts_maxupri) ? tsinfo.ts_maxupri : pri);
	tsparms.ts_uprilim = tsinfo.ts_maxupri; 

	pcparms.pc_cid 	= pcinfo.pc_cid;
	memcpy(&pcparms.pc_clparms, &tsparms, sizeof(tsparms));

	loop =0;
	while( (ret=priocntl(P_LWPID, P_MYID, PC_SETPARMS, (char *)&pcparms)) < 0)
	{
		if(errno == EAGAIN)
		{
			if(++loop > 100)
				return(RTRFALSE);
			RTR_THREAD_YIELD();
			continue;
		}
		return(RTRFALSE);
	}
	return(RTRTRUE);
}

#endif
#endif


void rtr_time_yield(int millisec)
{
#if defined(DEV_SOL_THR) || defined(DEV_POS_THR)
	if (millisec)
	{
		struct timespec ts;
		ts.tv_sec = millisec/1000;
		ts.tv_nsec = (millisec%1000)*1000000;
		nanosleep(&ts, NULL);
	}
#endif
#ifdef _WIN32
	Sleep(millisec);
#endif
}

#ifdef _WIN32
int SetThreadName( DWORD dwThreadID, LPCSTR szThreadName)
{
   THREADNAME_INFO info;
   info.dwType = 0x1000;
   info.szName = szThreadName;
   info.dwThreadID = dwThreadID;
   info.dwFlags = 0;

   __try
   {
      RaiseException( 0x406D1388, 0, sizeof(info)/sizeof(DWORD), (DWORD*)&info );
   }
	__except(EXCEPTION_CONTINUE_EXECUTION)
	{
	}

	return RTRTRUE;
}
#endif

