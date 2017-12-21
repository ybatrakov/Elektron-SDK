/*|-----------------------------------------------------------------------------
 *|            This source code is provided under the Apache 2.0 license      --
 *|  and is provided AS IS with no warranty or guarantee of fit for purpose.  --
 *|                See the project's LICENSE.md for details.                  --
 *|           Copyright Thomson Reuters 2018. All rights reserved.            --
 *|-----------------------------------------------------------------------------
 */

#ifndef __platlog_h
#define __platlog_h

#include "rtr/platform.h"

#if defined(__cplusplus)
extern "C" {
#endif

#define PLAT_LOG_DEBUG	1
#define PLAT_LOG_ERROR	2

#if 1  
extern void (*platLogger)(char *className,int logType,char *logtxt);
extern void setPlatLogger(void (*)(char*,int,char*));
extern void clearPlatLogger();
#else
extern void (*platLogger)();
extern void setPlatLogger(void (*)());
extern void clearPlatLogger();
#endif


#define PLATFORM_LOG(className,logType,logtxt) \
	if (platLogger) (*platLogger)((char*)className,(int)logType,(char*)logtxt)

#if defined(__cplusplus)
}
#endif


#endif
