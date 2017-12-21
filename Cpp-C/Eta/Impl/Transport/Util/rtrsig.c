/*|-----------------------------------------------------------------------------
 *|            This source code is provided under the Apache 2.0 license      --
 *|  and is provided AS IS with no warranty or guarantee of fit for purpose.  --
 *|                See the project's LICENSE.md for details.                  --
 *|           Copyright Thomson Reuters 2018. All rights reserved.            --
 *|-----------------------------------------------------------------------------
 */

#include "rtr/rtrsig.h"

#ifndef WIN32

RTRBOOL rtr_set_sig_handler(int sig, RTR_SIG_HANDLER(handler,sig),
						rtr_sigset *sigsetp, int flags)
{
#if defined(DEV_SVR4) || defined(DEV_SVR4_SIGNALS)
	struct sigaction	nact;
	rtr_sigset			dsigset;

	nact.sa_handler = (RTR_SIG_HAND_TYPE)handler;
	if (sigsetp)
		nact.sa_mask = *sigsetp;
	else
	{
		RTR_SIG_EMPTY_SET(&dsigset);
		nact.sa_mask = dsigset;
	}
	nact.sa_flags = 0;

#if defined(SOLARIS2) || defined(IBMRS) || defined(alpha_OSF1)
	if (flags & RTR_SIG_RESTART) nact.sa_flags = SA_RESTART;
#elif defined(DEV_UNIX)
#ifndef HP_UX
	if (!(flags & RTR_SIG_RESTART)) nact.sa_flags = SA_INTERRUPT;
#endif
#endif

	return( (sigaction(sig,&nact,(struct sigaction *)0) == 0) ?
					RTRTRUE: RTRFALSE);
#else

	struct sigvec	vec;

	vec.sv_handler = (RTR_SIG_HAND_TYPE)handler;
	if (sigsetp)
		vec.sv_mask = *sigsetp;
#if !defined(VMS)
		vec.sv_flags = 0 ;
#else
		vec.sv_onstack = 0 ;
#endif
	return( (sigvec(sig,&vec,(struct sigvec*)0) == 0) ? RTRTRUE: RTRFALSE);

#endif	/* defined(DEV_SVR4) || defined(DEV_SVR4_SIGNALS) */
}

#endif
