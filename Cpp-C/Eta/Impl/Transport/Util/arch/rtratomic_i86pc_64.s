
/* Interlocked functions for i86pc (Opteron 64 bit) */
	.file	"rtratomic_i86pc_64.c"
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
	lock; incq (%rdi)
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
	lock; decq (%rdi)
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
	lock; addq %rsi, (%rdi)
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
	xchgq %rsi, (%rdi)
	movq  %rsi, %rax
	ret
	.size	rtrInterExch, .-rtrInterExch

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
	movq	$1, %rax
	lock; xaddq %rax,(%rdi)
	incq	%rax
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
	movq	$-1, %rax
	lock; xaddq %rax,(%rdi)
	decq	%rax
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
	movq    %rsi, %rax
	lock; xaddq %rsi,(%rdi)
	addq	%rsi, %rax
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
	xchgq   %rsi, (%rdi)
	movq	%rsi, %rax
	ret
	.size	rtrInterExchRetOld, .-rtrInterExchRetOld


