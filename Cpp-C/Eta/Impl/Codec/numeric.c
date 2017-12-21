/*|-----------------------------------------------------------------------------
 *|            This source code is provided under the Apache 2.0 license      --
 *|  and is provided AS IS with no warranty or guarantee of fit for purpose.  --
 *|                See the project's LICENSE.md for details.                  --
 *|           Copyright Thomson Reuters 2018. All rights reserved.            --
 *|-----------------------------------------------------------------------------
 */

#include "rtr/rsslDataPackage.h"
#include "rtr/rsslReal.h"
#include "rtr/rsslRetCodes.h"
#include "rtr/rwfConvert.h"
#include "rtr/encoderTools.h"

#include <stdlib.h>
#include <string.h>


#include <math.h>
#include <float.h>
#include <ctype.h>


#ifdef WIN32
	#ifndef INFINITY
		#define INFINITY HUGE_VAL
		#ifndef NEG_INFINITY
			#define NEG_INFINITY -HUGE_VAL
		#endif
	#else
		#ifndef NEG_INFINITY
			#define NEG_INFINITY -INFINITY		
		#endif
	#endif
	#ifndef NAN
        static const unsigned long __nan[2] = {0xffffffff, 0x7fffffff};
        #define NAN (*(const double *) __nan)
    #endif
#else
	#define NEG_INFINITY -INFINITY
#endif


static RsslDouble powHintsEx[] = {100000000000000.0, 10000000000000.0, 1000000000000.0, 100000000000.0, 10000000000.0, 1000000000.0, 100000000.0, 10000000.0, 1000000.0, 100000.0, 10000.0, 1000.0, 100.0, 10.0, 1.0, 0.1, 0.01, 0.001, 0.0001, 0.00001, 0.000001, 0.0000001, 1, 2, 4, 8, 16, 32, 64, 128, 256 };

static RsslDouble powHints[] = {0.00000000000001, 0.0000000000001, 0.000000000001, 0.00000000001, 0.0000000001, 0.000000001, 0.00000001, 0.0000001, 0.000001, 0.00001, 0.0001, 0.001, 0.01, 0.1, 1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 1, 0.5, 0.25, 0.125, 0.0625, 0.03125, 0.015625, 0.0078125, 0.00390625 };


RSSL_API RsslRet rsslDoubleToReal(RsslReal * oReal, RsslDouble * iValue, RsslUInt8 iHint)
{
	RSSL_ASSERT(oReal, Invalid parameters or parameters passed in as NULL);

	if (iHint > RSSL_RH_NOT_A_NUMBER || iHint == 31)
		return RSSL_RET_FAILURE;

	if (*iValue == INFINITY)
	{
		oReal->hint = RSSL_RH_INFINITY;
		oReal->isBlank = RSSL_FALSE;
		oReal->value = 0;		
	}
	else if (*iValue == NEG_INFINITY)
	{
		oReal->hint = RSSL_RH_NEG_INFINITY;
		oReal->isBlank = RSSL_FALSE;
		oReal->value = 0;		
	}
	else if (*iValue == NAN)
	{
		oReal->hint = RSSL_RH_NOT_A_NUMBER;
		oReal->isBlank = RSSL_FALSE;
		oReal->value = 0;		
	}
	else
	{
		oReal->hint = iHint;

		if (!iValue)
		{
			/* blank value */
			oReal->isBlank = RSSL_TRUE;
			oReal->value = 0;
			return RSSL_RET_SUCCESS;
		}

#if defined (WIN32) || defined (_WIN32)
		if (iHint < RSSL_RH_FRACTION_1)
		{
			oReal->value = floor(((*iValue) / (pow((double)10, (double)(iHint - RSSL_RH_EXPONENT0)))) + 0.5);
		}
		else
			oReal->value = (*iValue)*(pow((double)2, (double)(iHint - RSSL_RH_FRACTION_1)));	
#else
			oReal->value = floor((*iValue) * powHintsEx[iHint] + 0.5);
#endif

		oReal->isBlank = RSSL_FALSE;
	}
	return RSSL_RET_SUCCESS;
}
	
RSSL_API RsslRet rsslFloatToReal(RsslReal * oReal, RsslFloat * iValue, RsslUInt8 iHint)
{
	RSSL_ASSERT(oReal, Invalid parameters or parameters passed in as NULL);
	
	if (iHint > RSSL_RH_NOT_A_NUMBER || iHint == 31)
		return RSSL_RET_FAILURE;

	if ((double)(*iValue) == INFINITY)
	{
		oReal->hint = RSSL_RH_INFINITY;
		oReal->isBlank = RSSL_FALSE;
		oReal->value = 0;		
	}
	else if ((double)(*iValue) == NEG_INFINITY)
	{
		oReal->hint = RSSL_RH_NEG_INFINITY;
		oReal->isBlank = RSSL_FALSE;
		oReal->value = 0;		
	}
	else if ((double)(*iValue) == NAN)
	{
		oReal->hint = RSSL_RH_NOT_A_NUMBER;
		oReal->isBlank = RSSL_FALSE;
		oReal->value = 0;		
	}

	oReal->hint = iHint;

	if (!iValue)
	{
		/* blank value */
		oReal->isBlank = RSSL_TRUE;
		oReal->value = 0;
		return RSSL_RET_SUCCESS;
	}

#if defined (WIN32) || defined (_WIN32)
	if (iHint < RSSL_RH_FRACTION_1)
	{
		oReal->value = floor(((*iValue)/(pow((float)10, (float)(iHint - RSSL_RH_EXPONENT0)))) + 0.5);
	}
	else
		oReal->value = (*iValue)*(pow((float)2, (float)(iHint - RSSL_RH_FRACTION_1)));	
#else
		oReal->value = floor((*iValue) * powHintsEx[iHint] + 0.5);
#endif
	
	oReal->isBlank = RSSL_FALSE;
	return RSSL_RET_SUCCESS;
}


RSSL_API RsslRet rsslRealToString(RsslBuffer * buffer, RsslReal * iReal)
{
	char tbuf[64];
	char *ret;
	int length;

	RSSL_ASSERT(buffer, Invalid parameters or parameters passed in as NULL);
	if (!iReal)
	{
		buffer->length = 0;
		if (buffer->data)
			buffer->data[0] = '\0';
	}
		
	ret = rwfReal64tosOpts( tbuf, 64, iReal, 0 );
	if (ret == 0)
		return RSSL_RET_INVALID_DATA;

	length = snprintf(buffer->data, buffer->length, "%s", ret);
	if (length >= 0 && length < buffer->length)
	{
		buffer->length = length;
		return RSSL_RET_SUCCESS;
	}
	return RSSL_RET_FAILURE;
}


RSSL_API RsslRet rsslRealToDouble(RsslDouble * oValue, RsslReal * iReal)
{
	RSSL_ASSERT(iReal && oValue, Invalid parameters or parameters passed in as NULL);

	if (iReal->isBlank == RSSL_TRUE)
		return RSSL_RET_FAILURE;
	
	/* TODO: Better checking here if we change hints */
	switch(iReal->hint)
	{
		case RSSL_RH_NOT_A_NUMBER:
			*oValue = NAN;
			return RSSL_RET_SUCCESS;
		break;
		case RSSL_RH_INFINITY:
			*oValue = INFINITY;
			return RSSL_RET_SUCCESS;
		break;
		case RSSL_RH_NEG_INFINITY:
			*oValue = NEG_INFINITY;
			return RSSL_RET_SUCCESS;
		break;
	}

	if (iReal->hint > RSSL_RH_MAX_DIVISOR)
		return RSSL_RET_FAILURE;
#if defined (WIN32) || defined (_WIN32)
	if (iReal->hint < RSSL_RH_FRACTION_1)
	{
		*oValue = iReal->value*(pow((double)10, (double)(iReal->hint - RSSL_RH_EXPONENT0)));
	}
	else
		*oValue = iReal->value/(pow((double)2, (double)(iReal->hint - RSSL_RH_FRACTION_1)));	
#else
 	*oValue = (iReal->value*powHints[iReal->hint]);
#endif
	
	return RSSL_RET_SUCCESS;
}

RSSL_API RsslRet rsslNumericStringToDouble(RsslDouble * oValue, RsslBuffer * iNumericString)
{
	RsslRet		ret;
	RsslReal	real64;

	RSSL_ASSERT(iNumericString && oValue, Invalid parameters or parameters passed in as NULL);

	if (!iNumericString || !iNumericString->data)
	{
		*oValue = NAN;
		return RSSL_RET_BLANK_DATA;
	}

	ret = rwf_storeal64_size( 
						&real64,
						iNumericString->data,
						iNumericString->data + iNumericString->length);

	if (ret == RSSL_RET_BLANK_DATA)
	{
		*oValue = NAN;
		return (ret);
	}
	else if (ret != RSSL_RET_SUCCESS)
		return (ret);

	switch(real64.hint)
	{
		case RSSL_RH_NOT_A_NUMBER:
			*oValue = NAN;
			return RSSL_RET_SUCCESS;
		break;
		case RSSL_RH_INFINITY:
			*oValue = INFINITY;
			return RSSL_RET_SUCCESS;
		break;
		case RSSL_RH_NEG_INFINITY:
			*oValue = NEG_INFINITY;
			return RSSL_RET_SUCCESS;
		break;
	}

#if defined (WIN32) || defined (_WIN32)
	if (real64.hint < RSSL_RH_FRACTION_1)
	{
		*oValue = real64.value*(pow((double)10, (double)(real64.hint - RSSL_RH_EXPONENT0)));
	}
	else
		*oValue = real64.value/(pow((double)2, (double)(real64.hint - RSSL_RH_FRACTION_1)));
#else
	*oValue = (real64.value*powHints[real64.hint]);
#endif
	return RSSL_RET_SUCCESS;
}

RSSL_API RsslRet rsslNumericStringToReal(RsslReal * oReal, RsslBuffer * iNumericString)
{
	RSSL_ASSERT(oReal, Invalid parameters or parameters passed in as NULL);

	if (!iNumericString || !iNumericString->data)
	{
		oReal->value = 0;
		oReal->hint = 0;
		oReal->isBlank = RSSL_TRUE;

		return RSSL_RET_BLANK_DATA;
	}

	return rwf_storeal64_size(
						oReal,
						iNumericString->data,
						iNumericString->data + iNumericString->length);
}

