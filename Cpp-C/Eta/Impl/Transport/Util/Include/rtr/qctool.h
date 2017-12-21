/*|-----------------------------------------------------------------------------
 *|            This source code is provided under the Apache 2.0 license      --
 *|  and is provided AS IS with no warranty or guarantee of fit for purpose.  --
 *|                See the project's LICENSE.md for details.                  --
 *|           Copyright Thomson Reuters 2018. All rights reserved.            --
 *|-----------------------------------------------------------------------------
 */

//	keywords:
//		Counted Linked List, Linked List, List, Data Structure
//
//	typedefs:
//		rtr_qctool_head	- Structure containing link elements for
//						- an element in a list.
//		rtr_qctool_list	- Structure containing list information.
//
//	defines:
//		void rtr_qctool_init(rtr_qctool_list *qList,int offset)
//			- Initialize the qList using the rtr_qctool_head at
//			- an offset in the element structure.
//
//		RTRBOOL rtr_qctool_empty(rtr_qctool_list *qList)
//			- Is the qList emtpy?
//			- Returns : RTRBOOL
//
//		unsigned long rtr_qctool_count(rtr_qctool_list *qList)
//			- Return the current number of items in the list.
//
//		void rtr_qctool_headInit(rtr_qctool_list *qList,void *qEl)
//			- Initialize the rtr_qctool_head in the
//			- element (qEl) used to store the element in qList.
//
//		RTRBOOL rtr_qctool_headActive(rtr_qctool_list *qList,void *qEl)
//			- Is the rtr_qctool_head of qEl for qList active?
//			- Is qEl in qList?
//			- Returns : RTRBOOL
//
//		void *rtr_qctool_first(rtr_qctool_list *qList)
//			- Return the first element in qList; 0 if empty.
//
//		void *rtr_qctool_last(rtr_qctool_list *qList)
//			- Return the last element in qList; 0 if empty.
//
//		void *rtr_qctool_next(rtr_qctool_list *qList,void *qEl)
//			- Return the next element in the list (qList)
//			- after the element (qEl); 0 if empty.
//
//		void *rtr_qctool_prev(rtr_qctool_list *qList,void *qEl)
//			- Return the previous element in the list (qList)
//			- after the element (qEl); 0 if empty.
//
//		void rtr_qctool_insqe(rtr_qctool_list *qList, void *pPrevEntry,
//								void *pNewEntry)
//			- Insert the new entry (pNewEntry) into the list (qList)
//			- after the element (pPrevEntry).
//			- REQUIRE : rtr_qctool_headActive(qList,pPrevEntry)
//			- REQUIRE : !rtr_qctool_headActive(qList,pNewEntry)
//
//		void rtr_qctool_insqeb(rtr_qctool_list *qList, void *pNextEntry,
//								void *pNewEntry)
//			- Insert the new entry (pNewEntry) into the list (qList)
//			- before the element (pNextEntry).
//			- REQUIRE : rtr_qctool_headActive(qList,pNextEntry)
//			- REQUIRE : !rtr_qctool_headActive(qList,pNewEntry)
//
//		void rtr_qctool_insqh(rtr_qctool_list *qList, void *pNewEntry)
//			- Insert the new entry (pNewEntry) at the beginning
//			- of the list (qList).
//			- REQUIRE : !rtr_qctool_headActive(qList,pNewEntry)
//
//		void rtr_qctool_insqt(rtr_qctool_list *qList, void *pNewEntry)
//			- Insert the new entry (pNewEntry) at the end
//			- of the list (qList).
//			- REQUIRE : !rtr_qctool_headActive(qList,pNewEntry)
//
//		void rtr_qctool_remqe(rtr_qctool_list *qList, void *pRemEntry)
//			- Remove the element (pRemEntry) from the list (qList).
//			- REQUIRE : rtr_qctool_headActive(qList,pRemEntry)
//
//		void *rtr_qctool_remqh(rtr_qctool_list *qList)
//			- Remove the first element in the list (qList).
//			- Returns the element removed from the head of the list.
//			- REQUIRE : !rtr_qctool_empty(qList)
//
//		void *rtr_qctool_remqt(rtr_qctool_list *qList)
//			- Remove the last element in the list (qList).
//			- Returns the element removed from the tail of the list.
//			- REQUIRE : !rtr_qctool_empty(qList)
//
//		void rtr_qctool_append(rtr_qctool_list *qList, rtr_qctool_list *fromQList)
//			- Append the elements from `fromQList' to the end of `qList'.
//			- REQUIRE : qList->offset == fromQList->offset
//
//		void rtr_qctool_prepend(rtr_qctool_list *qList, rtr_qctool_list *fromQList)
//			- Prepend the elements from `fromQList' to the end of `qList'.
//			- REQUIRE : qList->offset == fromQList->offset
//
//	Expected Use:
//
//		typedef struct {
//			rtr_qctool_head	link1;
//			rtr_qctool_head	link2;
//			int				value;
//		} myStructure;
//
//		main()
//		{
//			rtr_qctool_list	list1;
//			rtr_qctool_list	list2;
//			myStructure		*str;
//			myStructure		*remem;
//			int				i;
//
//			rtr_qctool_init(&list1,0);
//			rtr_qctool_init(&list2,sizeof(rtr_qctool_head));
//
//			if (rtr_qctool_empty(&list1)) printf("List one empty\n");
//			if (rtr_qctool_empty(&list2)) printf("List two empty\n");
//
//			for (i=0; i<10;i++)
//			{
//				str = (myStructure*)malloc(sizeof(myStructure));
//				rtr_qctool_headInit(&list1,str);
//				rtr_qctool_headInit(&list2,str);
//				str->value = i;
//				rtr_qctool_insqh(&list1,str);
//				rtr_qctool_insqt(&list2,str);
//			}
//
//			for (	str = (myStructure*)rtr_qctool_first(&list1);
//					str != 0;
//					str = (myStructure*)rtr_qctool_next(&list1,str) )
//			{
//				printf("value %d\n",str->value);
//			}
//
//			rtr_qctool_remqh(&list1);
//			rtr_qctool_remqt(&list2);
//		}
//


#ifndef __rtr_qctool_h
#define __rtr_qctool_h

#include "rtr/intqtool.h"


typedef rtr_int_qtool_head rtr_qctool_head;

typedef struct {
	rtr_int_qtool_list	head;
	int					offset;
	unsigned int		count;
} rtr_qctool_list;



	/* Return pointer to rtr_qctool_head for this list */
#define rtr_qctool_RtoL(qList,qEl)\
		(rtr_qctool_head*)rtr_int_qtool_RtoL((qList)->offset,qEl)

	/* Initialize the rtr_qctool_head of an element */
#define rtr_qctool_headInit(qList,qEl) rtr_int_qtool_headInit((qList)->offset,qEl)

	/* Is the rtr_qctool_head of an element active? */
#define rtr_qctool_headActive(qList,qEl) rtr_int_qtool_headActive((qList)->offset,qEl)

	/* Return the current count of the list */
#define rtr_qctool_count(qList) ((qList)->count)

			/* qList - rtr_qctool_list *ptr - pointer to list. */
#define rtr_qctool_first(qList) rtr_int_qtool_first(&(qList)->head,(qList)->offset)

			/* qList - rtr_qctool_list *ptr - pointer to list. */
#define rtr_qctool_last(qList) rtr_int_qtool_last(&(qList)->head,(qList)->offset)

			/* qList - rtr_qctool_list *ptr - pointer to list.
			   qEl - void *ptr - Element of list */
#define rtr_qctool_next(qList,qEl) rtr_int_qtool_next(&(qList)->head,qEl,(qList)->offset)

			/* qList - rtr_qctool_list *ptr - pointer to list.
			   qEl - void *ptr - Element of list */
#define rtr_qctool_prev(qList,qEl) rtr_int_qtool_prev(&(qList)->head,qEl,(qList)->offset)

#define rtr_qctool_empty(qList) rtr_int_qtool_empty(&(qList)->head)


#define rtr_qctool_insqe(qList,pPrevEntry,pNewEntry) \
		rtr_int_qtool_insqe(&(qList)->head,(qList)->offset,pPrevEntry,pNewEntry); \
		(qList)->count++

#define rtr_qctool_insqeb(qList,pNextEntry,pNewEntry) \
		rtr_int_qtool_insqeb(&(qList)->head,(qList)->offset,pNextEntry,pNewEntry); \
		(qList)->count++

#define rtr_qctool_insqh(qList,pNewEntry) \
		rtr_int_qtool_insqh(&(qList)->head,(qList)->offset,pNewEntry); \
		(qList)->count++

#define rtr_qctool_insqt(qList,pNewEntry) \
		rtr_int_qtool_insqt(&(qList)->head,(qList)->offset,pNewEntry); \
		(qList)->count++

#define rtr_qctool_remqe(qList,pRemEntry) \
		rtr_int_qtool_remqe(&(qList)->head,(qList)->offset,pRemEntry); \
		(qList)->count--

#define rtr_qctool_remqh(qList) \
		rtr_int_qtool_remqh(&(qList)->head,(qList)->offset); \
		(qList)->count--

#define rtr_qctool_remqt(qList) \
		rtr_int_qtool_remqt(&(qList)->head,(qList)->offset); \
		(qList)->count--

#define rtr_qctool_append(qList,fromQList) \
		RTPRECONDITION((qList)->offset == (fromQList)->offset); \
		(qList)->count += (fromQList)->count; \
		rtr_int_qtool_append(&(qList)->head,&(fromQList)->head,(qList)->offset); \
		(fromQList)->count = 0

#define rtr_qctool_prepend(qList,fromQList) \
		RTPRECONDITION((qList)->offset == (fromQList)->offset); \
		(qList)->count += (fromQList)->count; \
		rtr_int_qtool_prepend(&(qList)->head,&(fromQList)->head,(qList)->offset); \
		(fromQList)->count = 0

#if 1
extern void rtr_qctool_init(rtr_qctool_list *qList,int offset);
#else
extern void rtr_qctool_init();
#endif

#endif
