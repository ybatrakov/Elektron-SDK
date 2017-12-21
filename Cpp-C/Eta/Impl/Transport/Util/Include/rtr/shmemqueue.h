/*|-----------------------------------------------------------------------------
 *|            This source code is provided under the Apache 2.0 license      --
 *|  and is provided AS IS with no warranty or guarantee of fit for purpose.  --
 *|                See the project's LICENSE.md for details.                  --
 *|           Copyright Thomson Reuters 2018. All rights reserved.            --
 *|-----------------------------------------------------------------------------
 */

#ifndef __RTR_SHMEM_QUEUE
#define __RTR_SHMEM_QUEUE

#ifdef __cplusplus
extern "C" {
#endif

#include "shmem.h"

typedef struct
{
	RTR_SHM_OFFSET	prev;
	RTR_SHM_OFFSET	next;
} rtrShmQueueElem;

typedef struct
{
	rtrShmQueueElem	head;
	rtrUInt64	count;
	rtrUInt64   linkOffset;
} rtrShmQueue;


#define RTR_SHM_MAKEQE_PTR(___baseptr,___offset) \
  ((rtrShmQueueElem*)((RTR_SHM_OFFSET)___baseptr + ___offset))


RTR_C_ALWAYS_INLINE int RTRShmQueueIsEmpty(	rtrShmQueue *queue, char *shMemBase )
{
//	Assert(RTR_SHM_PTR_VALID(queue));
	return ((queue->head.prev == RTR_SHM_MAKE_OFFSET(shMemBase,&queue->head)) ? 1 : 0);
};

void RTRShmQueueInit(		rtrShmQueue *queue,
							char *shMemBase,
							rtrUInt64 linkOffset);

void RTRShmQueueElemInit(	rtrShmQueueElem *element );

void RTRShmQueueInsertTail(	rtrShmQueue *queue,
							rtrShmQueueElem *element,
							char *shMemBase );

void RTRShmQueueInsertHead(	rtrShmQueue *queue,
							rtrShmQueueElem *element,
							char *shMemBase );

void* RTRShmQueueRemoveTail(rtrShmQueue *queue,
							char *shMemBase );

void* RTRShmQueueRemoveHead(rtrShmQueue *queue,
							char *shMemBase );

void* RTRShmQueueGetFirst(	rtrShmQueue *queue,
							char *shMemBase );

void* RTRShmQueueGetNext(	rtrShmQueue *queue,
							rtrShmQueueElem *current,
							char *shMemBase );

void* RTRShmQueueGetLast(	rtrShmQueue *queue,
							char *shMemBase );

void* RTRShmQueueGetPrev(	rtrShmQueue *queue,
							rtrShmQueueElem *current,
							char *shMemBase );

void RTRShmQueueRemoveElem(	rtrShmQueue *queue,
							rtrShmQueueElem *current,
							char *shMemBase );

void RTRShmQueueAppend(		rtrShmQueue *toQ,
							rtrShmQueue *fromQ,
							char *shMemBase );

void RTRShmQueueDump(		rtrShmQueue *queue,
							char *shMemBase );

#ifdef __cplusplus
};
#endif

#endif
