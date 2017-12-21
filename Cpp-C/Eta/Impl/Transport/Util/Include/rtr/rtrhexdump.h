/*|-----------------------------------------------------------------------------
 *|            This source code is provided under the Apache 2.0 license      --
 *|  and is provided AS IS with no warranty or guarantee of fit for purpose.  --
 *|                See the project's LICENSE.md for details.                  --
 *|           Copyright Thomson Reuters 2018. All rights reserved.            --
 *|-----------------------------------------------------------------------------
 */

#ifndef __RTR_HEX_DUMP_H
#define __RTR_HEX_DUMP_H

#include "rtr/os.h"
#include "rtr/rtrdefs.h"
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

	/** 
	 ** @brief Returns the size of the output when hex dumping a buffer.
	 **
	 ** @param iBuffer Pointer to the buffer to hex dump.
	 ** @param iBufferLength Length of the buffer to hex dump.
	 ** @param valuesPerLine Number of input buffer bytes per line of output.
	 ** @param charsPerLine Pointer to the characters per line needed. If non-zero pointer, it is filled in by the function.
	 **/
extern int rtr_dumpHexSize(
						const char	*iBuffer,
						int			iBufferLength,
						int			valuesPerLine,
						int			*charsPerLine );

	/** 
	 ** @brief Hex dump buffer contents to FILE pointer.
	 **
	 ** Typical Use:<BR>
	 ** Typical Use:<BR>
	 **  1. Call rtr_dumpHex(stdout, buf, buflen, 20)
	 ** @param oFptr Pointer to the FILE to write output.
	 ** @param iBuffer Pointer to the buffer to hex dump.
	 ** @param iBufferLength Length of the buffer to hex dump.
	 ** @param valuesPerLine Number of input buffer bytes per line of output.
	 */
extern void rtr_dumpHex(FILE		*oFptr,
						const char	*iBuffer,
						int			iBufferLength,
						int			valuesPerLine );

	/** 
	 ** @brief Hex dump buffer contents to FILE pointer.
	 **
	 ** Typical Use:<BR>
	 ** Typical Use:<BR>
	 **  1. Allocate output buffer (char outBuf[2048])<BR>
	 **  2. Call rtr_dumpHexToBuffer(outBuf, 2048, buf, buflen, 20)
	 ** @param oBuffer Pointer to the buffer to write hex dump output.
	 ** @param oBufferLength Maximum length of the output buffer.
	 ** @param iBuffer Pointer to the buffer to hex dump.
	 ** @param iBufferLength Length of the buffer to hex dump.
	 ** @param valuesPerLine Number of input buffer bytes per line of output.
	 ** @return if (>0) number of bytes used in oBuffer. (<0) number of bytes needed in output buffer.
	 */
extern long rtr_dumpHexToBuffer(
						char		*oBuffer,
						int			oBufferLength,
						const char	*iBuffer,
						int			iBufferLength,
						int			valuesPerLine );

#ifdef __cplusplus
};
#endif

#endif
