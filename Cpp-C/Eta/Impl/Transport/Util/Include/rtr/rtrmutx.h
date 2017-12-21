/*|-----------------------------------------------------------------------------
 *|            This source code is provided under the Apache 2.0 license      --
 *|  and is provided AS IS with no warranty or guarantee of fit for purpose.  --
 *|                See the project's LICENSE.md for details.                  --
 *|           Copyright Thomson Reuters 2018. All rights reserved.            --
 *|-----------------------------------------------------------------------------
 */

//  description:
//	Private Mutex - Critical Sections.
//
//	This header file defines the macros used to define private mutex's.
//	A private mutex (also called a critical section) can only exist
//	in one process. It cannot cross process boundaries.
//	Because of this it is much quicker than a public mutex.
//
//
//	Expected Use:
//
//	main()
//	{
//		rtr_mutex		mutex;
//		rtr_mutex_error	mutex_error;
//
//		if (RTR_MUTEX_INIT(mutex,mutex_error))
//		{
//			printf("Mutex initialized\n");
//		}
//		else
//		{
//			printf("Could not initialize mutex error %d\n",RTR_MUTEX_ERROR(mutex_error));
//			exit(1);
//		}
//
//		if (!RTR_MUTEX_LOCK(mutex,mutex_error))
//		{
//			printf("Could not lock mutex error %d\n",RTR_MUTEX_ERROR(mutex_error));
//			exit(1);
//		}
//
//		if (!RTR_MUTEX_UNLOCK(mutex,mutex_error))
//		{
//			printf("Could not unlock mutex error %d\n",RTR_MUTEX_ERROR(mutex_error));
//			exit(1);
//		}
//
//		if (!RTR_MUTEX_TRYLOCK(mutex,mutex_error))
//		{
//			if (!RTR_MUTEX_BUSY(mutex_error))
//			{
//				printf("Could not trylock mutex error %d\n",RTR_MUTEX_ERROR(mutex_error));
//				exit(1);
//			}
//		}
//		else if (!RTR_MUTEX_UNLOCK(mutex,mutex_error))
//		{
//			printf("Could not unlock mutex error %d\n",RTR_MUTEX_ERROR(mutex_error));
//			exit(1);
//		}
//
//		if (!RTR_MUTEX_DESTROY(mutex,mutex_error))
//			printf("Could no destroy mutex error %d\n",RTR_MUTEX_ERROR(mutex_error));
//	}


#ifndef __rtrmutx_h
#define __rtrmutx_h

#include "rtr/rtrdefs.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum { rtr_mutexattr_default = 0, rtr_mutexattr_recursive = 1 } rtr_mutex_attributes;

#ifdef DEV_POS_THR
/***************************************************************************/
/* Posix threads definitions */

#include <pthread.h>

typedef pthread_mutex_t	rtr_mutex;
typedef int rtr_mutex_error;
typedef pthread_mutexattr_t rtr_mutexattr;
#define RTR_MUTEX_ERROR_VAR(var)	rtr_mutex_error var;

#define RTR_MUTEX_ERROR(error)	error

#define RTR_MUTEX_INIT(mutex,error) \
	(((error = pthread_mutex_init(&mutex,(pthread_mutexattr_t *)PTHREAD_PROCESS_PRIVATE)) == 0 ) ? RTRTRUE : RTRFALSE)

static inline RTRBOOL rtr_mutex_initattr(rtr_mutex *mutex, int *error, int attribute)
{
	pthread_mutexattr_t attr;
	pthread_mutexattr_init (&attr);
#if defined (sun4) && defined(SparcVersion) && SparcVersion < 10
/*  Recursive mutexes are not supported on platforms earlier than Solaris10 */
	return RTRFALSE;
#else
	if (attribute == rtr_mutexattr_recursive)
		*error = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
	else
		return RTRTRUE;

	if (*error)
		return RTRTRUE;

	*error = pthread_mutex_init(mutex, &attr);
	if (*error)
		return RTRTRUE;
	else
		return RTRFALSE;
#endif
}

#define RTR_MUTEX_DESTROY(mutex,error) \
	(((error = pthread_mutex_destroy(&mutex)) == 0 ) ? RTRTRUE : RTRFALSE)

#define RTR_MUTEX_LOCK(mutex,error) \
	(((error = pthread_mutex_lock(&mutex)) == 0) ? RTRTRUE : RTRFALSE)

#define RTR_MUTEX_UNLOCK(mutex,error) \
	(((error = pthread_mutex_unlock(&mutex)) == 0) ? RTRTRUE : RTRFALSE)

#define RTR_MUTEX_TRYLOCK(mutex,error) \
	(((error = pthread_mutex_trylock(&mutex)) == 0) ? RTRTRUE : RTRFALSE)

#define RTR_MUTEX_BUSY(error) (error == EBUSY)

/* End posix threads definitions */
/***************************************************************************/
#endif



#if defined(DEV_SOL_THR)
/***************************************************************************/
/* Solaris threads definitions */

#include <thread.h>
#include <synch.h>

typedef mutex_t	rtr_mutex;
typedef int rtr_mutex_error;
#define RTR_MUTEX_ERROR_VAR(var)	rtr_mutex_error var;

#define RTR_MUTEX_ERROR(error)	error

#define RTR_MUTEX_INIT(mutex,error) \
	(((error = mutex_init(&mutex,USYNC_THREAD,0)) == 0 ) ? RTRTRUE : RTRFALSE)

static RTRBOOL rtr_mutex_initattr(rtr_mutex *mutex, int *error, int attribute)
{
#if defined (sun4) && defined(SparcVersion) && SparcVersion < 10
	return RTRFALSE;
#else
	if (attribute == rtr_mutexattr_recursive)
		*error = mutex_init(mutex, LOCK_RECURSIVE | USYNC_THREAD,0);
	else
		return RTRTRUE;
	
	if (*error)
		return RTRTRUE;
	else
		return RTRFALSE;
#endif
}

#define RTR_MUTEX_DESTROY(mutex,error) \
	(((error = mutex_destroy(&mutex)) == 0 ) ? RTRTRUE : RTRFALSE)

#define RTR_MUTEX_LOCK(mutex,error) \
	(((error = mutex_lock(&mutex)) == 0) ? RTRTRUE : RTRFALSE)

#define RTR_MUTEX_UNLOCK(mutex,error) \
	(((error = mutex_unlock(&mutex)) == 0) ? RTRTRUE : RTRFALSE)

#define RTR_MUTEX_TRYLOCK(mutex,error) \
	(((error = mutex_trylock(&mutex)) == 0) ? RTRTRUE : RTRFALSE)

#define RTR_MUTEX_BUSY(error) (error == EBUSY)

/* End solaris threads definitions */
/***************************************************************************/
#endif



#ifdef _WIN32
/***************************************************************************/
/* win32 thread definitions */

#include <windows.h>

typedef DWORD rtr_mutex_error;
#define RTR_MUTEX_ERROR_VAR(var)

#if (_WIN32_WINNT >= 0x0500)
/* Windows 2000 or newer */

typedef CRITICAL_SECTION	rtr_mutex;

#define RTR_MUTEX_ERROR(error)	0

#define RTR_MUTEX_INIT(mutex,error) \
	(InitializeCriticalSectionAndSpinCount(&mutex,512),RTRTRUE)

/* TODO: inline? */
static RTRBOOL rtr_mutex_initattr(rtr_mutex *mutex, int *error, int attribute)
{
	/* Windows Critical sections are recursive by default */
	if (attribute == rtr_mutexattr_recursive)
		return RTR_MUTEX_INIT(*mutex, *error);
	else
		return RTRTRUE;
}

#define RTR_MUTEX_INIT_SPIN(mutex,spin,error) \
	(InitializeCriticalSectionAndSpinCount(&mutex,spin),RTRTRUE)

#define RTR_MUTEX_INIT_NO_SPIN(mutex,error) \
	(InitializeCriticalSection(&mutex),RTRTRUE)

#define RTR_MUTEX_DESTROY(mutex,error) \
	(DeleteCriticalSection(&mutex),RTRTRUE)

#define RTR_MUTEX_LOCK(mutex,error) \
	(EnterCriticalSection(&mutex),RTRTRUE)

#define RTR_MUTEX_UNLOCK(mutex,error) \
	(LeaveCriticalSection(&mutex),RTRTRUE)

#define RTR_MUTEX_TRYLOCK(mutex,error) \
	((TryEnterCriticalSection(&mutex) == TRUE) ? RTRTRUE : RTRFALSE)

#define RTR_MUTEX_BUSY(error) RTRTRUE

/* end Windows 2000 or newer */

#else
/* WinNT 4 */

#if !defined(_WIN32_WINNT)
#if (WINVER >= 0x0400)
WINBASEAPI BOOL WINAPI TryEnterCriticalSection(LPCRITICAL_SECTION);
#else
#define _WIN_NO_CRIT_SEC
#endif
#endif

typedef CRITICAL_SECTION	rtr_mutex;

#define RTR_MUTEX_ERROR(error)	0

#define RTR_MUTEX_INIT(mutex,error) \
	(InitializeCriticalSection(&mutex),RTRTRUE)

#define RTR_MUTEX_DESTROY(mutex,error) \
	(DeleteCriticalSection(&mutex),RTRTRUE)

#define RTR_MUTEX_LOCK(mutex,error) \
	(EnterCriticalSection(&mutex),RTRTRUE)

#define RTR_MUTEX_UNLOCK(mutex,error) \
	(LeaveCriticalSection(&mutex),RTRTRUE)

#define RTR_MUTEX_TRYLOCK(mutex,error) \
	((TryEnterCriticalSection(&mutex) == TRUE) ? RTRTRUE : RTRFALSE)

#define RTR_MUTEX_BUSY(error) RTRTRUE

/* End WinNT 4 defs */
#endif

/* End win32 threads definitions */
/***************************************************************************/
#endif


#ifndef RTR_MUTEX_INIT
/***************************************************************************/
/* Dummy threads definitions */
#define RTR_DUMMY_MUTEX_DEFS

typedef int	rtr_mutex;
typedef int rtr_mutex_error;
#define RTR_MUTEX_ERROR_VAR(var)	rtr_mutex_error var;

#define RTR_MUTEX_ERROR(error)	error

#define RTR_MUTEX_INIT(mutex,error)	RTRTRUE

#define RTR_MUTEX_DESTROY(mutex,error)	RTRTRUE

#define RTR_MUTEX_LOCK(mutex,error) RTRTRUE

#define RTR_MUTEX_UNLOCK(mutex,error) RTRTRUE

#define RTR_MUTEX_TRYLOCK(mutex,error) RTRTRUE

#define RTR_MUTEX_BUSY(error) RTRFALSE

/* End dummy threads definitions */
/***************************************************************************/
#endif

#ifdef __cplusplus
}
#endif


/* Thread-safe static mutex */


/*   Example of the static mutex usage:
*     
*    #include "rtr/rtrmutx.h"
*
*   
*    static  RTR_STATIC_MUTEX_DECL(s_rsslStaticDataMutex);
*
*    extern "C"  void            _rsslStaticDataMutexLock()
*    {
*        RTR_STATIC_MUTEX_LOCK( s_rsslStaticDataMutex );
*    }
*
*
*    extern "C"  void            _rsslStaticDataMutexUnlock()
*    {
*        RTR_STATIC_MUTEX_UNLOCK( s_rsslStaticDataMutex );
*    }
*
*/

#ifdef  _WIN32

#include "rtr/rtratomic.h"

	/* For windows need to implement as a spin lock */
#define RTR_STATIC_MUTEX_DECL(G_VAR)	rtr_atomic_val G_VAR=0;

	/* RTR_ATOMIC_SET_RETOLD returns the previous value of rtr_my_spin_lock.
	 * If it was already set to 1 then somebody else has
	 * already set it.
	 */
#define RTR_STATIC_MUTEX_LOCK(G_VAR) \
		do { \
			while (RTR_ATOMIC_SET_RETOLD(G_VAR,1) == 1) \
				Sleep(0); \
		} while( 0 )

#define RTR_STATIC_MUTEX_UNLOCK(G_VAR) RTR_ATOMIC_SET_RETOLD(G_VAR,0)

#else /* Unix */

#define RTR_STATIC_MUTEX_DECL(G_VAR)     rtr_mutex    G_VAR = PTHREAD_MUTEX_INITIALIZER
#define RTR_STATIC_MUTEX_LOCK(G_VAR)     do { rtr_mutex_error __error; RTR_MUTEX_LOCK(G_VAR,__error); } while( 0 )
#define RTR_STATIC_MUTEX_UNLOCK(G_VAR)   do { rtr_mutex_error __error; RTR_MUTEX_UNLOCK(G_VAR,__error); } while( 0 )     

#endif /* _WIN32 */ 

#endif


