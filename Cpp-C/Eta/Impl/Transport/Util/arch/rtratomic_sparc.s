/* Interlocked functions for Sparc64 (Ultra) */

/* All Interlocked function for Sparc64 use the cas (compare and
 * swap) operation. It basically works like this:
 *
 *	void cas(MEM, REG1, REG2)
 *	{
 *		START_ATOMIC();
 *		if (*(MEM) == REG1)
 *		{
 *			TMP = *(MEM);
 *			*(MEM) = REG2;
 *			REG2 = TMP;
 *		}
 *		else
 *			REG2 = *(MEM);
 *		END_ATOMIC();
 *
 */


/*************************************************************************************/
/* Return no value */
/*************************************************************************************/

/* Take the value at address val (*val), and increment by 1.

	 %o0 = address of value to increment
	 %o2 = work register to store actual value to be incremented 
	 %o3 = work register to store incremented value, after successful cas it contains old value
 */

		.section		".text"
		.global	rtrInterIncr
		.align	8
		.type rtrInterIncr, #function
rtrInterIncr:

ld [%o0], %o2							/* Store a copy of value into %o2 */
rtrInterIncrRetry:						/* Set the retry point */
	add %o2, 0x01, %o3					/* Add one to copy of value, store in %o3 */
	cas [%o0], %o2, %o3					/* Atomically check original value, from
										 * pointer [%o0] with copy of original
										 * value. If they are the same then nobody
										 * else has tried to modify the value and
										 * we swap [%o0] with new value %o3 
										 */
	cmp %o2, %o3						/* Compare */
	bne,a,pn %icc, rtrInterIncrRetry	/* If they are not equal, then retry */
	  ld [%o0], %o2						/* Operation to load in NPC due to ,a above.
										 * This is only run if cmp fails.
										 */
retl
  nop
.size rtrInterIncr, (.-rtrInterIncr)



/* Take the value at address val (*val), and decrement it by one.

	 %o0 = address of value to increment
	 %o2 = work register to store actual value to be incremented 
	 %o3 = work register to store incremented value, after successful cas it contains old value
 */
		.section		".text"
		.global	rtrInterDecr
		.align	8
		.type rtrInterDecr, #function
rtrInterDecr:

ld [%o0], %o2
rtrInterDecrRetry:
	sub %o2, 0x01, %o3
	cas [%o0], %o2, %o3
	cmp %o2, %o3
	bne,a,pn %icc, rtrInterDecrRetry
	  ld [%o0], %o2
retl
  nop
.size rtrInterDecr, (.-rtrInterDecr)



/* Take the value at address val (*val), and increment by incr.

	 %o0 = address of value to increment
	 %o1 = increment value 
	 %o2 = work register to store actual value to be incremented 
	 %o3 = work register to store incremented value, after successful cas contain old value
 */
		.section		".text"
		.global	rtrInterAdd
		.align	8
		.type rtrInterAdd, #function
rtrInterAdd:

ld [%o0], %o2
rtrInterAddRetry:
	add %o2, %o1, %o3
	cas [%o0], %o2, %o3
	cmp %o2, %o3
	bne,a,pn %icc, rtrInterAddRetry
	  ld [%o0], %o2
retl
  nop
.size rtrInterAdd, (.-rtrInterAdd)



/* Take the value at address val (*val), and set it to newval.
	The old value will be returned.

	 %o0 = address of value to increment
	 %o2 = work register to store actual value to be incremented 
	 %o3 = work register to store incremented value, after successful cas it contains old value
 */
		.section		".text"
		.global	rtrInterExch
		.type rtrInterExch, #function
rtrInterExch:

ld [%o0], %o2
rtrInterExchRetry:
	mov %o1, %o3
	cas [%o0], %o2, %o3
	cmp %o2, %o3
	bne,a,pn %icc, rtrInterExchRetry
	  ld [%o0], %o2
retl
  nop
.size rtrInterExch, (.-rtrInterExch)

		.section		".text"
		.global rtrInterExch64
		.type rtrInterExch64, #function
rtrInterExch64:

ld [%o0], %o2
rtrInterExch64Retry:
	unimp 0xf00

retl
  nop
.size rtrInterExch64, (.-rtrInterExch64)


/*************************************************************************************/
/* Return new value */
/*************************************************************************************/

/* Take the value at address val (*val), and increment by 1.
   The new value is stored in the same address and the new
   value is also returned.

	 %o0 = address of value to increment
	 %o2 = work register to store actual value to be incremented 
	 %o3 = work register to store incremented value, after successful cas it contains old value
 */

		.section		".text"
		.global	rtrInterIncrRet
		.align	8
		.type rtrInterIncrRet, #function
rtrInterIncrRet:

ld [%o0], %o2							/* Store a copy of value into %o2 */
rtrInterIncrRetRetry:					/* Set the retry point */
	add %o2, 0x01, %o3					/* Add one to copy of value, store in %o3 */
	cas [%o0], %o2, %o3					/* Atomically check original value, from
										 * pointer [%o0] with copy of original
										 * value. If they are the same then nobody
										 * else has tried to modify the value and
										 * we swap [%o0] with new value %o3 
										 */
	cmp %o2, %o3						/* Compare */
	bne,a,pn %icc, rtrInterIncrRetRetry	/* If they are not equal, then retry */
	  ld [%o0], %o2						/* Operation to load in NPC due to ,a above.
										 * This is only run if cmp fails.
										 */
retl
  add %o2, 0x01, %o0						/* Set return value */
.size rtrInterIncrRet, (.-rtrInterIncrRet)



/* Take the value at address val (*val), and decrement it by one.
	The old value will be returned.

	 %o0 = address of value to increment
	 %o2 = work register to store actual value to be incremented 
	 %o3 = work register to store incremented value, after successful cas it contains old value
 */
		.section		".text"
		.global	rtrInterDecrRet
		.align	8
		.type rtrInterDecrRet, #function
rtrInterDecrRet:

ld [%o0], %o2
rtrInterDecrRetRetry:
	sub %o2, 0x01, %o3
	cas [%o0], %o2, %o3
	cmp %o2, %o3
	bne,a,pn %icc, rtrInterDecrRetRetry
	  ld [%o0], %o2
retl
  sub %o2, 0x01, %o0
.size rtrInterDecrRet, (.-rtrInterDecrRet)



/* Take the value at address val (*val), and increment by incr.
   The new value is stored in the same address and the new
   value is also returned.

	 %o0 = address of value to increment
	 %o1 = increment value 
	 %o2 = work register to store actual value to be incremented 
	 %o3 = work register to store incremented value, after successful cas contain old value
 */
		.section		".text"
		.global	rtrInterAddRet
		.align	8
		.type rtrInterAddRet, #function
rtrInterAddRet:

ld [%o0], %o2
rtrInterAddRetRetry:
	add %o2, %o1, %o3
	cas [%o0], %o2, %o3
	cmp %o2, %o3
	bne,a,pn %icc, rtrInterAddRetRetry
	  ld [%o0], %o2
retl
  add %o2, %o1, %o0
.size rtrInterAddRet, (.-rtrInterAddRet)



/*************************************************************************************/
/* Return old value */
/*************************************************************************************/


/* Take the value at address val (*val), and set it to newval.
	The old value will be returned.

	 %o0 = address of value to increment
	 %o2 = work register to store actual value to be incremented 
	 %o3 = work register to store incremented value, after successful cas it contains old value
 */
		.section		".text"
		.global	rtrInterExchRetOld
		.type rtrInterExchRetOld, #function
rtrInterExchRetOld:

ld [%o0], %o2
rtrInterExchRetOldRetry:
	mov %o1, %o3
	cas [%o0], %o2, %o3
	cmp %o2, %o3
	bne,a,pn %icc, rtrInterExchRetOldRetry
	  ld [%o0], %o2
retl
  mov %o3, %o0
.size rtrInterExchRetOld, (.-rtrInterExchRetOld)



