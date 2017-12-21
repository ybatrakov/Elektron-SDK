#ifndef _fieldList_h_
#define _fieldList_h_

#include "rtr/rsslDataTypeEnums.h"
#include "rtr/rsslFieldList.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _fList
{
	RsslUInt16 id;
	RsslBuffer defs;
	struct _fList * next;
} FieldList;

void freeFieldLists(FieldList * fieldList);

FieldList * addFieldList(FieldList ** fieldList, RsslUInt16 id, const RsslBuffer * data);
FieldList * getFieldList(FieldList * fieldList, RsslUInt16 id);


#ifdef __cplusplus
}
#endif

#endif

