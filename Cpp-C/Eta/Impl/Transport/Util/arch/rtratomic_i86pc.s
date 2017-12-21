/* Interlocked functions for i86pc (Opteron) */
	.file	"rtratomic_i86pc.c"
	.text
/***********************************************************************
/*      Atomically increment the value at *var by 1
/*
/*          void rtrInterIncr(long *var)
/*          {
/*              *var++;
/*          }
/*

.globl rtrInterIncr
	.type	rtrInterIncr, @function
rtrInterIncr:
	movl	4(%esp), %eax
	lock; incl (%eax)
	ret
	.size	rtrInterIncr, .-rtrInterIncr


/***********************************************************************
/*      Atomically decrement the value at *var by 1
/*
/*          void rtrInterDecr(long *var)
/*          {
/*              *var--;
/*          }
/*

.globl rtrInterDecr
	.type	rtrInterDecr, @function
rtrInterDecr:
	movl	4(%esp), %eax
	lock; decl (%eax)
	ret
	.size	rtrInterDecr, .-rtrInterDecr


/***********************************************************************
/*      Atomically add "incr" to the value at "*var"
/*
/*          void rtrInterAdd(long *var, long incr)
/*          {
/*              *var += incr;
/*          }
/*

.globl rtrInterAdd
	.type	rtrInterAdd, @function
rtrInterAdd:
	movl	4(%esp), %eax
	movl	8(%esp), %edx
	lock; addl %edx, (%eax)
	ret
	.size	rtrInterAdd, .-rtrInterAdd


/***********************************************************************
/*          Assign the value "newval" into memory at "*var"
/*
/*          void rtrInterExch(long *var, long newval)
/*          {
/*              *var = newval;
/*          }
/*

.globl rtrInterExch
	.type	rtrInterExch, @function
rtrInterExch:
	movl	4(%esp), %eax
	movl	8(%esp), %edx
	xchgl %edx, (%eax)
	ret
	.size	rtrInterExch, .-rtrInterExch

/***********************************************************************
/*          Assign the value "newval" into memory at "*var" (64b on 32b system)
/*
/*          void rtrInterExch64(long long *var, long long newval)
/*          {
/*              *var = newval;
/*          }
/*

.globl rtrInterExch64
	.type	rtrInterExch64, @function
rtrInterExch64:

	subl	$8, %esp

	movl %ebx, (%esp)
	movl %esi, 4(%esp)

	/* Store the desired value into EBX:ECX
	movl	16(%esp), %ebx
	movl	20(%esp), %ecx

	/* Loop until cmpxchg8b succeeds in writing the new value(sets ZF). 
	/* FYI: The old value is written to EDX:EAX.
	movl	12(%esp), %esi
	rtrInterExch64Loop:
	lock; cmpxchg8b (%esi)
	jnz rtrInterExch64Loop


	movl (%esp), %ebx
	movl 4(%esp), %esi

	addl	$8, %esp

	ret
	.size	rtrInterExch64, .-rtrInterExch64

/***********************************************************************
/*      Atomically increment the value at *var by 1
/*      and return the new value
/*
/*          long rtrInterIncrRet(long *var)
/*          {
/*              *var++;
/*              return *var;
/*          }
/*

.globl rtrInterIncrRet
	.type	rtrInterIncrRet, @function
rtrInterIncrRet:
	movl	4(%esp), %edx
	movl	$1, %eax
	lock; xadd %eax,(%edx)
	incl	%eax
	ret
	.size	rtrInterIncrRet, .-rtrInterIncrRet

/***********************************************************************
/*      Atomically decrement the value at *var by 1
/*      and return the new value
/*
/*          long rtrInterDecrRet(long *var)
/*          {
/*              *var--;
/*              return *var;
/*          }
/*

.globl rtrInterDecrRet
	.type	rtrInterDecrRet, @function
rtrInterDecrRet:
	movl	4(%esp), %edx
	movl	$-1, %eax
	lock; xadd %eax,(%edx)
	decl	%eax
	ret
	.size	rtrInterDecrRet, .-rtrInterDecrRet
/***********************************************************************
/*      Atomically increment the value at "*var" by "incr"
/*      and return the new value
/*
/*
/*          long rtrInterAddRet(long *var, long incr)
/*          {
/*              *var += incr;
/*              return  *var;
/*          }
/*

.globl rtrInterAddRet
	.type	rtrInterAddRet, @function
rtrInterAddRet:
	movl	8(%esp), %ecx
	movl	4(%esp), %edx
	movl	%ecx, %eax
	lock; xadd %eax,(%edx)
	addl	%ecx, %eax
	ret
	.size	rtrInterAddRet, .-rtrInterAddRet


/***********************************************************************
/*      Atomic exchange
/*      Assign "newval" to location at "*var"
/*      return the previous value of "*var".
/*
/*          long rtrInterExchRetOld(long *var, long newval)
/*          {
/*              long temp = *var;
/*              *var = newval;
/*              return temp;
/*          }
/*

.globl rtrInterExchRetOld
	.type	rtrInterExchRetOld, @function
rtrInterExchRetOld:
	movl	4(%esp), %edx
	movl	8(%esp), %eax
	xchgl %eax, (%edx)
	ret
	.size	rtrInterExchRetOld, .-rtrInterExchRetOld

