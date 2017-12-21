/*|-----------------------------------------------------------------------------
 *|            This source code is provided under the Apache 2.0 license      --
 *|  and is provided AS IS with no warranty or guarantee of fit for purpose.  --
 *|                See the project's LICENSE.md for details.                  --
 *|           Copyright Thomson Reuters 2018. All rights reserved.            --
 *|-----------------------------------------------------------------------------
 */

#include "rtr/intqtool.h"

void rtr_int_qtool_init(rtr_int_qtool_list *qList)
{
	qList->flink = (char*)qList;
	qList->blink = (char*)qList;
}


