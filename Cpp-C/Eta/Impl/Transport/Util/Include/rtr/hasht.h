/*|-----------------------------------------------------------------------------
 *|            This source code is provided under the Apache 2.0 license      --
 *|  and is provided AS IS with no warranty or guarantee of fit for purpose.  --
 *|                See the project's LICENSE.md for details.                  --
 *|           Copyright Thomson Reuters 2018. All rights reserved.            --
 *|-----------------------------------------------------------------------------
 */

//  description:
//		Hash Table - This file implements an intrusive hash table.
//			This means the link elements are part of the data structure
//			put into the hash table.
//
//	typedefs:
//		rtr_hash_sum	- Hash value.
//		rtr_hashtable	- Structure containing hash table information.
//		rtr_hashtable_link	- Structure containing link element for
//						- an element in the hash table.
//		rtr_hashtable_iter	- Structure containing hash table iteration
//						- information.
//		rtr_hashtable_key - Structure containing hash table key. Does
//						- not have to be used. Should be used if
//						- int_key_hashSum() is being used to calculate
//						- hash key.
//
//	defines:
//
//		void rtr_hasht_linkInit(rtr_hashtable *hTable, void * element)
//			- Initialize the rtr_hashtable_link in element used
//			- in hTable.
//
//		RTRBOOL rtr_hasht_empty(rtr_hashtable *hTable)
//			- Is the hash table empty?
//
//		RTRBOOL rtr_hasht_linkActive(rtr_hashtable *hTable, void * element)
//			- Is the rtr_hashtable_link in element for this hash
//			- table active (being used)?
//
//		int rtr_hasht_size(rtr_hashtable *hTable)
//			- The current number of elements stored in the hash table.
//
//		int rtr_hasht_capacity(rtr_hashtable *hTable)
//			- The number of buckets in the hash table.
//
//		RTRBOOL rtr_hasht_init(rtr_hashtable *hTable, int offset, int capacity
//								RTRBOOL(*compareFunc)(void* key,void* element),
//								rtr_hash_sum(*hashKeyFunc)(void*) );
//			- Initialize the rtr_hashtable with rtr_hashtable_link at
//			- an offset in the elements. The capacity is the number
//			- of buckets. The compareFunc is a function used to compare
//			- a key with an element to see if they match. And a hashKeyFunc
//			- is a function used to calculate a hash key.
//
//		void rtr_hasht_cleanup(rtr_hashtable *hTable)
//			- Cleanup the hash table.
//
//		void rtr_hasht_add(rtr_hashtable *hTable,void * key, void * record)
//			- Add an element (record) in the hash table with key.
//
//		void rtr_hasht_addfast(rtr_hashtable *hTable,void *key,rtr_hash_sum hashSum,
//								void *element)
//			- Add an element (record) in the hash table with key. The hashSum
//			- has already been calculate for key.
//
//		void rtr_hasht_remove(rtr_hashtable *hTable,void * key)
//			- Remove the element with key from the hash table.
//
//		void rtr_hasht_removefast(rtr_hashtable *hTable,void * key,rtr_hash_sum hashSum)
//			- Remove the element with key from the hash table. The hashSum
//			- has already been calculated for key.
//
//		void *rtr_hasht_find(rtr_hashtable *hTable,void * key)
//			- Find and return the element in the hash table with key.
//			- Returns 0 if not in hash table.
//
//		void *rtr_hasht_findfast(rtr_hashtable *hTable,void * key,rtr_hash_sum hashSum)
//			- Find and return the element in the hash table with key. The hashSum
//			- has already been calculate for key.
//			- Returns 0 if not in hash table.
//
//		void rtr_hasht_purge(rtr_hashtable *hTable,void (*purgeFunc)(void*,int), int purgeval)
//			- Remove all the elements from the hash table. If purgeFunc != 0
//			- then call purge func for each (element,purgeval) while they
//			- are being removed.
//
//
//	  Hash Key Calculation: Can be used as hashKeyFunc() in hash table.
//
//		rtr_hash_sum cstr_key_hashSum(void * key)
//			- This function calculates a hash sum given a null terminated string.
//			- key is a char*.
//
//		rtr_hash_sum int_key_hashSum(void * key)
//			- This function calculates a hash sum given a pointer to a integer.
//			- key is a int*.
//
//		rtr_hash_sum hasht_key_hashSum(void * key)
//			- This function calculates a hash sum given a rtr_hashtable_key.
//			- key is a rtr_hashtable_key*.
//
//		rtr_hash_sum hasht_mult_key_hashSum(void * key)
//			- This function calculates a hash sum of multiple keys.
//			- key is a pointer to an array of rtr_hashtable_key.
//			- The last element in the array has key->key == 0.
//
//
//	  Iteration:
//
//		void rtr_hashtiter_init(rtr_hashtable_iter *iter,rtr_hashtable *hTable)
//			- Initialize the hash table iterator with hTable.
//
//		void rtr_hashtiter_start(rtr_hashtable_iter *iter)
//			- Start the iteration.
//
//		void rtr_hashtiter_remove(rtr_hashtable_iter *iter)
//			- Remove the current element in iteration. rtr_hashtiter_item().
//			- Must use this function to remove an element while
//			- iterating.
//			- REQUIRE : !rtr_hashtiter_off(iter)
//
//		RTRBOOL rtr_hashtiter_off(rtr_hashtable_iter *iter)
//			- Are we at the end of the iteration?
//
//		void rtr_hashtiter_forth(rtr_hashtable_iter *iter)
//			- Move to the next item in the hash table.
//			- REQUIRE : !rtr_hashtiter_off(iter)
//
//		void *rtr_hashtiter_item(rtr_hashtable_iter *iter)
//			- Current item in the iteration.
//			- REQUIRE : !rtr_hashtiter_off(iter)
//
//
//	Expected Use:
//
//		typedef struct {
//			int					value;
//			rtr_hashtable_link	link1;
//			char				str[12];
//		} myStructure;
//
//		RTRBOOL checkDiff(void *key, void *element)
//		{
//			TESTSTRUCT  *temp=(TESTSTRUCT*)element;
//			return( ((strcmp((char*)temp->str,(char*)key)) == 0) ? RTRTRUE : RTRFALSE);
//		}
//
//		main()
//		{
//			rtr_hashtable	ht;
//			rtr_hashtable_iter	iter;
//			myStructure		*str;
//			int				i;
//
//			if (!rtr_hasht_init(&ht,sizeof(int),10,checkDiff,cstr_key_hashSum))
//			{
//				printf("Could not initialize hash table\n");
//				exit(-1);
//			}
//
//			if (rtr_hasht_empty(&ht))
//				printf("Hash table empty\n");
//
//			for (i=0; i<20;i++)
//			{
//				str = (myStructure*)malloc(sizeof(myStructure));
//				rtr_hasht_linkInit(&ht,str);
//				str->value = i;
//				sprintf(str->str,"Value_%d",i);
//				printf("Adding to hash table %s %d\n",str->str,str->num1);
//				rtr_hasht_add(&ht,str->str,str);
//			}
//
//			rtr_hashtiter_init(&iter,&ht);
//
//			for (	rtr_hashtiter_start(&iter);
//					!rtr_hashtiter_off(&iter);
//					rtr_hashtiter_forth(&iter) )
//			{
//				str = (TESTSTRUCT*)rtr_hashtiter_item(&iter);
//				printf("value %d  key %s\n",str->value,str->str);
//			}
//
//			str = (TESTSTRUCT*)rtr_hasht_find(&ht,"Value_10");
//			if (str)
//				printf("value %d  key %s\n",str->value,str->str);
//
//			rtr_hasht_remove(&ht,"Value_11");
//			rtr_hasht_remove(&ht,"Value_16");
//
//			for (	rtr_hashtiter_start(&iter);
//					!rtr_hashtiter_off(&iter);
//					rtr_hashtiter_forth(&iter) )
//			{
//				str = (TESTSTRUCT*)rtr_hashtiter_item(&iter);
//				printf("value %d  key %s\n",str->value,str->str);
//			}
//
//			rtr_hasht_purge(&ht,0,0);
//			rtr_hasht_cleanup(&ht);
//		}
//


#ifndef __rtr_hasht_h
#define __rtr_hasht_h

#include "rtr/intqtool.h"


typedef unsigned long	rtr_hash_sum;

typedef struct {
	rtr_int_qtool_list	*table;
	int					offset;
	int					buckets;	/* Number of buckets in hash table */
	int					size;		/* Current number of elements in hash table */
#if 1
	RTRBOOL				(*compare)(void*,void*);
	rtr_hash_sum		(*hashKey)(void*);
#else
	RTRBOOL				(*compare)();
	rtr_hash_sum		(*hashKey)();
#endif
} rtr_hashtable;

typedef rtr_int_qtool_head	rtr_hashtable_link;

typedef struct {
	rtr_hashtable		*htable;
	rtr_int_qtool_list	*col_list;
	rtr_int_qtool_list	*last_col_list;
	int					bucket;
	void				*current;
	void				*next;
	RTRBOOL				currentRemoved;
} rtr_hashtable_iter;


typedef struct {
	char	*key;
	int		length;
} rtr_hashtable_key;



	/* Initialize the rtr_qtool_head of an element */
#define rtr_hasht_linkInit(hTable,qEl) rtr_int_qtool_headInit((hTable)->offset,qEl)

	/* Is the rtr_qtool_head of an element active? */
#define rtr_hasht_linkActive(hTable,qEl) rtr_int_qtool_headActive((hTable)->offset,qEl)

#define rtr_hasht_empty(hTable) (((hTable)->size) ? RTRFALSE : RTRTRUE )

#define rtr_hasht_size(hTable)	((hTable)->size)
#define rtr_hasht_capacity(hTable)	((hTable)->buckets)


#if 1
extern RTRBOOL rtr_hasht_init(rtr_hashtable *hTable,int offset,int capacity,
							RTRBOOL(*)(void*,void*),rtr_hash_sum(*)(void*) );
extern void rtr_hasht_cleanup(rtr_hashtable *hTable);
extern void rtr_hasht_add(rtr_hashtable *hTable,void * key, void * record);
extern void rtr_hasht_addfast(rtr_hashtable *hTable,void * key,rtr_hash_sum hashSum, void * record);
extern void rtr_hasht_remove(rtr_hashtable *hTable,void * key);
extern void rtr_hasht_removefast(rtr_hashtable *hTable,void * key,rtr_hash_sum hashSum);
extern void *rtr_hasht_find(rtr_hashtable *hTable,void * key);
extern void *rtr_hasht_findfast(rtr_hashtable *hTable,void * key,rtr_hash_sum hashSum);
extern void rtr_hasht_purge(rtr_hashtable *hTable,void (*purgeFunc)(void*,int), int purgeval);

extern rtr_hash_sum cstr_key_hashSum(void * key);
extern rtr_hash_sum int_key_hashSum(void * key);
extern rtr_hash_sum hasht_key_hashSum(void * key);
extern rtr_hash_sum hasht_mult_key_hashSum(void * key);

extern void rtr_hashtiter_init(rtr_hashtable_iter*,rtr_hashtable*);
extern void rtr_hashtiter_start(rtr_hashtable_iter*);
extern void rtr_hashtiter_remove(rtr_hashtable_iter*);
extern void rtr_hashtiter_pos_next(rtr_hashtable_iter*);

#else
extern RTRBOOL rtr_hasht_init();
extern void rtr_hasht_cleanup();
extern void rtr_hasht_add();
extern void rtr_hasht_addfast();
extern void rtr_hasht_remove();
extern void rtr_hasht_removefast();
extern void *rtr_hasht_find();
extern void *rtr_hasht_findfast();
extern void rtr_hasht_purge();

extern rtr_hash_sum cstr_key_hashSum();
extern rtr_hash_sum int_key_hashSum();
extern rtr_hash_sum hasht_key_hashSum();
extern rtr_hash_sum hasht_mult_key_hashSum();

extern void rtr_hashtiter_init();
extern void rtr_hashtiter_start();
extern void rtr_hashtiter_remove();
extern void rtr_hashtiter_pos_next();

#endif


#define rtr_hashtiter_off(iter) (((iter)->current) ? RTRFALSE : RTRTRUE)
#define rtr_hashtiter_forth(iter) rtr_hashtiter_pos_next(iter)
#define rtr_hashtiter_item(iter) ((iter)->current)


#endif
