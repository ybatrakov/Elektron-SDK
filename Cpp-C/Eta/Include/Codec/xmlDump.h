#ifndef _xmlDump_h_
#define _xmlDump_h_

#include "rtr/rsslDataTypeEnums.h"
#include "rtr/rsslDataPackage.h"

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

void encodeindents(FILE * file);
void xmlDumpUInt(FILE *file, RsslUInt64 value);
void xmlDumpInt(FILE *file, RsslInt64 value);
void xmlDumpDouble(FILE *file, RsslDouble value);
void xmlDumpDate(FILE *file, RsslDate * value);
void xmlDumpTime(FILE *file, RsslTime * value);
void xmlDumpDateTime(FILE *file, RsslDateTime * value);

void xmlDumpStreamState(FILE *file, RsslUInt8 code);
void xmlDumpDataState(FILE *file, RsslUInt8 code);
void xmlDumpStateCode(FILE *file, RsslUInt8 code);


void xmlDumpQos(FILE * file, const RsslQos * qos);
void xmlDumpWorstQos(FILE * file, const RsslQos * qos);
void xmlDumpReal(FILE *file, RsslReal * value);
void xmlDumpString(FILE *file, int length, const char * data);
void xmlDumpBuffer(FILE *file, const RsslBuffer * buffer);
void xmlDumpGroupId(FILE *file, const RsslBuffer * buffer);
void xmlDumpHexBuffer(FILE *file, const RsslBuffer * buffer);

void xmlDumpFieldEncodingDef(FILE *file, int fieldId, int dataType);

void xmlDumpState(FILE *file, const RsslState *state);
void xmlDumpDataType(FILE *file, RsslDataType type);

void xmlDumpElementListBegin(FILE *file, RsslElementList *eList);
void xmlDumpElementBegin(FILE *file, RsslElementEntry *element);
void xmlDumpElementEnd(FILE *file);
void xmlDumpElementListEnd(FILE *file);


void xmlDumpFieldListBegin(FILE *file, RsslFieldList *fList);
void xmlDumpFieldBegin(FILE *file, RsslFieldEntry *field, RsslDataType dataType);
void xmlDumpFieldEnd(FILE *file);
void xmlDumpFieldListEnd(FILE *file);

void xmlDumpSummaryDataBegin(FILE *file);
void xmlDumpSummaryDataEnd(FILE *file);

void xmlDumpVectorBegin(FILE *file, RsslVector *vec);
void xmlDumpVectorEntryBegin(FILE *file, RsslVectorEntry *vEntry);
void xmlDumpVectorEntryEnd(FILE *file);
void xmlDumpVectorEnd(FILE *file);

void xmlDumpMapBegin(FILE *file, RsslMap *rsslMap);
void xmlDumpMapEntryBegin(FILE *file, RsslDataType keyPrimitiveType, RsslMapEntry *mEntry, void *pMapKeyData);
void xmlDumpMapEntryEnd(FILE *file);
void xmlDumpMapEnd(FILE *file);


void xmlDumpArrayBegin(FILE *file, RsslArray *rsslArray);
void xmlDumpArrayItemBegin(FILE *file);
void xmlDumpArrayItemEnd(FILE *file);
void xmlDumpArrayEnd(FILE *file);

void xmlDumpFilterListBegin(FILE *file, RsslFilterList *fList);
void xmlDumpFilterItemBegin(FILE *file, RsslFilterEntry *fItem);
void xmlDumpFilterItemEnd(FILE *file);
void xmlDumpFilterListEnd(FILE *file);

void xmlDumpSeriesBegin(FILE *file, RsslSeries *series);
void xmlDumpSeriesRowBegin(FILE *file, RsslSeriesEntry *row);
void xmlDumpSeriesRowEnd(FILE *file);
void xmlDumpSeriesEnd(FILE *file);

void xmlDumpLocalElementSetDefDb(FILE *file, RsslLocalElementSetDefDb *elListSetDb);
void xmlDumpLocalFieldSetDefDb(FILE *file, RsslLocalFieldSetDefDb *flListSetDb);

void xmlDumpEndNoTag(FILE *file);

void xmlDumpComment(FILE *file, const char* comment);
void xmlDumpTimestamp(FILE *file);
void xmlGetTimeFromEpoch(unsigned long long *hour, unsigned long long *min, unsigned long long*sec, unsigned long long *msec);

#ifdef __cplusplus
}
#endif


#endif
