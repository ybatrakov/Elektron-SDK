/*|-----------------------------------------------------------------------------
 *|            This source code is provided under the Apache 2.0 license      --
 *|  and is provided AS IS with no warranty or guarantee of fit for purpose.  --
 *|                See the project's LICENSE.md for details.                  --
 *|           Copyright Thomson Reuters 2018. All rights reserved.            --
 *|-----------------------------------------------------------------------------
 */
 
#include "rtr/shmemqueue.h"
#include <stdio.h>


void RTRShmQueueInit( rtrShmQueue *queue, char *shMemBase, rtrUInt64 linkOffset )
{
	//Assert(RTR_SHM_PTR_VALID(queue));
	queue->head.prev = queue->head.next = RTR_SHM_MAKE_OFFSET(shMemBase,&queue->head);
	queue->linkOffset = linkOffset;
};

void RTRShmQueueElemInit( rtrShmQueueElem *element )
{
	element->prev = element->next = RTR_INVALID_OFFSET;
};

void RTRShmQueueInsertTail(	rtrShmQueue *queue, 
							rtrShmQueueElem *element,
							char *shMemBase )
{
	rtrShmQueueElem *prevPtr = RTR_SHM_MAKEQE_PTR(shMemBase,queue->head.prev);
	RTR_SHM_OFFSET elemOffset = RTR_SHM_MAKE_OFFSET(shMemBase,element);

//	Assert(RTR_SHM_PTR_VALID(queue));
//	Assert(RTR_SHM_PTR_VALID(element));

	element->next = prevPtr->next;
	element->prev = queue->head.prev;
	queue->head.prev = elemOffset;
	prevPtr->next = elemOffset;
	queue->count++;
};

void RTRShmQueueInsertHead(	rtrShmQueue *queue, 
							rtrShmQueueElem *element,
							char *shMemBase )
{
	rtrShmQueueElem *nextPtr = RTR_SHM_MAKEQE_PTR(shMemBase,queue->head.next);
	RTR_SHM_OFFSET elemOffset = RTR_SHM_MAKE_OFFSET(shMemBase,element);

//	Assert(RTR_SHM_PTR_VALID(queue));
//	Assert(RTR_SHM_PTR_VALID(element));

	element->prev = nextPtr->prev;
	element->next = queue->head.next;
	queue->head.next = elemOffset;
	nextPtr->prev = elemOffset;
	queue->count++;
};

void* RTRShmQueueRemoveTail(	rtrShmQueue *queue,
								char *shMemBase )
{
	rtrShmQueueElem *lastElemPtr = RTR_SHM_MAKEQE_PTR(shMemBase,queue->head.prev);
	rtrShmQueueElem *secLastElemPtr;

	if (lastElemPtr == &queue->head)	/* back to the queue head? */
		return 0;

	secLastElemPtr = RTR_SHM_MAKEQE_PTR(shMemBase,(lastElemPtr)->prev);
	secLastElemPtr->next = lastElemPtr->next;
	queue->head.prev = lastElemPtr->prev;
	queue->count--;

	lastElemPtr->next = lastElemPtr->prev = RTR_INVALID_OFFSET;

	return ((rtrShmQueueElem*)((RTR_SHM_OFFSET)lastElemPtr - queue->linkOffset));
	
};

void* RTRShmQueueRemoveHead(	rtrShmQueue *queue,
								char *shMemBase )
{
	rtrShmQueueElem *firstElemPtr = RTR_SHM_MAKEQE_PTR(shMemBase,queue->head.next);
	rtrShmQueueElem *secElemPtr;

	if (firstElemPtr == &queue->head)		/* back to the queue head? */
		return 0;

	secElemPtr = RTR_SHM_MAKEQE_PTR(shMemBase,firstElemPtr->next);
	queue->head.next = firstElemPtr->next;
	secElemPtr->prev = firstElemPtr->prev;
	queue->count--;

	firstElemPtr->next = firstElemPtr->prev = RTR_INVALID_OFFSET;

	return ((rtrShmQueueElem*)((RTR_SHM_OFFSET)firstElemPtr - queue->linkOffset));
	
};

void* RTRShmQueueGetFirst(	rtrShmQueue *queue,
							char *shMemBase )
{
	rtrShmQueueElem *firstElemPtr = RTR_SHM_MAKEQE_PTR(shMemBase,queue->head.next);
	return ( (firstElemPtr == &queue->head) ? 0 : (((rtrShmQueueElem*)((RTR_SHM_OFFSET)firstElemPtr - queue->linkOffset))) );
};

void* RTRShmQueueGetNext(	rtrShmQueue *queue,
							rtrShmQueueElem *current,
							char *shMemBase )
{
	rtrShmQueueElem *nextElemPtr = RTR_SHM_MAKEQE_PTR(shMemBase,current->next);
	return ( (nextElemPtr == &queue->head) ? 0 : 
		(((rtrShmQueueElem*)((RTR_SHM_OFFSET)nextElemPtr - queue->linkOffset))));
		
};

void* RTRShmQueueGetLast(	rtrShmQueue *queue,
							char *shMemBase )
{
	rtrShmQueueElem *lastElemPtr = RTR_SHM_MAKEQE_PTR(shMemBase,queue->head.prev);
	return ( (lastElemPtr == &queue->head) ? 0 : (((rtrShmQueueElem*)((RTR_SHM_OFFSET)lastElemPtr - queue->linkOffset))) );
};

void* RTRShmQueueGetPrev(	rtrShmQueue *queue,
							rtrShmQueueElem *current,
							char *shMemBase )
{
	rtrShmQueueElem *prevElemPtr = RTR_SHM_MAKEQE_PTR(shMemBase,current->prev);
	return ( (prevElemPtr == &queue->head) ? 0 : (((rtrShmQueueElem*)((RTR_SHM_OFFSET)prevElemPtr - queue->linkOffset))) );
};

void RTRShmQueueRemoveElem(	rtrShmQueue *queue,
							rtrShmQueueElem *current,
							char *shMemBase )
{
	rtrShmQueueElem *nextElemPtr = RTR_SHM_MAKEQE_PTR(shMemBase,current->next);
	rtrShmQueueElem *prevElemPtr = RTR_SHM_MAKEQE_PTR(shMemBase,current->prev);

	prevElemPtr->next = RTR_SHM_MAKE_OFFSET(shMemBase,nextElemPtr);
	nextElemPtr->prev = RTR_SHM_MAKE_OFFSET(shMemBase,prevElemPtr);

	current->next = current->prev = RTR_INVALID_OFFSET;
};

void RTRShmQueueAppend(		rtrShmQueue *toQ,
							rtrShmQueue *fromQ,
							char *shMemBase )
{
	rtrShmQueueElem *fromQFirst = RTR_SHM_MAKEQE_PTR(shMemBase,fromQ->head.next);
	rtrShmQueueElem *fromQLast;
	rtrShmQueueElem *toQLast;

	if (fromQFirst == &fromQ->head)		/* Empty */
		return;

	fromQLast = RTR_SHM_MAKEQE_PTR(shMemBase,fromQ->head.prev);
	toQLast = RTR_SHM_MAKEQE_PTR(shMemBase,toQ->head.prev);

	fromQLast->next = RTR_SHM_MAKE_OFFSET(shMemBase,&toQ->head);
	toQ->head.prev = RTR_SHM_MAKE_OFFSET(shMemBase,fromQLast);
	toQLast->next = RTR_SHM_MAKE_OFFSET(shMemBase,fromQFirst);
	fromQFirst->prev = RTR_SHM_MAKE_OFFSET(shMemBase,toQLast);

	toQ->count += fromQ->count;

	fromQ->count = 0;
	fromQ->head.next = fromQ->head.prev = RTR_SHM_MAKE_OFFSET(shMemBase,&fromQ->head);
	return;
};

void RTRShmQueueDump(	rtrShmQueue *queue,
						char *shMemBase )
{
	rtrShmQueueElem *element;
	int count = 0;

	printf("Queue %u: next %u prev %u\n", 
			RTR_SHM_MAKE_OFFSET(shMemBase,&queue->head),
			queue->head.next, queue->head.prev );

	element = RTR_SHM_MAKEQE_PTR(shMemBase,queue->head.next);
	while (element != &queue->head)
	{
		printf("  QueueElem %u: next %u prev %u\n",
				RTR_SHM_MAKE_OFFSET(shMemBase,element),
				element->next, element->prev );
		element = RTR_SHM_MAKEQE_PTR(shMemBase,element->next);
	}

	printf("\n");
};
