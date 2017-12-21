/*|-----------------------------------------------------------------------------
 *|            This source code is provided under the Apache 2.0 license      --
 *|  and is provided AS IS with no warranty or guarantee of fit for purpose.  --
 *|                See the project's LICENSE.md for details.                  --
 *|           Copyright Thomson Reuters 2018. All rights reserved.            --
 *|-----------------------------------------------------------------------------
 */

#include "rtr/rsslDataTypeEnums.h"
#include "rtr/rsslRetCodes.h"
#include "rtr/rwfConvert.h"
#include "rtr/rsslDataUtils.h"
#include <ctype.h>
#include <time.h>

#include "rtr/custmem.h"

static const char * months[12] = {"JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"};

int translateMonth(const char* monthStr)
{
	int i, month = 0;

	for  (i = 0; i < 12; i++)
	{
		if (!strcmp(months[i], monthStr))
		{
			month = i + 1;
			break;
		}
	}

	return month;
}

RSSL_API RsslBool _rsslIsLeapYear(RsslUInt16 year)
{
	if ( (year % 4 == 0) && 
		 ((year % 100 != 0) || (year % 400 == 0)) )
		return RSSL_TRUE;
	else
		return RSSL_FALSE;
}

RSSL_API RsslBool rsslDateIsValid(const RsslDate * iDate)
{
	RsslDate blankDate = RSSL_BLANK_DATE;
	if (rsslDateIsEqual(iDate, &blankDate))
		return RSSL_TRUE;

	/* month or date or year of 0 is valid because marketfeed can send it */

	switch (iDate->month)
	{
	case 0:
	case 1:
	case 3:
	case 5:
	case 7:
	case 8:
	case 10:
	case 12:
		if (iDate->day > 31)
			return RSSL_FALSE;
		break;
	case 4:
	case 6:
	case 9:
	case 11:
		if (iDate->day > 30)
			return RSSL_FALSE;
		break;
	case 2:
		if (iDate->day > 29)
			return RSSL_FALSE;
		else if ((iDate->day == 29) && !_rsslIsLeapYear(iDate->year))
			return RSSL_FALSE;
		break;
	default:
		return RSSL_FALSE;
	}

	return RSSL_TRUE;
}

RSSL_API RsslBool rsslTimeIsValid(const RsslTime * iTime)
{
	RsslTime blankTime = RSSL_BLANK_TIME;
		
	if (rsslTimeIsEqual(iTime, &blankTime))
		return RSSL_TRUE;

	/* make sure hours are between 0 and 23, minute/sec is between 0 and 59, and milli is between 0 and 999 */
	if ((iTime->hour != 255) && (iTime->hour > 23))
		return RSSL_FALSE;
	if ((iTime->minute != 255) && (iTime->minute > 59))
		return RSSL_FALSE;
	if ((iTime->second != 255) && (iTime->second > 60))
		return RSSL_FALSE;
	if ((iTime->millisecond != 65535) && (iTime->millisecond > 999))
		return RSSL_FALSE;
	if ((iTime->microsecond != 2047) && (iTime->microsecond > 999))
		return RSSL_FALSE;
	if ((iTime->nanosecond != 2047) && (iTime->nanosecond > 999))
		return RSSL_FALSE;
	
		if (iTime->nanosecond == 2047)
		{
			if (iTime->microsecond == 2047)
			{
				if (iTime->millisecond == 65535)
				{
					if (iTime->second == 255)
					{
						if (iTime->minute == 255)
							return RSSL_TRUE;
						else 
						{
							if (iTime->hour == 255)
								return RSSL_FALSE;
							else
								return RSSL_TRUE;
						}
					}
					else
					{
						if ((iTime->hour == 255) || (iTime->minute == 255))
							return RSSL_FALSE;
						else
							return RSSL_TRUE;					
					}
				}
				else
				{
					if ((iTime->hour == 255) || (iTime->minute == 255) || (iTime->second == 255))
						return RSSL_FALSE;
					else
						return RSSL_TRUE;
				}
			}
			else
			{
				if ((iTime->hour == 255) || (iTime->minute == 255) || (iTime->second == 255) || (iTime->millisecond == 65535))
					return RSSL_FALSE;
				else
					return RSSL_TRUE;
			}
		}
		else if ((iTime->hour == 255) || (iTime->minute == 255) || (iTime->second == 255) || (iTime->millisecond == 65535) || (iTime->microsecond == 2047))
			return RSSL_FALSE;

		return RSSL_TRUE;
}

RSSL_API RsslBool rsslDateTimeIsValid(const RsslDateTime * iDateTime)
{
	/* not checking for Blank datetime because the ValidDate and ValidTime check the individual parts */
	/* there is a slight risk here because date or time may be blank while the other part is not.  */

	return (rsslDateIsValid(&iDateTime->date) && rsslTimeIsValid(&iDateTime->time));
}

RSSL_API RsslRet rsslDateStringToDate(RsslDate * oDate, const RsslBuffer * iDateString)
{
	char * tmp;
	RsslUInt8 u8;
	RsslUInt16 u16;
	int a, b, c;
	char monthStr[32];

	if (iDateString->data == NULL || iDateString->length == 0)
	{
		oDate->day = 0;
		oDate->month = 0;
		oDate->year = 0;

		return RSSL_RET_BLANK_DATA;
	}

	if (sscanf(iDateString->data, "%d/%d/%d", &a, &b, &c) == 3)
	{
		// 1974/04/14
		//
		if (a > 255) // assume year here is greater than MAX UINT8
		{
			oDate->day = c;
			oDate->month = b;
			oDate->year = a;
		}
		// 04/14/74
		//
		else if (c < 100) // assume year here is less than 100, then add 1900
		{
			oDate->day = b;
			oDate->month = a;
			oDate->year = c + 1900;
		}
		// 04/14/1974
		//
		else
		{
			oDate->day = b;
			oDate->month = a;
			oDate->year = c;
		}
		return RSSL_RET_SUCCESS;
	}	

	if (sscanf(iDateString->data, "%d%d%d", &a, &b, &c) == 3)
	{
		if (a > 255) // assume year here is greater than MAX UINT
		{
			oDate->day = c;
			oDate->month = b;
			oDate->year = a;
		}
		else if (c < 100) // assume year here is less than 100, then add 1900
		{
			oDate->day = b;
			oDate->month = a;
			oDate->year = c + 1900;
		}
		else
		{
			oDate->day = b;
			oDate->month = a;
			oDate->year = c;
		}
		return RSSL_RET_SUCCESS;
	}

	if (isdigit(iDateString->data[3]))
	{
		if (sscanf(iDateString->data, "%d %d %d", &a, &b, &c) == 3)
		{
			if (a > 255) // assume year here is greater than MAX UINT
			{
				oDate->day = c;
				oDate->month = b;
				oDate->year = a;
			}
			else if (c < 100) // assume year here is less than 100, then add 1900
			{
				oDate->day = b;
				oDate->month = a;
				oDate->year = c + 1900;
			}
			else
			{
				oDate->day = b;
				oDate->month = a;
				oDate->year = c;
			}
			return RSSL_RET_SUCCESS;
		}
	}
	else if (isalpha(iDateString->data[3]))
	{
		if (sscanf(iDateString->data, "%d %3s %d", &a, monthStr, &c) == 3)
		{
			if (c < 100) // assume year here is less than 100, then add 1900
			{
				oDate->day = a;
				oDate->month = translateMonth(monthStr);
				oDate->year = c + 1900;
			}
			else
			{
				oDate->day = a;
				oDate->month = translateMonth(monthStr);
				oDate->year = c;
			}
			return RSSL_RET_SUCCESS;
		}
	}

	if (iDateString->length > 0)
	{
		tmp = iDateString->data;
		for (u8 = 0; isdigit(*tmp); tmp++)
			u8 = u8 * 10 + (*tmp - '0');
		oDate->day = u8;

		while (isspace(*tmp))
			tmp++; /* skip whitespace */
		
		if (tmp == iDateString->data + iDateString->length)
		{
			oDate->day = 0;
			oDate->month = 0;
			oDate->year = 0;

			return RSSL_RET_BLANK_DATA;
		}

		for (u8 = 1; u8 <= 12 && strncmp(months[u8-1], tmp, 3) ; u8++);
		if (u8 > 12)
        {
			return RSSL_RET_INVALID_DATA;
        }
		else
			oDate->month = u8;
		tmp += 3;

		tmp++; /* skip whitespace */

		for (u16 = 0; isdigit(*tmp); tmp++)
			u16 = u16 * 10 + (*tmp - '0');
		oDate->year = u16;
	}
	else
	{
		oDate->day = 0;
		oDate->month = 0;
		oDate->year = 0;

		return RSSL_RET_BLANK_DATA;
	}

	return RSSL_RET_SUCCESS;
}

RSSL_API RsslRet rsslTimeStringToTime(RsslTime * oTime, const RsslBuffer * iTimeString)
{
	char * tmp;
	RsslUInt8 u8;
	RsslUInt16 u16;
	int min = 0;
	int sec = 0;
	int hour = 0;
	int milli = 0;
	int micro = 0;
	int nano = 0;

	if (iTimeString->data == NULL || iTimeString->length == 0)
	{
		oTime->hour = 255;
		oTime->minute = 255;
		oTime->second = 255;
		oTime->millisecond = 65535;
		oTime->microsecond = 2047;
		oTime->nanosecond = 2047;

		return RSSL_RET_BLANK_DATA;
	}

	if (sscanf(iTimeString->data, "%d:%d:%d:%d:%d:%d", &hour, &min, &sec, &milli, &micro, & nano) >= 2)
	{
		oTime->hour = hour;
		oTime->minute = min;
		oTime->second = sec; 
		oTime->millisecond = milli;
		oTime->microsecond = micro;
		oTime->nanosecond = nano;

		return RSSL_RET_SUCCESS;
	}

	if (sscanf(iTimeString->data, "%d%d%d%d%d%d", &hour, &min, &sec, &milli, &micro, &nano) >= 2)
	{
		oTime->hour = hour;
		oTime->minute = min;
		oTime->second = sec;
		oTime->millisecond = milli;
		oTime->microsecond = micro;
		oTime->nanosecond = nano;

		return RSSL_RET_SUCCESS;
	}

	if (iTimeString->length > 0)
	{
		tmp = iTimeString->data;

		while (isspace(*tmp))
			tmp++; /* skip whitespace */
		
		/* if all whitespaces init to blank */
		if (tmp == iTimeString->data + iTimeString->length)
		{
			oTime->hour = 255;
			oTime->minute = 255;
			oTime->second = 255;
			oTime->millisecond = 65535;
			oTime->microsecond = 2047;
			oTime->nanosecond = 2047;

			return RSSL_RET_BLANK_DATA;
		}

		for (u8 = 0; isdigit(*tmp); tmp++)
			u8 = u8 * 10 + (*tmp - '0');
		oTime->hour = u8;
		if (*tmp != ':')
        {
			return RSSL_RET_INVALID_DATA;
        }
		else
			tmp++;

		for (u8 = 0; isdigit(*tmp); tmp++)
			u8 = u8 * 10 + (*tmp - '0');
		oTime->minute = u8;

		while (isspace(*tmp))
			tmp++;

		if ((tmp < iTimeString->data + iTimeString->length) && (*tmp == ':'))
		{
			tmp++;
			for (u8 = 0; isdigit(*tmp); tmp++)
				u8 = u8 * 10 + (*tmp - '0');
			oTime->second = u8;
		}

		while (isspace(*tmp))
			tmp++;

		if ((tmp < iTimeString->data + iTimeString->length) && (*tmp == ':'))
		{
			tmp++;
			for (u16 = 0; isdigit(*tmp); tmp++)
				u16 = u16 * 10 + (*tmp - '0');
			oTime->millisecond =u16;
		}

		while (isspace(*tmp))
			tmp++;

		if ((tmp < iTimeString->data + iTimeString->length) && (*tmp == ':'))
		{
			tmp++;
			for (u16 = 0; isdigit(*tmp); tmp++)
				u16 = u16 * 10 + (*tmp - '0');
			oTime->microsecond =u16;
		}

		while (isspace(*tmp))
			tmp++;

		if ((tmp < iTimeString->data + iTimeString->length) && (*tmp == ':'))
		{
			tmp++;
			for (u16 = 0; isdigit(*tmp); tmp++)
				u16 = u16 * 10 + (*tmp - '0');
			oTime->nanosecond =u16;
		}		
	}
	else
	{
		oTime->hour = 255;
		oTime->minute = 255;
		oTime->second = 255;
		oTime->millisecond = 65535;
		oTime->microsecond = 2047;
		oTime->nanosecond = 2047;

		return RSSL_RET_BLANK_DATA;
	}

	return RSSL_RET_SUCCESS;
}

RSSL_API RsslRet rsslDateTimeStringToDateTime(RsslDateTime *oDateTime, const RsslBuffer *iDateTimeString)
{
	int hour = 0;
	int minute = 0;
	int second = 0;
	int millisecond = 0;
	int micro = 0;
	int nano = 0;
	int a, b, c;
	char monthStr[32];

	if (iDateTimeString->data == NULL || iDateTimeString->length == 0)
	{
		oDateTime->date.day = 0;
		oDateTime->date.month = 0;
		oDateTime->date.year = 0;

		oDateTime->time.hour = 255;
		oDateTime->time.minute = 255;
		oDateTime->time.second = 255;
		oDateTime->time.millisecond = 65535;
		oDateTime->time.microsecond = 2047;
		oDateTime->time.nanosecond = 2047;
		
		return RSSL_RET_BLANK_DATA;
	}

	if (sscanf(iDateTimeString->data, "%d/%d/%d %d:%d:%d:%d:%d:%d", &a, &b, &c, &hour, &minute, &second, &millisecond, &micro, & nano) >= 5)
	{
		// 1974/04/14
		//
		if (a > 255) // assume year here is greater than MAX UINT8
		{
			oDateTime->date.day = c;
			oDateTime->date.month = b;
			oDateTime->date.year = a;

			oDateTime->time.hour = hour;
			oDateTime->time.minute = minute;
			oDateTime->time.second = second;
			oDateTime->time.millisecond = millisecond;
			oDateTime->time.microsecond = micro;
			oDateTime->time.nanosecond = nano;
		}
		// 04/14/74
		//
		else if (c < 100) // assume year here is less than 100, then add 1900
		{
			oDateTime->date.day = b;
			oDateTime->date.month = a;
			oDateTime->date.year = c + 1900;

			oDateTime->time.hour = hour;
			oDateTime->time.minute = minute;
			oDateTime->time.second = second;
			oDateTime->time.millisecond = millisecond;
			oDateTime->time.microsecond = micro;
			oDateTime->time.nanosecond = nano;
		}
		// 04/14/1974
		//
		else
		{
			oDateTime->date.day = b;
			oDateTime->date.month = a;
			oDateTime->date.year = c;

			oDateTime->time.hour = hour;
			oDateTime->time.minute = minute;
			oDateTime->time.second = second;
			oDateTime->time.millisecond = millisecond;
			oDateTime->time.microsecond = micro;
			oDateTime->time.nanosecond = nano;
		}
		return RSSL_RET_SUCCESS;
	}	

	if (isdigit(iDateTimeString->data[3]))
	{
		if (sscanf(iDateTimeString->data, "%d %d %d %d %d %d %d %d %d", &a, &b, &c, &hour, &minute, &second, &millisecond, &micro, &nano) >= 5)
		{
			if (a > 255) // assume year here is greater than MAX UINT8
			{
				oDateTime->date.day = c;
				oDateTime->date.month = b;
				oDateTime->date.year = a;

				oDateTime->time.hour = hour;
				oDateTime->time.minute = minute;
				oDateTime->time.second = second;
				oDateTime->time.millisecond = millisecond;
				oDateTime->time.microsecond = micro;
				oDateTime->time.nanosecond = nano;
			}
			else if (c < 100) // assume year here is less than 100, then add 1900
			{
				oDateTime->date.day = b;
				oDateTime->date.month = a;
				oDateTime->date.year = c + 1900;

				oDateTime->time.hour = hour;
				oDateTime->time.minute = minute;
				oDateTime->time.second = second;
				oDateTime->time.millisecond = millisecond;
				oDateTime->time.microsecond = micro;
				oDateTime->time.nanosecond = nano;
			}
			else
			{
				oDateTime->date.day = b;
				oDateTime->date.month = a;
				oDateTime->date.year = c;

				oDateTime->time.hour = hour;
				oDateTime->time.minute = minute;
				oDateTime->time.second = second;
				oDateTime->time.millisecond = millisecond;
				oDateTime->time.microsecond = micro;
				oDateTime->time.nanosecond = nano;
			}
			return RSSL_RET_SUCCESS;
		}	
	}
	else if (isalpha(iDateTimeString->data[3]))
	{
		if (sscanf(iDateTimeString->data, "%d %3s %d %d %d %d %d %d %d", &a, monthStr, &c, &hour, &minute, &second, &millisecond, &micro, &nano) >= 5)
		{
			if (c < 100) // assume year here is less than 100, then add 1900
			{
				oDateTime->date.day = a;
				oDateTime->date.month = translateMonth(monthStr);
				oDateTime->date.year = c + 1900;

				oDateTime->time.hour = hour;
				oDateTime->time.minute = minute;
				oDateTime->time.second = second;
				oDateTime->time.millisecond = millisecond;
				oDateTime->time.microsecond = micro;
				oDateTime->time.nanosecond = nano;
			}
			else
			{
				oDateTime->date.day = a;
				oDateTime->date.month = translateMonth(monthStr);
				oDateTime->date.year = c;

				oDateTime->time.hour = hour;
				oDateTime->time.minute = minute;
				oDateTime->time.second = second;
				oDateTime->time.millisecond = millisecond;
				oDateTime->time.microsecond = micro;
				oDateTime->time.nanosecond = nano;
			}
			return RSSL_RET_SUCCESS;
		}
	}

	if (sscanf(iDateTimeString->data, "%d/%d/%d %d %d %d %d %d %d", &a, &b, &c, &hour, &minute, &second, &millisecond, &micro, &nano) >= 5)
	{
		// 1974/04/14
		//
		if (a > 255) // assume year here is greater than MAX UINT8
		{
			oDateTime->date.day = c;
			oDateTime->date.month = b;
			oDateTime->date.year = a;

			oDateTime->time.hour = hour;
			oDateTime->time.minute = minute;
			oDateTime->time.second = second;
			oDateTime->time.millisecond = millisecond;
			oDateTime->time.microsecond = micro;
			oDateTime->time.nanosecond = nano;
		}
		// 04/14/74
		//
		else if (c < 100) // assume year here is less than 100, then add 1900
		{
			oDateTime->date.day = b;
			oDateTime->date.month = a;
			oDateTime->date.year = c + 1900;

			oDateTime->time.hour = hour;
			oDateTime->time.minute = minute;
			oDateTime->time.second = second;
			oDateTime->time.millisecond = millisecond;
			oDateTime->time.microsecond = micro;
			oDateTime->time.nanosecond = nano;
		}
		// 04/14/1974
		//
		else
		{
			oDateTime->date.day = b;
			oDateTime->date.month = a;
			oDateTime->date.year = c;

			oDateTime->time.hour = hour;
			oDateTime->time.minute = minute;
			oDateTime->time.second = second;
			oDateTime->time.millisecond = millisecond;
			oDateTime->time.microsecond = micro;
			oDateTime->time.nanosecond = nano;
		}
		return RSSL_RET_SUCCESS;
	}	

	if (isdigit(iDateTimeString->data[3]))
	{
		if (sscanf(iDateTimeString->data, "%d %d %d %d:%d:%d:%d:%d:%d", &a, &b, &c, &hour, &minute, &second, &millisecond, &micro, &nano) >= 5)
		{
			if (a > 255) // assume year here is greater than MAX UINT8
			{
				oDateTime->date.day = c;
				oDateTime->date.month = b;
				oDateTime->date.year = a;

				oDateTime->time.hour = hour;
				oDateTime->time.minute = minute;
				oDateTime->time.second = second;
				oDateTime->time.millisecond = millisecond;
				oDateTime->time.microsecond = micro;
				oDateTime->time.nanosecond = nano;
			}
			else if (c < 100) // assume year here is less than 100, then add 1900
			{
				oDateTime->date.day = b;
				oDateTime->date.month = a;
				oDateTime->date.year = c + 1900;

				oDateTime->time.hour = hour;
				oDateTime->time.minute = minute;
				oDateTime->time.second = second;
				oDateTime->time.millisecond = millisecond;
				oDateTime->time.microsecond = micro;
				oDateTime->time.nanosecond = nano;
			}
			else
			{
				oDateTime->date.day = b;
				oDateTime->date.month = a;
				oDateTime->date.year = c;

				oDateTime->time.hour = hour;
				oDateTime->time.minute = minute;
				oDateTime->time.second = second;
				oDateTime->time.millisecond = millisecond;
				oDateTime->time.microsecond = micro;
				oDateTime->time.nanosecond = nano;
			}
			return RSSL_RET_SUCCESS;
		}	
	}
	else if (isalpha(iDateTimeString->data[3]))
	{
		if (sscanf(iDateTimeString->data, "%d %3s %d %d:%d:%d:%d:%d:%d", &a, monthStr, &c, &hour, &minute, &second, &millisecond, &micro, &nano) >= 5)
		{
			if (c < 100) // assume year here is less than 100, then add 1900
			{
				oDateTime->date.day = a;
				oDateTime->date.month = translateMonth(monthStr);
				oDateTime->date.year = c + 1900;

				oDateTime->time.hour = hour;
				oDateTime->time.minute = minute;
				oDateTime->time.second = second;
				oDateTime->time.millisecond = millisecond;
				oDateTime->time.microsecond = micro;
				oDateTime->time.nanosecond = nano;
			}
			else
			{
				oDateTime->date.day = a;
				oDateTime->date.month = translateMonth(monthStr);
				oDateTime->date.year = c;

				oDateTime->time.hour = hour;
				oDateTime->time.minute = minute;
				oDateTime->time.second = second;
				oDateTime->time.millisecond = millisecond;
				oDateTime->time.microsecond = micro;
				oDateTime->time.nanosecond = nano;
			}
			return RSSL_RET_SUCCESS;
		}
	}

	if (iDateTimeString->length > 0)
	{
		char* tmp = iDateTimeString->data;

		while (isspace(*tmp))
			tmp++; /* skip whitespace */
		
		/* if all whitespaces init to blank */
		if (tmp == iDateTimeString->data + iDateTimeString->length)
		{
			oDateTime->date.day = 0;
			oDateTime->date.month = 0;
			oDateTime->date.year = 0;

			oDateTime->time.hour = 255;
			oDateTime->time.minute = 255;
			oDateTime->time.second = 255;
			oDateTime->time.millisecond = 65535;
			oDateTime->time.microsecond = 2047;
			oDateTime->time.nanosecond = 2047;

			return RSSL_RET_BLANK_DATA;
		}

		if (rwf_stodatetime(oDateTime, iDateTimeString->data) < 0)
		{
			return RSSL_RET_INVALID_DATA;
		}
	}

	return RSSL_RET_SUCCESS;
}

RSSL_API RsslRet rsslDateTimeGmtTime(RsslDateTime * oDateTime)
{
	time_t now;
	struct tm t;
	time(&now);
#if defined(_WIN32) || defined(WIN32)
	t = *gmtime( &now );
#else
	t = *gmtime_r( &now, &t);
#endif
	oDateTime->date.day = t.tm_mday;
	oDateTime->date.month = t.tm_mon + 1;
	oDateTime->date.year = 1900 + t.tm_year;
	oDateTime->time.hour = t.tm_hour;
	oDateTime->time.minute = t.tm_min;
	oDateTime->time.second = t.tm_sec;
	oDateTime->time.millisecond = 0;
	oDateTime->time.microsecond = 0;
	oDateTime->time.nanosecond = 0;

	return RSSL_RET_SUCCESS;
}

RSSL_API RsslRet rsslDateTimeLocalTime(RsslDateTime * oDateTime)
{
	time_t now;
	struct tm t;
	time(&now);
#if defined(_WIN32) || defined(WIN32)
	t = *localtime( &now );
#else
	t = *localtime_r( &now, &t);
#endif
	oDateTime->date.day = t.tm_mday;
	oDateTime->date.month = t.tm_mon + 1;
	oDateTime->date.year = 1900 + t.tm_year;
	oDateTime->time.hour = t.tm_hour;
	oDateTime->time.minute = t.tm_min;
	oDateTime->time.second = t.tm_sec;
	oDateTime->time.millisecond = 0;
	oDateTime->time.microsecond = 0;
	oDateTime->time.nanosecond = 0;

	return RSSL_RET_SUCCESS;
}

RSSL_API RsslRet rsslDateTimeToString(RsslBuffer * oBuffer, RsslDataType type, RsslDateTime * iDateTime)
{
	RsslDateTime blankdatetime = RSSL_BLANK_DATETIME;
	RsslRet ret = 0;
	int remainingLength = (int)oBuffer->length;
	int bufOffset = 0;
	
	switch(type)
	{
		case RSSL_DT_DATETIME:
			/* check for blank date and time */
			if (iDateTime->date.day == 0 && iDateTime->date.month == 0 && iDateTime->date.year == 0 &&
				iDateTime->time.hour == 255 && iDateTime->time.minute == 255 && iDateTime->time.second == 255 && 
				iDateTime->time.millisecond == 65535 && iDateTime->time.microsecond == 2047 && iDateTime->time.nanosecond == 2047)
			{
				snprintf(oBuffer->data, oBuffer->length, "");
				oBuffer->length = 0;
				return RSSL_RET_SUCCESS;
			}

		/* fall through */
		case RSSL_DT_DATE:
		{	
			if (iDateTime->date.day == 0 && iDateTime->date.month == 0 && iDateTime->date.year == 0)
			{
				/* blank date */
				/* if we only have a date return 0 */
				snprintf(oBuffer->data, oBuffer->length, "");
				oBuffer->length = 0;
				if (type == RSSL_DT_DATE)
					return RSSL_RET_SUCCESS;		
			}
			else if (!rsslDateIsValid(&iDateTime->date))
			{
				ret = snprintf(oBuffer->data, remainingLength, "Invalid date");
				if (ret < 0 || ret >= remainingLength)
				{
					return RSSL_RET_FAILURE;
				}
				bufOffset += ret;
				remainingLength -= ret;
			}
			else
			{
				/* normal date */
				/* put this into the same format as marketfeed uses where if any portion is blank, it is 
				   represented as spaces */
				if (iDateTime->date.day)
				{
					ret = snprintf(oBuffer->data, remainingLength, "%02d ", iDateTime->date.day);
					if (ret < 0 || ret >= remainingLength)
					{
						return RSSL_RET_FAILURE;
					}
					bufOffset += ret;
					remainingLength -= ret;
				}
				else
				{
					ret = snprintf(oBuffer->data, remainingLength, "   ");
					if (ret < 0 || ret >= remainingLength)
					{
						return RSSL_RET_FAILURE;
					}
					bufOffset += ret;
					remainingLength -= ret;
				}
				
				if (iDateTime->date.month)
				{
					ret = snprintf((oBuffer->data + bufOffset), remainingLength, "%s ", months[iDateTime->date.month - 1]);
					if (ret < 0 || ret >= remainingLength)
					{
						return RSSL_RET_FAILURE;
					}
					bufOffset += ret;
					remainingLength -= ret;
				}
				else
				{
					ret = snprintf((oBuffer->data + bufOffset), remainingLength, "    ");
					if (ret < 0 || ret >= remainingLength)
					{
						return RSSL_RET_FAILURE;
					}
					bufOffset += ret;
					remainingLength -= ret;
				}

				if (iDateTime->date.year)
				{
					ret = snprintf((oBuffer->data + bufOffset), remainingLength, "%4d", iDateTime->date.year);
					if (ret < 0 || ret >= remainingLength)
					{
						return RSSL_RET_FAILURE;
					}
					bufOffset += ret;
					remainingLength -= ret;
				}
				else
				{
					ret = snprintf((oBuffer->data + bufOffset), remainingLength, "    ");
					if (ret < 0 || ret >= remainingLength)
					{
						return RSSL_RET_FAILURE;
					}
					bufOffset += ret;
					remainingLength -= ret;
				}
			}
		}
		/* fall through if we were dateTime */
		if (type == RSSL_DT_DATE)
			break;
		else
		{
			/* fall through, but put in a space first */
			ret = snprintf((oBuffer->data + bufOffset), remainingLength, " ");
			if (ret < 0 || ret >= remainingLength)
			{
				return RSSL_RET_FAILURE;
			}
			bufOffset += ret;
			remainingLength -= ret;
		}

		case RSSL_DT_TIME:
		{
			if (iDateTime->time.hour == 255 && iDateTime->time.minute == 255 && iDateTime->time.second == 255 && iDateTime->time.millisecond == 65535 && iDateTime->time.microsecond == 2047 && iDateTime->time.nanosecond == 2047)
			{
				if (type == RSSL_DT_TIME)
				{
					/* only do this if we didnt put a date in here */
					snprintf(oBuffer->data, oBuffer->length, "");
					oBuffer->length = 0;
					return RSSL_RET_SUCCESS;
				}
				else
				{
					oBuffer->length = strlen(oBuffer->data);
					return RSSL_RET_SUCCESS;
				}
			}
			else
			{
				/* have to do this piece by piece to handle the various trailing portions being blank */
				/* hour is always present */
				ret = snprintf((oBuffer->data + bufOffset), remainingLength, "%02d", 
							iDateTime->time.hour);
				if (ret < 0 || ret >= remainingLength)
				{
					return RSSL_RET_FAILURE;
				}
				bufOffset += ret;
				remainingLength -= ret;
				/* minute */
				if (iDateTime->time.minute == 255)
					break;
				
				ret = snprintf((oBuffer->data + bufOffset), remainingLength, ":%02d", 
							iDateTime->time.minute);
				if (ret < 0 || ret >= remainingLength)
				{
					return RSSL_RET_FAILURE;
				}
				bufOffset += ret;
				remainingLength -= ret;
				/* second */
				if (iDateTime->time.second == 255)
					break;
				
				ret = snprintf((oBuffer->data + bufOffset), remainingLength, ":%02d", 
							iDateTime->time.second);
				if (ret < 0 || ret >= remainingLength)
				{
					return RSSL_RET_FAILURE;
				}
				bufOffset += ret;
				remainingLength -= ret;
				/* millisecond */
				if (iDateTime->time.millisecond == 65535)
					break;
				
				ret = snprintf((oBuffer->data + bufOffset), remainingLength, ":%03d", 
							iDateTime->time.millisecond);
				if (ret < 0 || ret >= remainingLength)
				{
					return RSSL_RET_FAILURE;
				}
				bufOffset += ret;
				remainingLength -= ret;
				/* microsecond */
				if (iDateTime->time.microsecond == 2047)
					break;
				
				ret = snprintf((oBuffer->data + bufOffset), remainingLength, ":%03d", 
							iDateTime->time.microsecond);
				if (ret < 0 || ret >= remainingLength)
				{
					return RSSL_RET_FAILURE;
				}
				bufOffset += ret;
				/* nanosecond */
				if (iDateTime->time.nanosecond == 2047)
					break;
				
				ret = snprintf((oBuffer->data + bufOffset), remainingLength, ":%03d", 
							iDateTime->time.nanosecond);
				if (ret < 0 || ret >= remainingLength)
				{
					return RSSL_RET_FAILURE;
				}
				bufOffset += ret;
				remainingLength -= ret;		remainingLength -= ret;
			}
		}
		break;

		default:
			ret = RSSL_RET_UNSUPPORTED_DATA_TYPE;
		break;
	}

	/* set length on out buffer */
	oBuffer->length = strlen(oBuffer->data);

	return RSSL_RET_SUCCESS;
}
