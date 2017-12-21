/*|-----------------------------------------------------------------------------
 *|            This source code is provided under the Apache 2.0 license      --
 *|  and is provided AS IS with no warranty or guarantee of fit for purpose.  --
 *|                See the project's LICENSE.md for details.                  --
 *|           Copyright Thomson Reuters 2018. All rights reserved.            --
 *|-----------------------------------------------------------------------------
 */

//  description:
//		Linked List	- This file implements an intrusive double
//			linked list. This means the link elements are part of
//			the record put into the list. You can have one element
//			on multiple lists by having multiple linkes in the
//			element.
//
//	keywords:
//		Linked List, List, Data Structure
//
//	typedefs:
//		rtr_int_qtool_head	- Structure containing link elements for
//						- an element in a list.
//		rtr_int_qtool_list	- Structure containing list information.
//
//	defines:
//		void rtr_int_qtool_init(rtr_int_qtool_list *qList,int offset)
//			- Initialize the qList using the rtr_int_qtool_head at
//			- an offset in the element structure.
//
//		RTRBOOL rtr_int_qtool_empty(rtr_int_qtool_list *qList)
//			- Is the qList emtpy?
//			- Returns : RTRBOOL
//
//		unsigned long rtr_int_qtool_count(rtr_int_qtool_list *qList,int offset)
//			- Return the current number of elements in the list.
//
//		void rtr_int_qtool_headInit(int offset,void *qEl)
//			- Initialize the rtr_int_qtool_head in the
//			- element (qEl) used to store the element in qList.
//
//		RTRBOOL rtr_int_qtool_headActive(int offset,void *qEl)
//			- Is the rtr_int_qtool_head of qEl for qList active?
//			- Is qEl in qList?
//			- Returns : RTRBOOL
//
//		void *rtr_int_qtool_first(rtr_int_qtool_list *qList,int offset)
//			- Return the first element in qList; 0 if empty.
//
//		void *rtr_int_qtool_last(rtr_int_qtool_list *qList,int offset)
//			- Return the last element in qList; 0 if empty.
//
//		void *rtr_int_qtool_next(rtr_int_qtool_list *qList,void *qEl,int offset)
//			- Return the next element in the list (qList)
//			- after the element (qEl); 0 if empty.
//
//		void *rtr_int_qtool_prev(rtr_int_qtool_list *qList,void *qEl,int offset)
//			- Return the previous element in the list (qList)
//			- after the element (qEl); 0 if empty.
//
//		void rtr_int_qtool_insqe(rtr_int_qtool_list *qList, int offset, void *pPrevEntry,
//								void *pNewEntry)
//			- Insert the new entry (pNewEntry) into the list (qList)
//			- after the element (pPrevEntry).
//			- REQUIRE : rtr_int_qtool_headActive(offset,pPrevEntry)
//			- REQUIRE : !rtr_int_qtool_headActive(offset,pNewEntry)
//
//		void rtr_int_qtool_insqeb(rtr_int_qtool_list *qList, int offset, void *pNextEntry,
//								void *pNewEntry)
//			- Insert the new entry (pNewEntry) into the list (qList)
//			- before the element (pNextEntry).
//			- REQUIRE : rtr_int_qtool_headActive(offset,pNextEntry)
//			- REQUIRE : !rtr_int_qtool_headActive(offset,pNewEntry)
//
//		void rtr_int_qtool_insqh(rtr_int_qtool_list *qList, int offset, void *pNewEntry)
//			- Insert the new entry (pNewEntry) at the beginning
//			- of the list (qList).
//			- REQUIRE : !rtr_int_qtool_headActive(offset,pNewEntry)
//
//		void rtr_int_qtool_insqt(rtr_int_qtool_list *qList, int offset, void *pNewEntry)
//			- Insert the new entry (pNewEntry) at the end
//			- of the list (qList).
//			- REQUIRE : !rtr_int_qtool_headActive(offset,pNewEntry)
//
//		void rtr_int_qtool_remqe(rtr_int_qtool_list *qList, int offset, void *pRemEntry)
//			- Remove the element (pRemEntry) from the list (qList).
//			- REQUIRE : rtr_int_qtool_headActive(offset,pRemEntry)
//
//		void *rtr_int_qtool_remqh(rtr_int_qtool_list *qList, int offset)
//			- Remove the first element in the list (qList).
//			- Returns the element removed from the head of the list.
//			- REQUIRE : !rtr_int_qtool_empty(qList)
//
//		void *rtr_int_qtool_remqt(rtr_int_qtool_list *qList, int offset)
//			- Remove the last element in the list (qList).
//			- Returns the element removed from the tail of the list.
//			- REQUIRE : !rtr_int_qtool_empty(qList)
//
//		void *rtr_int_qtool_takeqh(rtr_int_qtool_list *qList, int offset)
//			- Remove the first element in the list (qList), if one exists.
//			- Returns the element removed from the head of the list, 0 otherwise.
//
//		void *rtr_int_qtool_takeqt(rtr_int_qtool_list *qList, int offset)
//			- Remove the last element in the list (qList), if one exists.
//			- Returns the element removed from the tail of the list, 0 otherwise.
//
//		void rtr_int_qtool_append(rtr_int_qtool_list *qList, rtr_int_qtool_list *fromQList,
//									int offset)
//			- Append the elements from `fromQList' to the end of `qList'.
//
//		void rtr_int_qtool_prepend(rtr_int_qtool_list *qList, rtr_int_qtool_list *fromQList,
//									int offset)
//			- Prepend the elements from `fromQList' to the end of `qList'.
//
//	Expected Use:
//
//		typedef struct {
//			rtr_int_qtool_head	link1;
//			rtr_int_qtool_head	link2;
//			int				value;
//		} myStructure;
//
//		main()
//		{
//			rtr_int_qtool_list	list1;
//			rtr_int_qtool_list	list2;
//			myStructure		*str;
//			myStructure		*remem;
//			int				i;
//
//			rtr_int_qtool_init(&list1,0);
//			rtr_int_qtool_init(&list2,sizeof(rtr_int_qtool_head));
//
//			if (rtr_int_qtool_empty(&list1)) printf("List one empty\n");
//			if (rtr_int_qtool_empty(&list2)) printf("List two empty\n");
//
//			for (i=0; i<10;i++)
//			{
//				str = (myStructure*)malloc(sizeof(myStructure));
//				rtr_int_qtool_headInit(0,str);
//				rtr_int_qtool_headInit(sizeof(rtr_int_qtool_head),str);
//				str->value = i;
//				rtr_int_qtool_insqh(&list1,str);
//				rtr_int_qtool_insqt(&list2,str);
//			}
//
//			for (	str = (myStructure*)rtr_int_qtool_first(&list1,0);
//					str != 0;
//					str = (myStructure*)rtr_int_qtool_next(&list1,str,0) )
//			{
//				printf("value %d\n",str->value);
//			}
//
//			rtr_int_qtool_remqh(&list1);
//			rtr_int_qtool_remqt(&list2);
//		}
//


#ifndef __rtr_int_qtool_h
#define __rtr_int_qtool_h

#include "rtr/rtrdefs.h"
#include "rtr/os.h"

typedef struct {
	char	*flink;
	char	*blink;
} rtr_int_qtool_head;

typedef rtr_int_qtool_head	rtr_int_qtool_list;



	/* Return pointer to rtr_int_qtool_head for this list */
#define rtr_int_qtool_RtoL(offset,qEl)\
				(rtr_int_qtool_head*)((char*)qEl + offset)

	/* Initialize the rtr_int_qtool_head of an element */
#define rtr_int_qtool_headInit(offset,qEl) \
	(rtr_int_qtool_RtoL(offset,qEl))->flink = 0; \
	(rtr_int_qtool_RtoL(offset,qEl))->blink = 0

	/* Is the rtr_int_qtool_head of an element active? */
#define rtr_int_qtool_headActive(offset,qEl) \
	(((rtr_int_qtool_RtoL(offset,qEl))->flink || (rtr_int_qtool_RtoL(offset,qEl))->blink) ? \
		RTRTRUE : RTRFALSE )

			/* qList - rtr_int_qtool_list *ptr - pointer to list. */
#define rtr_int_qtool_first(qList,offset) \
	( ( (qList)->flink == (char*)(qList)) ? 0 : \
			&(((char*)(qList)->flink)[-offset]) )

			/* qList - rtr_int_qtool_list *ptr - pointer to list. */
#define rtr_int_qtool_last(qList,offset) \
	( ( (qList)->blink == (char*)(qList) ) ? 0 : \
			&(((char*)(qList)->blink)[-offset]) )

			/* qList - rtr_int_qtool_list *ptr - pointer to list.
			   qEl - void *ptr - Element of list */
#define rtr_int_qtool_next(qList,qEl,offset) \
	( ( (rtr_int_qtool_RtoL(offset,qEl))->flink == (char*)(qList) ) ? 0 : \
			&(((char*)(rtr_int_qtool_RtoL(offset,qEl))->flink)[-offset]) )

			/* qList - rtr_int_qtool_list *ptr - pointer to list.
			   qEl - void *ptr - Element of list */
#define rtr_int_qtool_prev(qList,qEl,offset) \
	( ( (rtr_int_qtool_RtoL(offset,qEl))->blink == (char*)(qList) ) ? 0 : \
			&(((char*)(rtr_int_qtool_RtoL(offset,qEl))->blink)[-offset]) )

#define rtr_int_qtool_empty(qList) \
	( ( (qList)->flink == (char*)(qList) ) ? RTRTRUE : RTRFALSE )


#define rtr_entry_active(entry) \
	((((entry)->flink == 0) || ((entry)->blink == 0)) ? RTRFALSE : RTRTRUE)


RTR_C_ALWAYS_INLINE unsigned int rtr_int_qtool_count(rtr_int_qtool_list *qList,int offset)
{
	char *ptr;
	unsigned int count=0;

	for (	ptr=(char*)rtr_int_qtool_first(qList,offset);
			ptr != 0;
			ptr=(char*)rtr_int_qtool_next(qList,ptr,offset)
		)
	{
		count++;
	}
	return(count);
}


/***************************************************************************
* Function:		int_qtool_insqe()
* Description:	This function inserts a new entry after a specified entry
***************************************************************************/
RTR_C_ALWAYS_INLINE void rtr_int_qtool_insqe(rtr_int_qtool_list *qList,int offset, void *pPrevEntry, void *pNewEntry)
{
	rtr_int_qtool_head *prev = rtr_int_qtool_RtoL(offset,pPrevEntry);
	rtr_int_qtool_head *newE = rtr_int_qtool_RtoL(offset,pNewEntry);

	RTPRECONDITION(rtr_entry_active(prev));
	RTPRECONDITION(!rtr_entry_active(newE));

	newE->flink = prev->flink;
	newE->blink = (char*) prev;
	prev->flink = (char*) newE;
	((rtr_int_qtool_head*) (newE->flink))->blink = (char*) newE;
}


/***************************************************************************
* Function:		int_qtool_insqeb()
* Description:	This function inserts a new entry before a specified entry
***************************************************************************/
RTR_C_ALWAYS_INLINE void rtr_int_qtool_insqeb(rtr_int_qtool_list *qList,int offset, void *pNextEntry, void *pNewEntry)
{
	rtr_int_qtool_head *next = rtr_int_qtool_RtoL(offset,pNextEntry);
	rtr_int_qtool_head *newE = rtr_int_qtool_RtoL(offset,pNewEntry);

	RTPRECONDITION(rtr_entry_active(next));
	RTPRECONDITION(!rtr_entry_active(newE));

	newE->blink = next->blink;
	newE->flink = (char*) next;
	((rtr_int_qtool_head*) (newE->blink))->flink = (char*) newE;
	next->blink = (char*) newE;
}


/***************************************************************************
* Function:		int_qtool_insqh()
* Description:	This function inserts a new entry at the head of a queue
***************************************************************************/
RTR_C_ALWAYS_INLINE void rtr_int_qtool_insqh(rtr_int_qtool_list *qList,int offset,  void *pNewEntry)
{
	rtr_int_qtool_head *newE = rtr_int_qtool_RtoL(offset,pNewEntry);

	RTPRECONDITION(!rtr_entry_active(newE));

	newE->blink = (char*)qList;
	newE->flink = (char*)qList->flink;
	qList->flink = (char*) newE;
	((rtr_int_qtool_head*) (newE->flink))->blink = (char*) newE;
}


/***************************************************************************
* Function:		int_qtool_insqt()
* Description:	This function inserts a new entry to the tail of a queue
***************************************************************************/
RTR_C_ALWAYS_INLINE void rtr_int_qtool_insqt(rtr_int_qtool_list *qList,int offset,  void *pNewEntry)
{
	rtr_int_qtool_head *newE = rtr_int_qtool_RtoL(offset,pNewEntry);

	RTPRECONDITION(!rtr_entry_active(newE));

	newE->flink = (char*)qList;
	newE->blink = (char*)qList->blink;
	qList->blink = (char*) newE;
	((rtr_int_qtool_head*) (newE->blink))->flink = (char*) newE;
}


/***************************************************************************
* Function:		int_qtool_remqe()
* Description:	This function removes a specified entry from a queue
***************************************************************************/
RTR_C_ALWAYS_INLINE void rtr_int_qtool_remqe(rtr_int_qtool_list *qList,int offset, void *pRemEntry)
{
	rtr_int_qtool_head *remE = rtr_int_qtool_RtoL(offset,pRemEntry);

	RTPRECONDITION(!rtr_int_qtool_empty(qList));
	RTPRECONDITION(rtr_entry_active(remE));

	((rtr_int_qtool_head*) (remE->blink))->flink = remE->flink;
	((rtr_int_qtool_head*) (remE->flink))->blink = remE->blink;

	remE->blink = 0;
	remE->flink = 0;
}


/***************************************************************************
* Function:		int_qtool_remqh()
* Description:	This function removes an entry from the head of a queue
***************************************************************************/
RTR_C_ALWAYS_INLINE void *rtr_int_qtool_remqh(rtr_int_qtool_list *qList,int offset)
{
	rtr_int_qtool_head *ppEntry;

	RTPRECONDITION(!rtr_int_qtool_empty(qList));

	ppEntry = (rtr_int_qtool_head*) qList->flink;
	qList->flink = (ppEntry)->flink;
	((rtr_int_qtool_head*) ((ppEntry)->flink))->blink = (char*)qList;

	(ppEntry)->flink = 0;
	(ppEntry)->blink = 0;
	
	return ((void *)&(((char*)ppEntry)[-offset]));
}


/***************************************************************************
* Function:		int_qtool_remqt()
* Description:	This function removes an entry from the tail of a queue
***************************************************************************/
RTR_C_ALWAYS_INLINE void *rtr_int_qtool_remqt(rtr_int_qtool_list *qList,int offset )
{
	rtr_int_qtool_head *ppEntry;

	RTPRECONDITION(!rtr_int_qtool_empty(qList));

	ppEntry = (rtr_int_qtool_head*) qList->blink;
	((rtr_int_qtool_head*) ((ppEntry)->blink))->flink = (char*)qList;
	qList->blink = (ppEntry)->blink;

	(ppEntry)->flink = 0;
	(ppEntry)->blink = 0;
	
	return ((void *)&(((char*)ppEntry)[-offset]));
}

/***************************************************************************
* Function:		int_qtool_takeqh()
* Description:	This function removes an entry from the head of a queue,
*               if one exists, and returns that element.
***************************************************************************/
RTR_C_ALWAYS_INLINE void *rtr_int_qtool_takeqh(rtr_int_qtool_list *qList,int offset)
{
	rtr_int_qtool_head *ppEntry;

	if (rtrUnlikely(rtr_int_qtool_empty(qList)))
		return(0);

	ppEntry = (rtr_int_qtool_head*) qList->flink;
	qList->flink = (ppEntry)->flink;
	((rtr_int_qtool_head*) ((ppEntry)->flink))->blink = (char*)qList;

	(ppEntry)->flink = 0;
	(ppEntry)->blink = 0;
	
	return ((void *)&(((char*)ppEntry)[-offset]));
}


/***************************************************************************
* Function:		int_qtool_takeqt()
* Description:	This function removes an entry from the tail of a queue,
*				if one exists, and returns that element.
***************************************************************************/
RTR_C_ALWAYS_INLINE void *rtr_int_qtool_takeqt(rtr_int_qtool_list *qList,int offset )
{
	rtr_int_qtool_head *ppEntry;

	if (rtrUnlikely(rtr_int_qtool_empty(qList)))
		return(0);

	ppEntry = (rtr_int_qtool_head*) qList->blink;
	((rtr_int_qtool_head*) ((ppEntry)->blink))->flink = (char*)qList;
	qList->blink = (ppEntry)->blink;

	(ppEntry)->flink = 0;
	(ppEntry)->blink = 0;
	
	return ((void *)&(((char*)ppEntry)[-offset]));
}

RTR_C_ALWAYS_INLINE void rtr_int_qtool_append(rtr_int_qtool_list *qList, rtr_int_qtool_list *fromQList, int offset)
{
	if (!rtr_int_qtool_empty(fromQList))
	{
		rtr_int_qtool_head	*thisListLast = (rtr_int_qtool_head*) qList->blink;
		rtr_int_qtool_head	*othListFirst = (rtr_int_qtool_head*) fromQList->flink;
		rtr_int_qtool_head  *othListLast = (rtr_int_qtool_head*) fromQList->blink;

		othListLast->flink = (char*)qList;
		qList->blink = (char*)othListLast;

		thisListLast->flink = (char*)othListFirst;
		othListFirst->blink = (char*)thisListLast;

			/* Reset the other qlist to nothing */
		fromQList->flink = (char*)fromQList;
		fromQList->blink = (char*)fromQList;
	}
}

RTR_C_ALWAYS_INLINE void rtr_int_qtool_prepend(rtr_int_qtool_list *qList, rtr_int_qtool_list *fromQList, int offset)
{
	if (!rtr_int_qtool_empty(fromQList))
	{
		rtr_int_qtool_head	*thisListFirst = (rtr_int_qtool_head*) qList->flink;
		rtr_int_qtool_head	*othListFirst = (rtr_int_qtool_head*) fromQList->flink;
		rtr_int_qtool_head  *othListLast = (rtr_int_qtool_head*) fromQList->blink;

		othListFirst->blink = (char*)qList;
		qList->flink = (char*)othListFirst;

		othListLast->flink = (char*)thisListFirst;
		thisListFirst->blink = (char*)othListLast;

			/* Reset the other qlist to nothing */
		fromQList->flink = (char*)fromQList;
		fromQList->blink = (char*)fromQList;
	}
}

extern void rtr_int_qtool_init(rtr_int_qtool_list *qList);


#endif
