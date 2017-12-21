/*|-----------------------------------------------------------------------------
 *|            This source code is provided under the Apache 2.0 license      --
 *|  and is provided AS IS with no warranty or guarantee of fit for purpose.  --
 *|                See the project's LICENSE.md for details.                  --
 *|           Copyright Thomson Reuters 2018. All rights reserved.            --
 *|-----------------------------------------------------------------------------
 */

#include "rtr/hasht.h"
#include "rtr/primenum.h"
#include "rtr/platmem.h"


RTRBOOL rtr_hasht_init(rtr_hashtable *hTable,int offset,int capacity,
						RTRBOOL(*comp)(void*,void*),rtr_hash_sum(*hk)(void*) )
{
	int i;

	RTPRECONDITION(hTable != 0);
	RTPRECONDITION(capacity != 0);
	RTPRECONDITION(comp != 0);
	RTPRECONDITION(hk != 0);

	hTable->offset = offset;
	hTable->buckets = rtr_find_prime_num(capacity);
	hTable->size = 0;
	hTable->compare = comp;
	hTable->hashKey = hk;

	hTable->table = (rtr_int_qtool_list*)PLATFORM_MEMORY_ALLOC(sizeof(rtr_int_qtool_list) * hTable->buckets);
	if (hTable->table == 0)
		return(RTRFALSE);

	for (i=0; i< hTable->buckets; i++)
		rtr_int_qtool_init(&hTable->table[i]);

	return(RTRTRUE);
}

void rtr_hasht_cleanup(rtr_hashtable *hTable)
{
	RTPRECONDITION(hTable != 0);

	if (hTable->table)
		PLATFORM_MEMORY_DEALLOC(hTable->table);

	hTable->table = 0;
	hTable->offset = 0;
	hTable->buckets = 0;
	hTable->size = 0;
	hTable->compare = 0;
	hTable->hashKey = 0;
}

void rtr_hasht_purge(rtr_hashtable *hTable,void (*purgeFunc)(void*,int), int purgeval)
{
	rtr_hashtable_iter	iter;
	void				*cur;

	RTPRECONDITION(hTable != 0);

	rtr_hashtiter_init(&iter,hTable);

	for (	rtr_hashtiter_start(&iter);
			!rtr_hashtiter_off(&iter);
			rtr_hashtiter_forth(&iter))
	{
		cur = rtr_hashtiter_item(&iter);
		rtr_hashtiter_remove(&iter);
		if (purgeFunc)
			(*purgeFunc)(cur,purgeval);
	}
}

void rtr_hasht_add(rtr_hashtable *hTable,void * key, void * record)
{
	rtr_hash_sum ii;

	RTPRECONDITION(hTable != 0);
	RTPRECONDITION(key != 0);
	RTPRECONDITION(record != 0);
	RTPRECONDITION(!rtr_hasht_linkActive(hTable,record));
	RTPRECONDITION(rtr_hasht_find(hTable,key) == 0);

	ii = ((*hTable->hashKey)(key)) % hTable->buckets;
	rtr_int_qtool_insqt(&hTable->table[ii],hTable->offset,record);
	hTable->size++;
}

void rtr_hasht_addfast(rtr_hashtable *hTable,void * key,rtr_hash_sum hashSum, void * record)
{
	rtr_hash_sum ii;

	RTPRECONDITION(hTable != 0);
	RTPRECONDITION(key != 0);
	RTPRECONDITION(record != 0);
	RTPRECONDITION(!rtr_hasht_linkActive(hTable,record));
	RTPRECONDITION(rtr_hasht_find(hTable,key) == 0);

	ii = hashSum % hTable->buckets;
	rtr_int_qtool_insqt(&hTable->table[ii],hTable->offset,record);
	hTable->size++;
}

void rtr_hasht_remove(rtr_hashtable *hTable,void * key)
{
	rtr_hash_sum		ii;
	rtr_int_qtool_list	*qList;
	void				*element;

	RTPRECONDITION(hTable != 0);
	RTPRECONDITION(key != 0);

	ii = ((*hTable->hashKey)(key)) % hTable->buckets;
	qList = &hTable->table[ii];

	for (	element = (void*)rtr_int_qtool_first(qList,hTable->offset);
			element != 0;
			element = (void*)rtr_int_qtool_next(qList,element,hTable->offset) )
	{
		if ((*hTable->compare)(key,element))
			break;
	}

	if (element)
	{
		rtr_int_qtool_remqe(qList,hTable->offset,element);
		hTable->size--;
	}
}

void rtr_hasht_removefast(rtr_hashtable *hTable,void * key,rtr_hash_sum hashSum)
{
	rtr_hash_sum		ii;
	rtr_int_qtool_list	*qList;
	void				*element;

	RTPRECONDITION(hTable != 0);
	RTPRECONDITION(key != 0);

	ii = hashSum % hTable->buckets;
	qList = &hTable->table[ii];

	for (	element = (void*)rtr_int_qtool_first(qList,hTable->offset);
			element != 0;
			element = (void*)rtr_int_qtool_next(qList,element,hTable->offset) )
	{
		if ((*hTable->compare)(key,element))
			break;
	}

	if (element)
	{
		rtr_int_qtool_remqe(qList,hTable->offset,element);
		hTable->size--;
	}
}

void *rtr_hasht_find(rtr_hashtable *hTable,void * key)
{
	rtr_hash_sum		ii;
	rtr_int_qtool_list	*qList;
	void				*element;

	RTPRECONDITION(hTable != 0);
	RTPRECONDITION(key != 0);

	ii = ((*hTable->hashKey)(key)) % hTable->buckets;
	qList = &hTable->table[ii];

	for (	element = (void*)rtr_int_qtool_first(qList,hTable->offset);
			element != 0;
			element = (void*)rtr_int_qtool_next(qList,element,hTable->offset) )
	{
		if ((*hTable->compare)(key,element))
			return(element);
	}
	return(0);
}

void *rtr_hasht_findfast(rtr_hashtable *hTable,void * key,rtr_hash_sum hashSum)
{
	rtr_hash_sum		ii;
	rtr_int_qtool_list	*qList;
	void				*element;

	RTPRECONDITION(hTable != 0);
	RTPRECONDITION(key != 0);

	ii = hashSum % hTable->buckets;
	qList = &hTable->table[ii];

	for (	element = (void*)rtr_int_qtool_first(qList,hTable->offset);
			element != 0;
			element = (void*)rtr_int_qtool_next(qList,element,hTable->offset) )
	{
		if ((*hTable->compare)(key,element))
			return(element);
	}
	return(0);
}

rtr_hash_sum cstr_key_hashSum(void * key)
{
	char			*kkey=(char*)key;
	rtr_hash_sum	hash_sum=0;

	RTPRECONDITION(key != 0);

	while (*kkey)
	{
		hash_sum = (hash_sum << 4) + (unsigned long)*kkey++;
		hash_sum ^= (hash_sum >> 12);
	}
	return(hash_sum);
}

rtr_hash_sum int_key_hashSum(void * key)
{
	RTPRECONDITION(key != 0);
	return(*((rtr_hash_sum*)key));
}

rtr_hash_sum hasht_key_hashSum(void * key)
{
	rtr_hashtable_key	*kkey=(rtr_hashtable_key*)key;
	rtr_hash_sum		hash_sum=0;
	int					i=0;

	RTPRECONDITION(key != 0);

	while (i < kkey->length)
	{
		hash_sum = (hash_sum << 4) + (unsigned long)kkey->key[i++];
		hash_sum ^= (hash_sum >> 12);
	}
	return(hash_sum);
}

rtr_hash_sum hasht_mult_key_hashSum(void * key)
{
	rtr_hashtable_key	*kkey=(rtr_hashtable_key*)key;
	rtr_hash_sum		hash_sum=0;
	int					i=0;

	RTPRECONDITION(key != 0);

	for (;kkey->key; kkey++)
	{
		i=0;
		while (i < kkey->length)
		{
			hash_sum = (hash_sum << 4) + (unsigned long)kkey->key[i++];
			hash_sum ^= (hash_sum >> 12);
		}
	}
	return(hash_sum);
}



void rtr_hashtiter_init(rtr_hashtable_iter *iter,rtr_hashtable *hasht)
{
	RTPRECONDITION(iter != 0);
	RTPRECONDITION(hasht != 0);

	iter->htable = hasht;
	iter->col_list = 0;
	iter->last_col_list = 0;
	iter->bucket = 0;
	iter->current = 0;
	iter->next = 0;
	iter->currentRemoved = RTRFALSE;
}

void rtr_hashtiter_pos_next(rtr_hashtable_iter* iter)
{
	RTPRECONDITION(iter != 0);
	RTPRECONDITION(iter->htable != 0);

	if (iter->next)
	{
		iter->current = iter->next;
		iter->next = rtr_int_qtool_next(iter->col_list,iter->current,
										iter->htable->offset);
	}
	else
	{
		for (	iter->col_list++,iter->current = 0;
				iter->col_list <= iter->last_col_list;
				iter->col_list++ )
		{
			if ((iter->current = rtr_int_qtool_first(iter->col_list,
									iter->htable->offset)) != 0)
			{
				iter->next = rtr_int_qtool_next(iter->col_list,iter->current,
										iter->htable->offset);
				break;
			}
		}
	}
	iter->currentRemoved = RTRFALSE;
}

void rtr_hashtiter_start(rtr_hashtable_iter *iter)
{
	RTPRECONDITION(iter != 0);
	RTPRECONDITION(iter->htable != 0);

	iter->col_list = &iter->htable->table[0];
	iter->last_col_list =&(iter->htable->table[iter->htable->buckets-1]);
	iter->current = rtr_int_qtool_first(iter->col_list,iter->htable->offset);
	iter->next = 0;

	if (iter->current != 0)
		iter->next = rtr_int_qtool_next(iter->col_list,iter->current,
										iter->htable->offset);
	else
		rtr_hashtiter_pos_next(iter);

	iter->currentRemoved = RTRFALSE;
}


void rtr_hashtiter_remove(rtr_hashtable_iter *iter)
{
	RTPRECONDITION(iter != 0);
	RTPRECONDITION(iter->htable != 0);
	RTPRECONDITION(iter->col_list != 0);
	RTPRECONDITION(iter->current != 0);

	rtr_int_qtool_remqe(iter->col_list,iter->htable->offset,iter->current);
	iter->htable->size--;
	iter->currentRemoved = RTRTRUE;
}

