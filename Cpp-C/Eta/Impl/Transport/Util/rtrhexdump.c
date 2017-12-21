/*|-----------------------------------------------------------------------------
 *|            This source code is provided under the Apache 2.0 license      --
 *|  and is provided AS IS with no warranty or guarantee of fit for purpose.  --
 *|                See the project's LICENSE.md for details.                  --
 *|           Copyright Thomson Reuters 2018. All rights reserved.            --
 *|-----------------------------------------------------------------------------
 */
 
#include "rtr/rtrhexdump.h"

int rtr_dumpHexSize( 
		const char	*iBuffer,
		int			iBufferLength,
		int			valuesPerLine,
		int			*charsPerLine )
{
	int	charPerLine;
	int	bufferNeeded;

		/*          Ascii hex         +  Spaces Ascii Hex   + spaces + ascii out + null */
	charPerLine = (valuesPerLine * 2) + (valuesPerLine / 2) + 3 + valuesPerLine + 1;
	bufferNeeded = (((iBufferLength / valuesPerLine) + 1) * charPerLine);

	if (charsPerLine)
		*charsPerLine = charPerLine;

	return (bufferNeeded);
}

static void rtr_startNewLine(
		FILE	*oFptr,
		char	*hexbuf,
		char	*strbuf,
		int		cursor,
		int		vperline )
{
	short totCursor = (vperline * 2) + (vperline / 2);
	short curCursor = (cursor * 2) + (cursor / 2);

	fprintf(oFptr,"%s",hexbuf);
	while (curCursor++ < totCursor)
		fprintf(oFptr," ");
	fprintf(oFptr,"   %s\n",strbuf);
}

void rtr_dumpHex(
			FILE		*oFptr,
			const char	*iBuffer,
			int			iBufferLength,
			int			valuesPerLine )
{
	char			buf[256];
	char			buf1[256];
	char			*hexPtr;
	char			*charPtr;
	unsigned char	byte;
	int				position = 0;
	int				curbyte = 0;
	int				eobyte=0;

	if (valuesPerLine == 0)
		valuesPerLine = 20;
	else
	{
			/* Max is 70 due to length of buf and buf1 */
		if (valuesPerLine > 70)
			valuesPerLine = 70;
		else if (valuesPerLine & 0x01)
			valuesPerLine--;
	}

	hexPtr = buf;
	charPtr = buf1;
	while (position < iBufferLength)
	{
		byte = *iBuffer++;
		hexPtr += sprintf(hexPtr,(eobyte & 1 ? "%2.2x " : "%2.2x"),byte);
		*charPtr++ = ( byte >= ' ' && byte < 0x7f) ? byte : '.';
		eobyte ^= 1;
		position++;
		curbyte++;

		if ((position % valuesPerLine) == 0)
		{
			*hexPtr = *charPtr = '\0';
			rtr_startNewLine( oFptr, buf, buf1, curbyte, valuesPerLine);
			hexPtr = buf;
			charPtr = buf1;
			curbyte = 0;
			eobyte = 0;
		}
	}
	if ((position % valuesPerLine) != 0)
	{
		*hexPtr = *charPtr = '\0';
		rtr_startNewLine( oFptr, buf, buf1, curbyte, valuesPerLine);
		hexPtr = buf;
		charPtr = buf1;
		position = 0;
		curbyte = 0;
		eobyte = 0;
	}
}

static char * rtr_startNewLineBuffer(
		char	*oBufPtr,
		char	*hexbuf,
		char	*strbuf,
		int		cursor,
		int		vperline )
{
	short totCursor = (vperline * 2) + (vperline / 2);
	short curCursor = (cursor * 2) + (cursor / 2);

	oBufPtr += sprintf(oBufPtr,"%s",hexbuf);
	while (curCursor++ < totCursor)
		oBufPtr += sprintf(oBufPtr," ");
	oBufPtr += sprintf(oBufPtr,"   %s\n",strbuf);
	return (oBufPtr);
}

long rtr_dumpHexToBuffer(
		char		*oBuf,
		int			oBufLen,
		const char	*iBuffer,
		int			iBufferLength,
		int			valuesPerLine )
{
	char			buf[256];
	char			buf1[256];
	char			*hexPtr;
	char			*charPtr;
	char			*oBufPtr;
	unsigned char	byte;
	int				position = 0;
	int				curbyte = 0;
	int				eobyte=0;
	int				bytesOut=0;
	int	bufferNeeded;

	if (valuesPerLine == 0)
		valuesPerLine = 20;
	else
	{
			/* Max is 70 due to length of buf and buf1 */
		if (valuesPerLine > 70)
			valuesPerLine = 70;
		else if (valuesPerLine & 0x01)
			valuesPerLine--;
	}

	*oBuf = '\0';

	oBufPtr = oBuf;

	bufferNeeded = rtr_dumpHexSize( iBuffer, iBufferLength, valuesPerLine, 0);

		/* Check to make sure it will fit. */
	if (bufferNeeded > oBufLen)
		return (-bufferNeeded);

	hexPtr = buf;
	charPtr = buf1;
	while (position < iBufferLength)
	{
		byte = *iBuffer++;
		hexPtr += sprintf(hexPtr,(eobyte & 1 ? "%2.2x " : "%2.2x"),byte);
		*charPtr++ = ( byte >= ' ' && byte < 0x7f) ? byte : '.';
		eobyte ^= 1;
		position++;
		curbyte++;

		if ((position % valuesPerLine) == 0)
		{
			*hexPtr = *charPtr = '\0';
			oBufPtr = rtr_startNewLineBuffer( oBufPtr, buf, buf1, curbyte, valuesPerLine);
			hexPtr = buf;
			charPtr = buf1;
			curbyte = 0;
			eobyte = 0;
		}
		fflush(stdout);
	}
	if ((position % valuesPerLine) != 0)
	{
		*hexPtr = *charPtr = '\0';
		oBufPtr = rtr_startNewLineBuffer( oBufPtr, buf, buf1, curbyte, valuesPerLine);
		hexPtr = buf;
		charPtr = buf1;
		position = 0;
		curbyte = 0;
		eobyte = 0;
	}
	RTRASSERT((oBufPtr - oBuf) <= oBufLen );
	return (oBufPtr - oBuf);
}
	
