/*|-----------------------------------------------------------------------------
 *|            This source code is provided under the Apache 2.0 license      --
 *|  and is provided AS IS with no warranty or guarantee of fit for purpose.  --
 *|                See the project's LICENSE.md for details.                  --
 *|           Copyright Thomson Reuters 2018. All rights reserved.            --
 *|-----------------------------------------------------------------------------
 */

//  description:
//
//	typdefs:
//		rtr_thread_info	- Thread information (structure)
//						- thread_id member will always exist
//		rtr_stack_sz;	- Stack size
//		rtr_thr_err		- Thread error
//		rtr_thr_pri		- Thread priority
//
//	defines:
//		RTR_THREAD_START_FUNC(thr_strt_func,arg)
//			- Declaration of the thread start functions.
//			- thr_strt_func is the function name and
//			- arg is the arg to the function
//
//		RTR_INIT_THR_INFO(thr_info_ptr)
//			- Initialize thread information
//			- Call before RTR_THREAD_CREATE
//		RTR_DESTROY_THR_INFO(thr_info_ptr)
//			- Destroy thread information
//			- Call after RTR_THREAD_EXIT
//
//		// Attributes to be set before RTR_THREAD_CREATE
//		// Default UNBOUND, ATTACHED, RUNNING
//		//		Default WIN32 BOUND, ATTACHED, RUNNING
//		RTR_SET_THR_BOUND(thr_info_ptr)
//		RTR_SET_THR_DETACHED(thr_info_ptr)
//			- Not available for win32 threads
//		RTR_SET_THR_SUSPENDED(thr_info_ptr)
//			- Not available for posix threads
//		RTR_SET_STACK(thr_info_ptr,base,size)
//
//		RTR_THREAD_CREATE(thr_info_ptr,strt_func,arg,error_ptr)
//			- Create a thread. Returns RTRTRUE on success, RTRFALSE otherwise.
//			- thr_info_ptr - pointer to rtr_thread_info structure.
//				- Contains attributes for thread that are
//				- set before the thread is created.
//			- strt_func - thread start function name (thr_strt_func above).
//			- arg - arguments to pass to strt_func.
//			- error_ptr - rtr_thr_err value.
//
//		RTR_THR_ERROR(error)
//			- Return rtr_thr_err for last error_ptr on thread call.
//
//		RTR_THREAD_EXIT()
//			- Exit current thread
//
//		RTR_THREAD_YIELD()
//			- Yield current thread
//
//		RTR_THREAD_TIMEYIELD(millisec)
//			- Yield for a certain period of time.
//			- Go to sleep for x milliseconds.
//
//		RTR_THREAD_JOIN(thr_info_ptr,error_ptr)
//			- Block until rtr_thread_id thr_info_ptr exits.
//			- Return RTRTRUE on success, RTRFALSE otherwise.
//			- thr_info_ptr - pointer to rtr_thread_info structure.
//			- error_ptr - rtr_thr_err value.
//
//		RTR_THREAD_SUSPEND(thr_info_ptr,error_ptr)
//			- Suspend rtr_thread_id thr_info_ptr.
//			- Return RTRTRUE on success, RTRFALSE otherwise.
//			- thr_info_ptr - pointer to rtr_thread_info structure.
//			- error_ptr - rtr_thr_err value.
//			- Not available for posix threads
//
//		RTR_THREAD_CONTINUE(thr_info_ptr,error_ptr)
//			- Continue rtr_thread_id thr_info_ptr.
//			- Return RTRTRUE on success, RTRFALSE otherwise.
//			- thr_info_ptr - pointer to rtr_thread_info structure.
//			- error_ptr - rtr_thr_err value.
//			- Not available for posix threads
// 
//		// Priority
//		// These can only be used on unbound threads
//		RTR_SET_THREAD_PRI(thr_info_ptr,error_ptr,pri) \
//			- Not available for win32 threads
//		RTR_GET_THREAD_PRI(thr_info_ptr,error_ptr,pri_ptr) \
//			- Not available for win32 threads
//
//		// Scheduling Classes
//		// These can only be used on bound threads
//		RTR_SCHED_REAL_TIME(thr_info_ptr,error_ptr,pri)
//		RTR_SCHED_SYSTEM(thr_info_ptr,error_ptr,pri)
//		RTR_SCHED_TIMESHARE(thr_info_ptr,error_ptr,pri)
//
//		// Thread signals
//		RTR_THREAD_SIGBLOCK(setptr,osetptr,error_ptr) \
//			- Add signals in setptr to signal mask
//			- for this thread.
//			- Not available for win32 threads
//		RTR_THREAD_SIGUNBLOCK(setptr,osetptr,error_ptr) \
//			- Remove signals in setptr from signal mask
//			- for this thread.
//			- Not available for win32 threads
//		RTR_THREAD_SIGSETMASK(setptr,osetptr,error_ptr) \
//			- Set the signal mask for this thread to setptr.
//			- Not available for win32 threads
//		RTR_THREAD_KILL(thr_info_ptr,sig,error_ptr)
//			- Send a signal to a thread.
//			- Not available for win32 threads
//		RTR_THREAD_SIGWAIT(setptr,sigptr,error_ptr)
//			- Wait for a signal in setptr to be delivered.
//			- Signal that is delivered is stored in sigptr (ptr to int).
//			- Not available for win32 threads
//
//
//	Expected Use:
//
//	RTR_THREAD_START_FUNC(start_func,args)
//	{
//		start_func code
//	}
//
//	main()
//	{
//		rtr_thread_info	thr_info;
//		rtr_thr_err		error;
//
//		RTR_INIT_THR_INFO(&thr_info);
//		RTR_SET_THR_BOUND(&thr_info);
//
//		if (RTR_THREAD_CREATE(&thr_info,start_func,0,flags,0,&error))
//		{
//			printf("Thread id %d created\n",thr_info.thread_id);
//		}
//		else
//		{
//			error = RTR_THR_ERROR(error);
//			printf("Error Creating thread %d\n",error);
//			exit(1);
//		}
//		RTR_THREAD_EXIT();
//		RTR_DESTROY_THR_INFO(&thr_info);
//	}


#ifndef __rtrthrd_h
#define __rtrthrd_h

#include "rtr/rtrdefs.h"

#ifdef DEV_POS_THR
#include <pthread.h>
#endif

#if defined(Linux)
#include <sys/prctl.h>
#endif

#if defined(DEV_SOL_THR)
#include <thread.h>
#include <errno.h>
#endif

#include "rtr/rtrsig.h"

#ifdef _WIN32
#include <windows.h>
#include <process.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif


/* Possible definitions for thread_info */
#define RTR_THR_BOUND		0x01
#define RTR_THR_DETACHED	0x02
#define RTR_THR_SUSPENDED	0x04

extern void rtr_time_yield(int);


#ifdef DEV_POS_THR
/***************************************************************************/
/* Posix threads definitions */

typedef pthread_t	rtr_thread_id;
typedef size_t		rtr_stack_sz;
typedef int			rtr_thr_err;
typedef int			rtr_thr_pri;

typedef struct
{
	rtr_thread_id	thread_id;
	unsigned long	thread_info;
	pthread_attr_t	thread_attr;
} rtr_thread_info;


#define RTR_THREAD_START_FUNC(thr_strt_func,arg) void *thr_strt_func(void *arg)

#define RTR_THR_ERROR_VAR(var)	rtr_thr_err var;

/* Error Values */
#define RTR_THR_ERROR(error)	error

/* Thread creation/termination */

/* Thread creation attributes operations */
#define RTR_INIT_THR_INFO(thr_info_ptr) \
	(thr_info_ptr)->thread_id = 0; \
	(thr_info_ptr)->thread_info = 0; \
	pthread_attr_init(&(thr_info_ptr)->thread_attr)
#define RTR_DESTROY_THR_INFO(thr_info_ptr) \
	pthread_attr_destroy(&(thr_info_ptr)->thread_attr)

#define RTR_SET_THR_BOUND(thr_info_ptr) \
	(thr_info_ptr)->thread_info |= RTR_THR_BOUND; \
	pthread_attr_setscope(&(thr_info_ptr)->thread_attr,PTHREAD_SCOPE_SYSTEM)
#define RTR_SET_THR_DETACHED(thr_info_ptr)\
	(thr_info_ptr)->thread_info |= RTR_THR_DETACHED; \
	pthread_attr_setdetachstate(&(thr_info_ptr)->thread_attr, \
		PTHREAD_CREATE_DETACHED)

/* No Thread suspention in posix threads */
/* #define RTR_SET_THR_SUSPENDED(thr_info_ptr) */

/* SunOS 6 and SunOS 8 do not have the setstack function, so build it out of
 * setstacksize and setstackaddr.
 */
#ifdef sun4_SunOS_56
#define RTR_SET_STACK(thr_info_ptr,base,size)\
	(pthread_attr_setstacksize(&(thr_info_ptr)->thread_attr,size) ? \
	pthread_attr_setstacksize(&(thr_info_ptr)->thread_attr,size) : \
	pthread_attr_setstackaddr(&(thr_info_ptr)->thread_attr,base))
#else
#define RTR_SET_STACK(thr_info_ptr,base,size)\
	pthread_attr_setstack(&(thr_info_ptr)->thread_attr,base,size)
#endif

#define RTR_THREAD_CREATE(thr_info_ptr,thr_strt_func,argval,error_ptr) \
	(((*error_ptr = pthread_create( &((thr_info_ptr)->thread_id), \
		&((thr_info_ptr)->thread_attr),thr_strt_func,(void*)argval)) == 0) \
			? RTRTRUE : RTRFALSE)

#define RTR_THREAD_EXIT()	pthread_exit(0)


/* Thread identifier */
#define RTR_THREAD_ID()		pthread_self()

#if defined(Linux)
#if (defined(__GLIBC__) && __GLIBC__ >= 2 && __GLIBC_MINOR__ >= 12)
#define RTR_SET_THR_NAME(thread_id, name,error_ptr)\
	( ((*error_ptr = pthread_setname_np(thread_id, name)) == 0) \
	? RTRTRUE : RTRFALSE )

#elif defined(PR_SET_NAME)
#define RTR_SET_THR_NAME(thread_id, name,error_ptr)\
	( ((*error_ptr = prctl(PR_SET_NAME, (unsigned long)name, 0, 0, 0)) >= 0) \
	? RTRTRUE : RTRFALSE )
#else
#define RTR_SET_THR_NAME(thread_id, name, error_ptr) RTRTRUE
#endif
#else
#define RTR_SET_THR_NAME(thread_id, name, error_ptr) RTRTRUE
#endif	

/* Thread yielding */
#define RTR_THREAD_YIELD()	sched_yield()
#define RTR_THREAD_TIMEYIELD(millisec)	rtr_time_yield(millisec)


/* Thread joining */
#define RTR_THREAD_JOIN(thr_info_ptr,error_ptr) \
	(((*error_ptr = pthread_join((pthread_t)(thr_info_ptr)->thread_id,0)) == 0 ) ? RTRTRUE : RTRFALSE)


/* Thread suspend/continue */
/* No Thread suspention in posix threads */
/* #define RTR_THREAD_SUSPEND(thr_info_ptr,error_ptr) */
/* #define RTR_THREAD_CONTINUE(thr_info_ptr,error_ptr) */


/* Thread Priority */
extern int rtr_pthr_set_pri(rtr_thread_info*,rtr_thr_err*,rtr_thr_pri);
extern int rtr_pthr_get_pri(rtr_thread_info*,rtr_thr_err*,rtr_thr_pri*);
#define RTR_SET_THREAD_PRI(thr_info_ptr,error_ptr,pri) \
	rtr_pthr_set_pri(thr_info_ptr,error_ptr,pri)
#define RTR_GET_THREAD_PRI(thr_info_ptr,error_ptr,pri_ptr) \
	rtr_pthr_get_pri(thr_info_ptr,error_ptr,pri)


#ifdef SOLARIS2

/* Thread scheduling class */
extern int rtr_sched_real_time(rtr_thr_err*,rtr_thr_pri);
extern int rtr_sched_system(rtr_thr_err*,rtr_thr_pri);
extern int rtr_sched_timeshare(rtr_thr_err*,rtr_thr_pri);
#define RTR_SCHED_REAL_TIME(thr_info_ptr,error_ptr,pri) \
	rtr_sched_real_time(error_ptr,pri)
#define RTR_SCHED_SYSTEM(thr_info_ptr,error_ptr,pri) \
	rtr_sched_system(error_ptr,pri)
#define RTR_SCHED_TIMESHARE(thr_info_ptr,error_ptr,pri) \
	rtr_sched_timeshare(error_ptr,pri)

#endif


/* Thread signals */
#define RTR_THREAD_SIGBLOCK(setptr,osetptr,error_ptr) \
	(((*error_ptr = pthread_sigmask(SIG_BLOCK,setptr,osetptr)) == 0 ) ? \
		RTRTRUE : RTRFALSE)
#define RTR_THREAD_SIGUNBLOCK(setptr,osetptr,error_ptr) \
	(((*error_ptr = pthread_sigmask(SIG_UNBLOCK,setptr,osetptr)) == 0 ) ? \
		RTRTRUE : RTRFALSE)
#define RTR_THREAD_SIGSETMASK(setptr,osetptr,error_ptr) \
	(((*error_ptr = pthread_sigmask(SIG_SETMASK,setptr,osetptr)) == 0 ) ? \
		RTRTRUE : RTRFALSE)
#define RTR_THREAD_KILL(thr_info_ptr,sig,error_ptr) \
	(((*error_ptr = pthread_kill((thr_info_ptr)->thread_id,sig)) == 0 ) ? \
		RTRTRUE : RTRFALSE)
#define RTR_THREAD_SIGWAIT(setptr,sigptr,error_ptr) \
	(((*error_ptr = sigwait(setptr,sigptr)) >= 0 ) ? \
		RTRTRUE : RTRFALSE)

	

/* End posix threads definitions */
/***************************************************************************/
#endif


#if defined(DEV_SOL_THR)
/***************************************************************************/
/* Solaris threads definitions */


typedef thread_t	rtr_thread_id;
typedef size_t		rtr_stack_sz;
typedef int			rtr_thr_err;
typedef int			rtr_thr_pri;

typedef struct
{
	rtr_thread_id	thread_id;
	unsigned long	thread_info;
	long			flags;
	rtr_stack_sz	stack_size;
	void			*stack_base;
} rtr_thread_info;


#define RTR_THREAD_START_FUNC(thr_strt_func,arg) void *thr_strt_func(void *arg)

#define RTR_THR_ERROR_VAR(var)	rtr_thr_err var;

/* Error Values */
#define RTR_THR_ERROR(error)	error


/* Thread creation/termination */

/* Thread creation attributes operations */
#define RTR_INIT_THR_INFO(thr_info_ptr) \
	(thr_info_ptr)->thread_id = 0; (thr_info_ptr)->flags = 0; \
	(thr_info_ptr)->thread_info = 0; \
	(thr_info_ptr)->stack_size = 0; (thr_info_ptr)->stack_base = 0
#define RTR_DESTROY_THR_INFO(thr_info_ptr)

#define RTR_SET_THR_BOUND(thr_info_ptr) \
	(thr_info_ptr)->flags |= THR_BOUND; \
	(thr_info_ptr)->thread_info |= RTR_THR_BOUND
#define RTR_SET_THR_DETACHED(thr_info_ptr) \
	(thr_info_ptr)->flags |= RTR_THR_DETACHED; \
	(thr_info_ptr)->flags |= THR_DETACHED
#define RTR_SET_THR_SUSPENDED(thr_info_ptr) \
	(thr_info_ptr)->flags |= RTR_THR_SUSPENDED; \
	(thr_info_ptr)->flags |= THR_SUSPENDED

#define RTR_SET_STACK(thr_info_ptr,base,size) \
	do \
	{ \
		(thr_info_ptr)->stack_size = size; \
		(thr_info_ptr)->stack_base = base; \
	} while(0)

/* Only Valid on Solaris */
#define RTR_SET_THR_NEW_LWP(thr_info_ptr)	(thr_info_ptr)->flags |= THR_NEW_LWP
#define RTR_SET_THR_DAEMON(thr_info_ptr)	(thr_info_ptr)->flags |= THR_DAEMON
/* End Only Valid on Solaris */


#define RTR_THREAD_CREATE(thr_info_ptr,thr_strt_func,argval,error_ptr) \
	(((*error_ptr = thr_create( (thr_info_ptr)->stack_base, \
			(thr_info_ptr)->stack_size, \
			thr_strt_func,(void*)argval,(thr_info_ptr)->flags, \
			&((thr_info_ptr)->thread_id))) == 0) ? RTRTRUE : RTRFALSE)

#define RTR_THREAD_EXIT()	thr_exit(0)


/* Thread identifier */
#define RTR_THREAD_ID()		thr_self()

/* Not Supported on Solaris */
#define RTR_SET_THR_NAME(thread_id, name, error_ptr) RTRTRUE

/* Thread yielding */
#define RTR_THREAD_YIELD()	thr_yield()
#define RTR_THREAD_TIMEYIELD(millisec)	rtr_time_yield(millisec)


/* Thread joining */
#define RTR_THREAD_JOIN(thr_info_ptr,error_ptr) \
	(((*error_ptr = thr_join((thr_info_ptr)->thread_id,0,0)) == 0 ) \
		? RTRTRUE : RTRFALSE)


/* Thread suspend/continue */
#define RTR_THREAD_SUSPEND(thr_info_ptr,error_ptr) \
	(((*error_ptr = thr_suspend((thr_info_ptr)->thread_id)) == 0 ) \
		? RTRTRUE : RTRFALSE)

#define RTR_THREAD_CONTINUE(thr_info_ptr,error_ptr) \
	(((*error_ptr = thr_continue((thr_info_ptr)->thread_id)) == 0 ) \
		? RTRTRUE : RTRFALSE)


extern int rtr_thr_set_real_time_pri(rtr_thread_info*,rtr_thr_err,int);
extern int rtr_thr_set_pri(rtr_thread_info*,rtr_thr_err,int);

/* Thread priority */
#define RTR_SET_THREAD_PRI(thr_info_ptr,error_ptr,pri) \
	(((*error_ptr = thr_setprio((thr_info_ptr)->thread_id,pri)) == 0 ) \
		? RTRTRUE : RTRFALSE)
#define RTR_GET_THREAD_PRI(thr_info_ptr,error_ptr,pri_ptr) \
	(((*error_ptr = thr_getprio((thr_info_ptr)->thread_id,pri_ptr)) == 0 ) \
		? RTRTRUE : RTRFALSE)


extern int rtr_sched_real_time(rtr_thr_err*,rtr_thr_pri);
extern int rtr_sched_system(rtr_thr_err*,rtr_thr_pri);
extern int rtr_sched_timeshare(rtr_thr_err*,rtr_thr_pri);
/* Thread scheduling class */
#define RTR_SCHED_REAL_TIME(thr_info_ptr,error_ptr,pri) \
	rtr_sched_real_time(error_ptr,pri)
#define RTR_SCHED_SYSTEM(thr_info_ptr,error_ptr,pri) \
	rtr_sched_system(error_ptr,pri)
#define RTR_SCHED_TIMESHARE(thr_info_ptr,error_ptr,pri) \
	rtr_sched_timeshare(error_ptr,pri)


/* Thread signals */
#define RTR_THREAD_SIGBLOCK(setptr,osetptr,error_ptr) \
	(((*error_ptr = thr_sigsetmask(SIG_BLOCK,setptr,osetptr)) == 0 ) ? \
		RTRTRUE : RTRFALSE)
#define RTR_THREAD_SIGUNBLOCK(setptr,osetptr,error_ptr) \
	(((*error_ptr = thr_sigsetmask(SIG_UNBLOCK,setptr,osetptr)) == 0 ) ? \
		RTRTRUE : RTRFALSE)
#define RTR_THREAD_SIGSETMASK(setptr,osetptr,error_ptr) \
	(((*error_ptr = thr_sigsetmask(SIG_SETMASK,setptr,osetptr)) == 0 ) ? \
		RTRTRUE : RTRFALSE)
#define RTR_THREAD_KILL(thr_info_ptr,sig,error_ptr) \
	(((*error_ptr = thr_kill((thr_info_ptr)->thread_id,sig)) == 0 ) ? \
		RTRTRUE : RTRFALSE)
#define RTR_THREAD_SIGWAIT(setptr,sigptr,error_ptr) \
	(((*sigptr = sigwait(setptr)) >= 0 ) ? \
		RTRTRUE : ((*error_ptr = errno) ? RTRFALSE : RTRFALSE) )
	

/* End solaris threads definitions */
/***************************************************************************/
#endif




#ifdef _WIN32
/***************************************************************************/
/* win32 thread definitions */

typedef unsigned	rtr_thread_id;
typedef unsigned	rtr_stack_sz;
typedef DWORD		rtr_thr_err;
typedef int			rtr_thr_pri;

typedef struct
{
	rtr_thread_id	thread_id;
	unsigned long	thread_info;
	HANDLE			handle;
	long			flags;
	rtr_stack_sz	stack_size;
	void			*stack_base;
} rtr_thread_info;


#define RTR_THREAD_START_FUNC(thr_strt_func,arg) unsigned __stdcall thr_strt_func(void *arg)

#define RTR_THR_ERROR_VAR(var)

/* Error Values */
#define RTR_THR_ERROR(error)	GetLastError()


/* Thread creation/termination */

/* Thread creation attributes operations */
#define RTR_INIT_THR_INFO(thr_info_ptr) \
	(thr_info_ptr)->thread_id = 0; (thr_info_ptr)->handle = 0; \
	(thr_info_ptr)->thread_info = RTR_THR_BOUND; (thr_info_ptr)->flags = 0; \
	(thr_info_ptr)->stack_size = 0; (thr_info_ptr)->stack_base = 0
#define RTR_DESTROY_THR_INFO(thr_info_ptr)

#define RTR_SET_THR_BOUND(thr_info_ptr)
/* #define RTR_SET_THR_DETACHED(thr_info_ptr) */
#define RTR_SET_THR_SUSPENDED(thr_info_ptr) \
	(thr_info_ptr)->thread_info |= RTR_THR_SUSPENDED; \
	(thr_info_ptr)->flags |= CREATE_SUSPENDED

#define RTR_SET_STACK(thr_info_ptr,base,size) \
	do \
	{ \
		(thr_info_ptr)->stack_size = size; \
		(thr_info_ptr)->stack_base = base; \
	} while(0)

#define RTR_THREAD_CREATE(thr_info_ptr,thr_strt_func,argval,error_ptr) \
	((((thr_info_ptr)->handle = (HANDLE)_beginthreadex( 0, \
		(thr_info_ptr)->stack_size, \
		thr_strt_func,(void*)argval, (thr_info_ptr)->flags, \
		&((thr_info_ptr)->thread_id))) == 0) ? RTRFALSE : RTRTRUE)

#define RTR_THREAD_EXIT()	_endthreadex(0)


/* Thread identifier */
#define RTR_THREAD_ID()		GetCurrentThreadId()

/* specifically for setting the name of a thread */
typedef struct tagTHREADNAME_INFO
{
   DWORD dwType; // must be 0x1000
   LPCSTR szName; // pointer to name (in user addr space)
   DWORD dwThreadID; // thread ID (-1=caller thread)
   DWORD dwFlags; // reserved for future use, must be zero
} THREADNAME_INFO;

extern int SetThreadName(DWORD,LPCSTR);
#define RTR_SET_THR_NAME(thread_id, name,error_ptr) SetThreadName(thread_id, name)

/* Thread yielding */
#define RTR_THREAD_YIELD()	Sleep(0)
#define RTR_THREAD_TIMEYIELD(millisec)	rtr_time_yield(millisec)


/* Thread joining */
#define RTR_THREAD_JOIN(thr_info_ptr,error_ptr) \
	(((*error_ptr = WaitForSingleObject((thr_info_ptr)->handle,INFINITE))==0) ?\
		(CloseHandle((thr_info_ptr)->handle) ? RTRTRUE : RTRTRUE ) : RTRFALSE)


/* Thread suspend/continue */
#define RTR_THREAD_SUSPEND(thr_info_ptr,error_ptr) \
	(((*error_ptr = SuspendThread((thr_info_ptr)->handle)) == 0 ) ?\
		RTRTRUE : RTRFALSE)

#define RTR_THREAD_CONTINUE(thr_info_ptr,error_ptr) \
	(((*error_ptr = ResumeThread((thr_info_ptr)->handle)) == 0 ) ?\
		RTRTRUE : RTRFALSE)


/* Thread priority */
/* #define RTR_SET_THREAD_PRI(thr_info_ptr,error_ptr,pri) */
/* #define RTR_GET_THREAD_PRI(thr_info_ptr,error_ptr,pri_ptr) */


/* Thread scheduling class */
#define RTR_SCHED_REAL_TIME(thr_info_ptr,error_ptr,pri) \
	((SetThreadPriority((thr_info_ptr)->handle,THREAD_PRIORITY_TIME_CRITICAL))\
		? RTRTRUE : RTRFALSE)
#define RTR_SCHED_SYSTEM(thr_info_ptr,error_ptr,pri) \
	((SetThreadPriority((thr_info_ptr)->handle,THREAD_PRIORITY_ABOVE_NORMAL))\
		? RTRTRUE : RTRFALSE)
#define RTR_SCHED_TIMESHARE(thr_info_ptr,error_ptr,pri) \
	((SetThreadPriority((thr_info_ptr)->handle,THREAD_PRIORITY_NORMAL))\
		? RTRTRUE : RTRFALSE)

/* End win32 threads definitions */
/***************************************************************************/
#endif

#ifdef __cplusplus
}
#endif

#endif
