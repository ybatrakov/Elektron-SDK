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
//		rtr_sigset	- Signal set definition
//
//	defines:
//		RTR_SIG_HANDLER(sighandler,sig)
//			- Declaration of the signal handler function.
//			- sighandler is the function name and
//			- sig is the signal number.
//
//		RTR_SIG_HAND_TYPE
//			- typecast for signal handler function.
//
//		RTR_SET_SIG_HANDLER(sig,handler,sigsetp,flags)
//			- Set a signal handler function
//				sig 	: signal number
//				handler	: function name
//				sigsetp : pointer to signal mask to block while in signal
//				flags	: flags
//					RTR_SIG_RESTART - Restart system calls on signal.
//
//		RTR_DFLT_SIG_HANDLER(sig)
//			- Set signal handler to default action for sig.
//
//		RTR_IGNORE_SIG_HANDLER(sig)
//			- Ignore sig signals.
//
//		RTR_SIG_EMPTY_SET(setp)
//			- Clear out signal set (setp - pointer to set)
//		RTR_SIG_FILL_SET(setp)
//			- Fill the signal set (setp - pointer to set)
//		RTR_SIG_ADDTO_SET(setp,sig)
//			- Add signal sig to set (setp - pointer to set)
//		RTR_SIG_REMOVEFROM_SET(setp,sig)
//			- Remove signal sig from set (setp - pointer to set)
//		RTR_SIG_ISMEMBER(setp,sig)
//			- Is sig a member of the set (setp - pointer to set)
//
//		RTR_PROC_BLOCK(newmask_p,oldmask_p)
//			- Add signals in newmask_p to signal mask for
//			- this process.
//
//		RTR_PROC_UNBLOCK(newmask_p,oldmask_p)
//			- Remove signal in newmask_p from signal mask for
//			- this process.
//
//		RTR_PROC_SETMASK(newmask_p,oldmask_p)
//			- Set the signal mask for this process to newmask_p
//
//
//
//	Expected Use:
//
//	RTR_SIG_HANDLER(sigiohandler,sig)
//	{
//		printf("In sigio handler %d\n",sig);
//	}
//
//	main()
//	{
//		rtr_sigset sigset;
//		RTR_SIG_FILL_SET(&sigset);
//		RTR_SET_SIG_HANDLER(SIGIO,sigiohandler,&sigset,0);
//		sleep(100);
//		RTR_DFLT_SIG_HANDLER(SIGIO);
//	}
//


#ifndef __rtrsig_h
#define __rtrsig_h

#include "rtr/rtrdefs.h"
#include "rtr/platform.h"

#ifndef WIN32
#include <signal.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif


#ifndef WIN32

	/* SUNPRO_CC sparcworks compiler */
#if defined(__cplusplus) && defined(sun4_SunOS_41X) && defined(__SUNPRO_CC)
#define RTR_SIG_HANDLER(handler,sig) void handler(int sig,DOTDOTDOT)
#else
#define RTR_SIG_HANDLER(handler,sig) void handler(int sig)
#endif


#if defined(__cplusplus) && defined(sun4_SunOS_41X) && defined(__SUNPRO_CC)
#define RTR_SIG_HAND_TYPE	void(*)(int,DOTDOTDOT)

#elif defined(SIG_PF)
#define RTR_SIG_HAND_TYPE	SIG_PF

#else
#if 1
#define RTR_SIG_HAND_TYPE	void(*)(int)
#else
#define RTR_SIG_HAND_TYPE	void(*)()
#endif	/* 1 */
#endif	/* defined(__cplusplus) && defined(sun4_SunOS_41X) && defined(__SUNPRO_CC) */


	/***************************/
	/* Signal mask definitions */
#if defined(DEV_SVR4) || defined(DEV_SVR4_SIGNALS)
typedef sigset_t	rtr_sigset;

#define RTR_SIG_EMPTY_SET(setp)				sigemptyset(setp)
#define RTR_SIG_FILL_SET(setp)				sigfillset(setp)
#define RTR_SIG_ADDTO_SET(setp,sig)			sigaddset(setp,sig)
#define RTR_SIG_REMOVEFROM_SET(setp,sig)	sigdelset(setp,sig)
#define RTR_SIG_ISMEMBER(setp,sig)			sigismember(setp,sig)

#elif defined(DEV_UNIX)
typedef int			rtr_sigset;

#define RTR_SIG_EMPTY_SET(setp)				*setp = 0;
#define RTR_SIG_FILL_SET(setp)				*setp =\
			sigmask(SIGIO)|sigmask(SIGTERM)|sigmask(SIGINT)| \
			sigmask(SIGUSR1)|sigmask(SIGUSR2)|sigmask(SIGPIPE)| \
			sigmask(SIGURG)|sigmask(SIGBUS)|sigmask(SIGSEGV)| \
			sigmask(SIGHUP)
#define RTR_SIG_ADDTO_SET(setp,sig)			(*setp |= sigmask(sig))
#define RTR_SIG_REMOVEFROM_SET(setp,sig)	(*setp &= ~(sigmask(sig)))
#define RTR_SIG_ISMEMBER(setp,sig)			(*setp & sigmask(sig))

#endif

/* 45port patch for vms */
#if defined(VMS)
typedef int                     rtr_sigset;  
#endif


	/**************************/
	/* Signal handler setting */
	/* Passed in as flags in RTR_SET_SIG_HANDLER */
#define RTR_SIG_RESTART		0x01		/* restart system call on interrupt */
   
extern RTRBOOL rtr_set_sig_handler(int,RTR_SIG_HAND_TYPE,rtr_sigset*,int);
 
#define RTR_SET_SIG_HANDLER(sig,handler,sigsetp,flags) \
		rtr_set_sig_handler(sig,handler,sigsetp,flags)
#define RTR_DFLT_SIG_HANDLER(sig) \
		rtr_set_sig_handler(sig,SIG_DFL,(rtr_sigset*)0,0)
#define RTR_IGNORE_SIG_HANDLER(sig) \
		rtr_set_sig_handler(sig,SIG_IGN,(rtr_sigset*)0,0)



	/******************************************/
	/* Signal blocking/unblocking definitions */
#if defined(DEV_SVR4) || defined(DEV_SVR4_SIGNALS)

#define RTR_PROC_BLOCK(newmask_p,oldmask_p) \
	((sigprocmask(SIG_BLOCK,newmask_p,oldmask_p) == 0) ? RTRTRUE : RTRFALSE)
#define RTR_PROC_UNBLOCK(newmask_p,oldmask_p) \
	((sigprocmask(SIG_UNBLOCK,newmask_p,oldmask_p) == 0) ? RTRTRUE : RTRFALSE)
#define RTR_PROC_SETMASK(newmask_p,oldmask_p) \
	((sigprocmask(SIG_SETMASK,newmask_p,oldmask_p) == 0) ? RTRTRUE : RTRFALSE)

#elif defined(DEV_UNIX)

#define RTR_PROC_BLOCK(newmask_p,oldmask_p) \
	(((*oldmask_p = sigblock(*newmask_p)) >= 0) ? RTRTRUE : RTRFALSE)
#define RTR_PROC_UNBLOCK(newmask_p,oldmask_p) \
	(((*oldmask_p = sigsetmask(sigsetmask(0) & ~(*newmask_p))) >= 0) ? \
			RTRTRUE : RTRFALSE)
#define RTR_PROC_SETMASK(newmask_p,oldmask_p) \
	(((*oldmask_p = sigsetmask(*newmask_p)) >= 0) ? RTRTRUE : RTRFALSE)

#endif


#endif	/* ifndef WIN32 */

#ifdef __cplusplus
}
#endif


#endif	/* ifndef __rtrsig_h */
