
#include "xmlDomainDump.h"
#include "xmlDump.h"
#include "decodeRoutines.h"

extern int indents;


void xmlDumpHeaderBegin(FILE * file, const char *tagName)
{
	encodeindents(file);
	fprintf(file, "<%s>\n", tagName);
	indents++;
}


void xmlDumpHeaderEnd(FILE * file, const char *tagName)
{
	indents--;
	encodeindents(file);
	fprintf(file, "</%s>", tagName);
	fprintf( file, "\n");
}

void xmlDumpDataBodyBegin(FILE * file)
{
	encodeindents(file);
	fprintf(file, "<dataBody>\n");
	indents++;
}

void xmlDumpDataBodyEnd(FILE * file)
{
	indents--;
	encodeindents(file);
	fprintf(file, "</dataBody>\n");
}
