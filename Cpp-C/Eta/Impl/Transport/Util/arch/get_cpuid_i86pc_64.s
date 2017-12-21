
.globl get_cpuid_info
	.type get_cpuid_info, @function
get_cpuid_info:                        
		   movq			%rdi,%r8 
		   movq         %rsi,%r9
		   movq 		%rdx,%r10
		   pushl        %rax
 		   pushl		%rbx
		   pushl		%rcx
		   pushl		%rdx
		   movl			%r9d, %eax
		   movl			%r10d, %ecx
           cpuid                        
           movl         %eax, (%r8)
           movl         %ebx, 4(%r8)
           movl         %ecx, 8(%r8)
           movl         %edx, 12(%r8)
		   popl			%rdx
		   popl			%rcx
		   popl			%rbx
		   popl			%rax
           ret 			
