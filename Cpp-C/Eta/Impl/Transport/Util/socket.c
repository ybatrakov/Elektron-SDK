/*|-----------------------------------------------------------------------------
 *|            This source code is provided under the Apache 2.0 license      --
 *|  and is provided AS IS with no warranty or guarantee of fit for purpose.  --
 *|                See the project's LICENSE.md for details.                  --
 *|           Copyright Thomson Reuters 2018. All rights reserved.            --
 *|-----------------------------------------------------------------------------
 */

#include "rtr/rtrdefs.h"
#include "rtr/socket.h"
#include "rtr/rtratomic.h"

#ifdef _WIN32
#include <stdio.h>
#endif

static rtr_atomic_val rtr_SocketInits=0;

#ifdef _WIN32
static int doSocketStart(char *errorText)
{
	WSADATA	wsaData;
	if(WSAStartup(MAKEWORD(2,2), &wsaData))
	{
		snprintf(errorText,128, "WSAStartup() Failed (%d).",WSAGetLastError());
		return -1;
	}
	if((LOBYTE(wsaData.wVersion) != 2) || (LOBYTE(wsaData.wVersion) != 2))
	{
		WSACleanup();
		snprintf(errorText,128,"WSAStartup() does not support version (%d.%d).",2,2);
		return -1;
	}
	return 1;
}
#else
static int doSocketStart(char *errorText)
{
	return 1;
}
#endif

int rtr_socket_startup(char *errorText)
{
	int ret = 1;
	if (RTR_ATOMIC_INCREMENT_RET(rtr_SocketInits) == 1)
	{
		ret = doSocketStart(errorText);
		if (ret < 0)
		{
			RTR_ATOMIC_DECREMENT(rtr_SocketInits);
		}
	}
	return ret;
}

int rtr_socket_shutdown()
{
	if (RTR_ATOMIC_DECREMENT_RET(rtr_SocketInits) == 0)
	{
#ifdef _WIN32
		WSACleanup();
#endif
	}
	return 1;
}

