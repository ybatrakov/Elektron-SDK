/*|-----------------------------------------------------------------------------
 *|            This source code is provided under the Apache 2.0 license      --
 *|  and is provided AS IS with no warranty or guarantee of fit for purpose.  --
 *|                See the project's LICENSE.md for details.                  --
 *|           Copyright Thomson Reuters 2018. All rights reserved.            --
 *|-----------------------------------------------------------------------------
 */

#include "rtr/primenum.h"
#include "rtr/rtrdefs.h"


int rtr_find_prime_num(int number)
{
	static int primes[] = {3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41,
						43, 47, 53, 59, 61, 67, 73, 79, 83, 89, 91, 97};
	int		prime_number=number;
	RTRBOOL	found=RTRFALSE;
	int		remainder;
	int		ii;

	if ( prime_number > 2 )
	{
		remainder = prime_number % 2;
		if ( remainder == 0 )
			prime_number++;

		do
		{
			found = RTRTRUE;
			for (ii = 0; ii < sizeof(primes)/sizeof(int); ii++)
			{
				if (((remainder=(prime_number%primes[ii])) == 0) &&
								(prime_number != primes[ii]) )
					found = RTRFALSE;
			}
			if (!found)
				prime_number += 2;
		} while (!found);
	}
	else
		prime_number = 2;

	return(prime_number);
}

