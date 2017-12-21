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
//			element. This list is more user friendly by encapsulating
//			list information in the list data structure so it
//			doesn't have to be passed into all calls.
//
//	keywords:
//		Linked List, List, Data Structure
//
//	typedefs:
//		rtr_qtool_head	- Structure containing link elements for
//						- an element in a list.
//		rtr_qtool_list	- Structure containing list information.
//
//	defines:
//		void rtr_qtool_init(rtr_qtool_list *qList,int offset)
//			- Initialize the qList using the rtr_qtool_head at
//			- an offset in the element structure.
//
//		RTRBOOL rtr_qtool_empty(rtr_qtool_list *qList)
//			- Is the qList emtpy?
//			- Returns : RTRBOOL
//
//		RTRBOOL rtr_qtool_count(rtr_qtool_list *qList)
//			- Return the current number of elements in the list.
//
//		void rtr_qtool_headInit(rtr_qtool_list *qList,void *qEl)
//			- Initialize the rtr_qtool_head in the
//			- element (qEl) used to store the element in qList.
//
//		RTRBOOL rtr_qtool_headActive(rtr_qtool_list *qList,void *qEl)
//			- Is the rtr_qtool_head of qEl for qList active?
//			- Is qEl in qList?
//			- Returns : RTRBOOL
//
//		void *rtr_qtool_first(rtr_qtool_list *qList)
//			- Return the first element in qList; 0 if empty.
//
//		void *rtr_qtool_last(rtr_qtool_list *qList)
//			- Return the last element in qList; 0 if empty.
//
//		void *rtr_qtool_next(rtr_qtool_list *qList,void *qEl)
//			- Return the next element in the list (qList)
//			- after the element (qEl); 0 if empty.
//
//		void *rtr_qtool_prev(rtr_qtool_list *qList,void *qEl)
//			- Return the previous element in the list (qList)
//			- after the element (qEl); 0 if empty.
//
//		void rtr_qtool_insqe(rtr_qtool_list *qList, void *pPrevEntry,
//								void *pNewEntry)
//			- Insert the new entry (pNewEntry) into the list (qList)
//			- after the element (pPrevEntry).
//			- REQUIRE : rtr_qtool_headActive(qList,pPrevEntry)
//			- REQUIRE : !rtr_qtool_headActive(qList,pNewEntry)
//
//		void rtr_qtool_insqeb(rtr_qtool_list *qList, void *pNextEntry,
//								void *pNewEntry)
//			- Insert the new entry (pNewEntry) into the list (qList)
//			- before the element (pNextEntry).
//			- REQUIRE : rtr_qtool_headActive(qList,pNextEntry)
//			- REQUIRE : !rtr_qtool_headActive(qList,pNewEntry)
//
//		void rtr_qtool_insqh(rtr_qtool_list *qList, void *pNewEntry)
//			- Insert the new entry (pNewEntry) at the beginning
//			- of the list (qList).
//			- REQUIRE : !rtr_qtool_headActive(qList,pNewEntry)
//
//		void rtr_qtool_insqt(rtr_qtool_list *qList, void *pNewEntry)
//			- Insert the new entry (pNewEntry) at the end
//			- of the list (qList).
//			- REQUIRE : !rtr_qtool_headActive(qList,pNewEntry)
//
//		void rtr_qtool_remqe(rtr_qtool_list *qList, void *pRemEntry)
//			- Remove the element (pRemEntry) from the list (qList).
//			- REQUIRE : rtr_qtool_headActive(qList,pRemEntry)
//
//		void *rtr_qtool_remqh(rtr_qtool_list *qList)
//			- Remove the first element in the list (qList).
//			- Returns the element removed from the head of the list.
//			- REQUIRE : !rtr_qtool_empty(qList)
//
//		void *rtr_qtool_remqt(rtr_qtool_list *qList)
//			- Remove the last element in the list (qList).
//			- Returns the element removed from the tail of the list.
//			- REQUIRE : !rtr_qtool_empty(qList)
//
//		void *rtr_qtool_takeqh(rtr_qtool_list *qList)
//			- Remove the first element in the list (qList), if one exists.
//			- Returns the element removed from the head of the list, 0 otherwise.
//
//		void *rtr_qtool_takeqt(rtr_qtool_list *qList)
//			- Remove the last element in the list (qList), if one exists.
//			- Returns the element removed from the tail of the list, 0 otherwise.
//
//		void rtr_qtool_append(rtr_qtool_list *qList, rtr_qtool_list *fromQList)
//			- Append the elements from `fromQList' to the end of `qList'.
//			- REQUIRE : qList->offset == fromQList->offset
//
//		void rtr_qtool_prepend(rtr_qtool_list *qList, rtr_qtool_list *fromQList)
//			- Prepend the elements from `fromQList' to the end of `qList'.
//			- REQUIRE : qList->offset == fromQList->offset
//
//	Expected Use:
//
//		typedef struct {
//			rtr_qtool_head	link1;
//			rtr_qtool_head	link2;
//			int				value;
//		} myStructure;
//
//		main()
//		{
//			rtr_qtool_list	list1;
//			rtr_qtool_list	list2;
//			myStructure		*str;
//			myStructure		*remem;
//			int				i;
//
//			rtr_qtool_init(&list1,0);
//			rtr_qtool_init(&list2,sizeof(rtr_qtool_head));
//
//			if (rtr_qtool_empty(&list1)) printf("List one empty\n");
//			if (rtr_qtool_empty(&list2)) printf("List two empty\n");
//
//			for (i=0; i<10;i++)
//			{
//				str = (myStructure*)malloc(sizeof(myStructure));
//				rtr_qtool_headInit(&list1,str);
//				rtr_qtool_headInit(&list2,str);
//				str->value = i;
//				rtr_qtool_insqh(&list1,str);
//				rtr_qtool_insqt(&list2,str);
//			}
//
//			for (	str = (myStructure*)rtr_qtool_first(&list1);
//					str != 0;
//					str = (myStructure*)rtr_qtool_next(&list1,str) )
//			{
//				printf("value %d\n",str->value);
//			}
//
//			rtr_qtool_remqh(&list1);
//			rtr_qtool_remqt(&list2);
//		}
//


#ifndef __rtr_qtool_h
#define __rtr_qtool_h

#include "rtr/intqtool.h"


typedef rtr_int_qtool_head rtr_qtool_head;

typedef struct {
	rtr_int_qtool_list	head;
	int					offset;
} rtr_qtool_list;



	/* Return pointer to rtr_qtool_head for this list */
#define rtr_qtool_RtoL(qList,qEl)\
		(rtr_qtool_head*)rtr_int_qtool_RtoL((qList)->offset,qEl)

	/* Initialize the rtr_qtool_head of an element */
#define rtr_qtool_headInit(qList,qEl) rtr_int_qtool_headInit((qList)->offset,qEl)

	/* Is the rtr_qtool_head of an element active? */
#define rtr_qtool_headActive(qList,qEl) rtr_int_qtool_headActive((qList)->offset,qEl)

	/* qList - rtr_qtool_list *ptr - pointer to list. */
#define rtr_qtool_first(qList) rtr_int_qtool_first(&(qList)->head,(qList)->offset)

	/* qList - rtr_qtool_list *ptr - pointer to list. */
#define rtr_qtool_last(qList) rtr_int_qtool_last(&(qList)->head,(qList)->offset)

	/* qList - rtr_qtool_list *ptr - pointer to list.
	   qEl - void *ptr - Element of list */
#define rtr_qtool_next(qList,qEl) rtr_int_qtool_next(&(qList)->head,qEl,(qList)->offset)

	/* qList - rtr_qtool_list *ptr - pointer to list.
	   qEl - void *ptr - Element of list */
#define rtr_qtool_prev(qList,qEl) rtr_int_qtool_prev(&(qList)->head,qEl,(qList)->offset)

#define rtr_qtool_empty(qList) rtr_int_qtool_empty(&(qList)->head)

#define rtr_qtool_count(qList) rtr_int_qtool_count(&(qList)->head,(qList)->offset)

#define rtr_qtool_insqe(qList,pPrevEntry,pNewEntry) \
		rtr_int_qtool_insqe(&(qList)->head,(qList)->offset,pPrevEntry,pNewEntry)

#define rtr_qtool_insqeb(qList,pNextEntry,pNewEntry) \
		rtr_int_qtool_insqeb(&(qList)->head,(qList)->offset,pNextEntry,pNewEntry)

#define rtr_qtool_insqh(qList,pNewEntry) \
		rtr_int_qtool_insqh(&(qList)->head,(qList)->offset,pNewEntry)

#define rtr_qtool_insqt(qList,pNewEntry) \
		rtr_int_qtool_insqt(&(qList)->head,(qList)->offset,pNewEntry)

#define rtr_qtool_remqe(qList,pRemEntry) \
		rtr_int_qtool_remqe(&(qList)->head,(qList)->offset,pRemEntry)

#define rtr_qtool_remqh(qList) \
		rtr_int_qtool_remqh(&(qList)->head,(qList)->offset)

#define rtr_qtool_remqt(qList) \
		rtr_int_qtool_remqt(&(qList)->head,(qList)->offset)

#define rtr_qtool_takeqh(qList) \
		rtr_int_qtool_takeqh(&(qList)->head,(qList)->offset)

#define rtr_qtool_takeqt(qList) \
		rtr_int_qtool_takeqt(&(qList)->head,(qList)->offset)

#define rtr_qtool_append(qList,fromQList) \
		RTPRECONDITION((qList)->offset == (fromQList)->offset); \
		rtr_int_qtool_append(&(qList)->head,&(fromQList)->head,(qList)->offset)

#define rtr_qtool_prepend(qList,fromQList) \
		RTPRECONDITION((qList)->offset == (fromQList)->offset); \
		rtr_int_qtool_prepend(&(qList)->head,&(fromQList)->head,(qList)->offset)

#if 1
extern void rtr_qtool_init(rtr_qtool_list *qList,int offset);
#else
extern void rtr_qtool_init();
#endif

#endif
