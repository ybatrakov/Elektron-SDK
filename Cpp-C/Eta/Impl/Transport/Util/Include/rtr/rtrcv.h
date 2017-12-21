/*|-----------------------------------------------------------------------------
 *|            This source code is provided under the Apache 2.0 license      --
 *|  and is provided AS IS with no warranty or guarantee of fit for purpose.  --
 *|                See the project's LICENSE.md for details.                  --
 *|           Copyright Thomson Reuters 2018. All rights reserved.            --
 *|-----------------------------------------------------------------------------
 */

//  description:
//	Condition Variable - Private Condition Variable
//
//	This header file defines the macros used to define private condition
//	variables. A private condition variable can only exist in one process.
//	It cannot cross process boundaries.
//	This condition variable is basic and only implements the blocking
//	condition variable.
//
//	Stopping:
//	When exiting a thread that has a waiting condition variable the way
//	to get out of the condition variable depends on the OS. This can
//  also be used to for a blocking wait() on a condition variable to
//	return RTRFALSE.
//		DEV_POS_THR and DEV_SOL_THR - Send a signal to the thread will cause cv to return.
//		_WIN32 - Have to RTR_CV_SET_FLAG() to set a flag event handle.
//					Then when want condition variable wait to return, SetEvent on
//					the event passed into RTR_CV_SET_FLAG().
//					Example: Thread that wait()s on cv.
//
//					HANDLE notifyEvent;
//					notifyEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
//					RTR_CV_SET_FLAG(condvar,notifyEvent);
//					// When thread wants to flag.
//					PulseEvent(notifyEvent);
//						// This would cause RTR_CV_WAIT() to return RTRFALSE.
//
//	Expected Use:
//
//	RTRBOOL value=RTRFALSE;
//	rtr_cond_var	cv;
//
//	main()
//	{
//		rtr_cv_error	cv_error;
//
//		if (RTR_CV_INIT(cv,cv_error))
//		{
//			printf("Condition Variable Initialized\n");
//		}
//		else
//		{
//			printf("Could not initialize cv error %d\n",RTR_CV_ERROR(cv_error));
//			exit(1);
//		}
//
//		Create and start otherThread
//
//		if (!RTR_CV_LOCK(cv,cv_error))
//		{
//			printf("Could not lock cv error %d\n",RTR_CV_ERROR(cv_error));
//			exit(1);
//		}
//
//		while (!value)
//		{
//			if (RTR_CV_WAIT(cv,cv_error))
//				break;
//		}
//
//		if (!RTR_CV_UNLOCK(cv,cv_error))
//		{
//			printf("Could not unlock cv error %d\n",RTR_CV_ERROR(cv_error));
//			exit(1);
//		}
//
//		if (!RTR_CV_DESTROY(cv,cv_error))
//			printf("Could no destroy cv error %d\n",RTR_CV_ERROR(cv_error));
//	}
//
//	otherThread()
//	{
//		rtr_cv_error	cv_error;
//
//		sleep(5);
//		if (!RTR_CV_LOCK(cv,cv_error))
//		{
//			printf("Could not lock cv error %d\n",RTR_CV_ERROR(cv_error));
//			exit(1);
//		}
//		value = RTRTRUE;
//		if (!RTR_CV_UNLOCK(cv,cv_error))
//		{
//			printf("Could not unlock cv error %d\n",RTR_CV_ERROR(cv_error));
//			exit(1);
//		}
//	}


#ifndef __rtrcv_h
#define __rtrcv_h

#include "rtr/rtrmutx.h"

#if defined(Linux) 
//
// As the time.h for Linux has no declaration of timestruc_t and
// MILLISEC, it is defined here for Linux only
//
#include <sys/time.h>
#ifndef _TIMESTRUC_T
#define _TIMESTRUC_T
#define MILLISEC 1000
typedef struct timespec timestruc_t;
#endif  /* _TIMESTRUC_T */
#endif  /* Linux */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef DEV_POS_THR
/***************************************************************************/
/* Posix threads definitions */

#include <pthread.h>
#include <errno.h>

#define RTRCVWAITTIMEOUT ETIME

typedef struct
{
	rtr_mutex		mutex;
	pthread_cond_t	condVar;
	pthread_condattr_t	attrVar;
} rtr_cond_var;

typedef rtr_mutex_error rtr_cv_error;

extern RTRBOOL rtr_cond_var_init(rtr_cond_var*,rtr_cv_error*);
extern RTRBOOL rtr_cond_var_destroy(rtr_cond_var*,rtr_cv_error*);
extern RTRBOOL rtr_cond_var_timed_wait(rtr_cond_var*,long seconds,long milliseconds,rtr_cv_error*);
extern RTRBOOL rtr_cond_var_timed_waitabs(rtr_cond_var*,long seconds,long milliseconds,rtr_cv_error*);

#define RTR_CV_ERROR(error)	error
#define RTR_CV_INIT(condv,error) rtr_cond_var_init(&condv,&error)
#define RTR_CV_DESTROY(condv,error) rtr_cond_var_destroy(&condv,&error)

#define RTR_CV_WAIT(condv,error) \
	(((error = pthread_cond_wait(&condv.condVar,&condv.mutex)) == 0) ? RTRTRUE : RTRFALSE)
#define RTR_CV_TIMED_WAIT(condv,seconds,millisec,error) \
	rtr_cond_var_timed_wait(&condv,seconds,millisec,&error)
#define RTR_CV_TIMED_WAITABS(condv,seconds,millisec,error) \
	rtr_cond_var_timed_waitabs(&condv,seconds,millisec,&error)
#define RTR_CV_SIGNAL(condv,error) \
	(((error = pthread_cond_signal(&condv.condVar)) == 0) ? RTRTRUE : RTRFALSE)
#define RTR_CV_SIGNALALL(condv,error) \
	(((error = pthread_cond_broadcast(&condv.condVar)) == 0) ? RTRTRUE : RTRFALSE)

#define RTR_CV_LOCK(condv,error) RTR_MUTEX_LOCK(condv.mutex,error)
#define RTR_CV_UNLOCK(condv,error) RTR_MUTEX_UNLOCK(condv.mutex,error)
#define RTR_CV_TRYLOCK(condv,error) RTR_MUTEX_TRYLOCK(condv.mutex,error)
#define RTR_CV_BUSY(error) RTR_MUTEX_BUSY(error)

/* End posix threads definitions */
/***************************************************************************/
#endif



#if defined(DEV_SOL_THR)
/***************************************************************************/
/* Solaris threads definitions */

#include <thread.h>
#include <errno.h>
#include <synch.h>

#define RTRCVWAITTIMEOUT ETIME

typedef struct
{
	rtr_mutex	mutex;
	cond_t		condVar;
} rtr_cond_var;

typedef rtr_mutex_error rtr_cv_error;

extern RTRBOOL rtr_cond_var_init(rtr_cond_var*,rtr_cv_error*);
extern RTRBOOL rtr_cond_var_destroy(rtr_cond_var*,rtr_cv_error*);
extern RTRBOOL rtr_cond_var_timed_wait(rtr_cond_var*,long seconds,long milliseconds,rtr_cv_error*);
extern RTRBOOL rtr_cond_var_timed_waitabs(rtr_cond_var*,long seconds,long milliseconds,rtr_cv_error*);

#define RTR_CV_ERROR(error)	error
#define RTR_CV_INIT(condv,error) rtr_cond_var_init(&condv,&error)
#define RTR_CV_DESTROY(condv,error) rtr_cond_var_destroy(&condv,&error)

#define RTR_CV_WAIT(condv,error) \
	(((error = cond_wait(&condv.condVar,&condv.mutex)) == 0) ? RTRTRUE : RTRFALSE)
#define RTR_CV_TIMED_WAIT(condv,seconds,millisec,error) \
	rtr_cond_var_timed_wait(&condv,seconds,millisec,&error)
#define RTR_CV_TIMED_WAITABS(condv,seconds,millisec,error) \
	rtr_cond_var_timed_waitabs(&condv,seconds,millisec,&error)
#define RTR_CV_SIGNAL(condv,error) \
	(((error = cond_signal(&condv.condVar)) == 0) ? RTRTRUE : RTRFALSE)
#define RTR_CV_SIGNALALL(condv,error) \
	(((error = cond_broadcast(&condv.condVar)) == 0) ? RTRTRUE : RTRFALSE)

#define RTR_CV_LOCK(condv,error) RTR_MUTEX_LOCK(condv.mutex,error)
#define RTR_CV_UNLOCK(condv,error) RTR_MUTEX_UNLOCK(condv.mutex,error)
#define RTR_CV_TRYLOCK(condv,error) RTR_MUTEX_TRYLOCK(condv.mutex,error)
#define RTR_CV_BUSY(error) RTR_MUTEX_BUSY(error)

/* End solaris threads definitions */
/***************************************************************************/
#endif



#ifdef _WIN32
/***************************************************************************/
/* win32 thread definitions */

#include <windows.h>

enum rtrCondVarHandles { RTR_NOTIFY=0, RTR_FLAG=1, RTR_MAX_HANDLES=2 };

#define RTRCVWAITTIMEOUT WAIT_TIMEOUT

typedef struct
{
	rtr_mutex	mutex;
	HANDLE		handles[RTR_MAX_HANDLES];
	DWORD		handleCount;
} rtr_cond_var;

typedef rtr_mutex_error rtr_cv_error;

extern RTRBOOL rtr_cond_var_init(rtr_cond_var*,rtr_cv_error*);
extern RTRBOOL rtr_cond_var_destroy(rtr_cond_var*,rtr_cv_error*);
extern RTRBOOL rtr_cond_var_wait(rtr_cond_var*,rtr_cv_error*);
extern RTRBOOL rtr_cond_var_timed_wait(rtr_cond_var*,long seconds,long milliseconds,rtr_cv_error*);
extern RTRBOOL rtr_cond_var_timed_waitabs(rtr_cond_var*,long seconds,long milliseconds,rtr_cv_error*);
extern RTRBOOL rtr_cond_var_signal(rtr_cond_var*,rtr_cv_error*);
extern RTRBOOL rtr_cond_var_set_flag(rtr_cond_var*,HANDLE);

#define RTR_CV_ERROR(error)	GetLastError()
#define RTR_CV_INIT(condv,error) rtr_cond_var_init(&condv,&error)
#define RTR_CV_DESTROY(condv,error) rtr_cond_var_destroy(&condv,&error)
#define RTR_CV_WAIT(condv,error) rtr_cond_var_wait(&condv,&error)
#define RTR_CV_TIMED_WAIT(condv,seconds,millisec,error) \
	rtr_cond_var_timed_wait(&condv,seconds,millisec,&error)
#define RTR_CV_TIMED_WAITABS(condv,seconds,millisec,error) \
	rtr_cond_var_timed_waitabs(&condv,seconds,millisec,&error)
#define RTR_CV_SIGNAL(condv,error) rtr_cond_var_signal(&condv,&error)
#define RTR_CV_SIGNALALL(condv,error) rtr_cond_var_signal(&condv,&error)
#define RTR_CV_LOCK(condv,error) RTR_MUTEX_LOCK(condv.mutex,error)
#define RTR_CV_UNLOCK(condv,error) RTR_MUTEX_UNLOCK(condv.mutex,error)
#define RTR_CV_TRYLOCK(condv,error) RTR_MUTEX_TRYLOCK(condv.mutex,error)
#define RTR_CV_BUSY(error) RTR_MUTEX_BUSY(error)

#define RTR_CV_SET_FLAG(condv,handle) rtr_cond_var_set_flag(&condv,handle)

/* End win32 threads definitions */
/***************************************************************************/
#endif


#ifndef RTR_CV_INIT
/***************************************************************************/
/* Dummy condition variable definitions */
#define RTR_DUMMY_CV_DEFS

typedef int rtr_cond_var;
typedef rtr_mutex_error rtr_cv_error;

#define RTR_CV_ERROR(error)	error
#define RTR_CV_INIT(condv,error) RTRTRUE
#define RTR_CV_DESTROY(condv,error) RTRTRUE

#define RTR_CV_WAIT(condv,error) RTRTRUE
#define RTR_CV_TIMED_WAIT(condv,seconds,millisec,error) RTRTRUE
#define RTR_CV_TIMED_WAITABS(condv,seconds,millisec,error) RTRTRUE
#define RTR_CV_SIGNAL(condv,error) RTRTRUE
#define RTR_CV_SIGNALALL(condv,error) RTRTRUE

#define RTR_CV_LOCK(condv,error) RTRTRUE
#define RTR_CV_UNLOCK(condv,error) RTRTRUE
#define RTR_CV_TRYLOCK(condv,error) RTRTRUE
#define RTR_CV_BUSY(error) RTRFALSE

/* End dummy condition variable definitions */
/***************************************************************************/
#endif


#ifdef __cplusplus
}
#endif

#endif
