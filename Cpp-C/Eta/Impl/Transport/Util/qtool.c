/*|-----------------------------------------------------------------------------
 *|            This source code is provided under the Apache 2.0 license      --
 *|  and is provided AS IS with no warranty or guarantee of fit for purpose.  --
 *|                See the project's LICENSE.md for details.                  --
 *|           Copyright Thomson Reuters 2018. All rights reserved.            --
 *|-----------------------------------------------------------------------------
 */

#include "rtr/qtool.h"


void rtr_qtool_init(rtr_qtool_list *qList, int offset)
{
	rtr_int_qtool_init(&(qList->head));
	qList->offset = offset;
}
