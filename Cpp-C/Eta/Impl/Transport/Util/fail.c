/*|-----------------------------------------------------------------------------
 *|            This source code is provided under the Apache 2.0 license      --
 *|  and is provided AS IS with no warranty or guarantee of fit for purpose.  --
 *|                See the project's LICENSE.md for details.                  --
 *|           Copyright Thomson Reuters 2018. All rights reserved.            --
 *|-----------------------------------------------------------------------------
 */

#include "rtr/rtrdefs.h"

#ifndef _KERNEL

#include <stdio.h>
#include <stdlib.h>

void rtrfail(const char* msg, const char* file, int line)
{
	static char fmt[] = "Fatal Error: %s (file %s, line %d)\n";
	fprintf(stderr, fmt, msg, file, line);
	abort();
}

#else

#include <sys/debug.h>

void rtrfail(const char* msg, const char* file, int line)
{
	assfail(msg,__FILE__,__LINE__);
}

#endif
