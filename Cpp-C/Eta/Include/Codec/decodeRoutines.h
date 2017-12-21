#ifndef __decodeRoutines_h_
#define __decodeRoutines_h_

#include "rtr/rsslDataTypeEnums.h"
#include "rtr/rsslElementList.h"
#include "rtr/rsslFieldList.h"
#include "rtr/rsslMap.h"
#include "rtr/rsslArray.h"
#include "rtr/rsslVector.h"
#include "rtr/rsslFilterList.h"
#include "rtr/rsslSeries.h"
#include "rtr/rsslRDM.h"
#include "rtr/rsslDataDictionary.h"
#include "rtr/rsslMsg.h"
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

int decodeDataTypeToXML(FILE * file, RsslDataType dataType, RsslBuffer * buffer, const RsslDataDictionary * dictionary, void *setDb, RsslDecodeIterator *iter);
int decodeEncodedDataToXML(FILE * file, RsslDataType dataEncoding, const RsslBuffer * buffer, const RsslDataDictionary * dictionary, void *setDb);
int decodeSummaryData(FILE *file, RsslDecodeIterator *dIter, RsslContainerType containerType, const RsslBuffer * input, const RsslUInt8 majorVer, const RsslUInt8 minorVer, const RsslDataDictionary *dictionary, void *setDb);
int decodeElementListToXML(FILE * file, RsslDecodeIterator *iter, const RsslDataDictionary * dictionary, RsslLocalElementSetDefDb *setDb);
int decodeFieldListToXML(FILE * file, RsslDecodeIterator *iter, const RsslDataDictionary * dictionary, RsslLocalFieldSetDefDb *setDb);
int decodeFilterListToXML(FILE * file, RsslDecodeIterator *iter, const RsslDataDictionary * dictionary);
int decodeSeriesToXML(FILE * file, RsslDecodeIterator *iter, const RsslDataDictionary * dictionary);
int decodeAnsiPageToXML(FILE * file, const RsslBuffer * input, const RsslDataDictionary * dictionary);
int decodeJSONToXML(FILE * file, const RsslBuffer *input, const RsslDataDictionary * dictionary);
int dumpOpaqueToXML(FILE * file, const RsslBuffer * input, const RsslDataDictionary * dictionary);
int decodeVectorToXML(FILE * file, RsslDecodeIterator *iter, const RsslDataDictionary * dictionary);
int decodeMapToXML(FILE * file, RsslDecodeIterator *iter, const RsslDataDictionary * dictionary);
int decodeArrayToXML(FILE * file, RsslDecodeIterator *iter, const RsslDataDictionary * dictionary);
RsslRet decodeKeyOpaque(FILE * file, const RsslMsgKey * key, RsslDecodeIterator *iter, const RsslDataDictionary * dictionary);
RsslRet decodeKeysToXML(FILE * file, const RsslMsgKey * key, RsslDecodeIterator *iter, const RsslDataDictionary * dictionary);
RsslRet decodeMsgClassToXML(FILE * file, const RsslMsg * msg, RsslDecodeIterator *iter, const RsslDataDictionary * dictionary);
/* set version on the msg structure before calling this */
RsslRet decodeMsgToXML(FILE * file, const RsslMsg * msg, const RsslDataDictionary * dictionary, RsslDecodeIterator *iter);
RsslRet decodeNestedRwfMsgToXML(FILE * file, RsslDecodeIterator *iter, const RsslDataDictionary * dictionary);

const char* getStateCodeAsString( RsslUInt8 code );
const char* getInstrumentNameTypeAsString( RDMInstrumentNameTypes symbolType );
const char* getStreamStateAsString( RsslUInt8 code); 
const char* getDataStateAsString( RsslUInt8 code); 
const char* getNakCodeAsString(RsslUInt8 code);

#ifdef __cplusplus
}
#endif

#endif

