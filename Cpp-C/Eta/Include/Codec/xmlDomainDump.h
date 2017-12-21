#ifndef _xmlDomainDump_h_
#define _xmlDomainDump_h_

#include <stdio.h>


#ifdef __cplusplus
extern "C" {
#endif


void xmlDumpHeaderBegin(FILE * file, const char* tagName);
void xmlDumpHeaderEnd(FILE * file, const char *tagName);

void xmlDumpDataBodyBegin(FILE * file);
void xmlDumpDataBodyEnd(FILE * file);

#ifdef __cplusplus
}
#endif


#endif
