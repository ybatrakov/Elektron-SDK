
.globl get_cpuid_info
	.type get_cpuid_info, @function
get_cpuid_info:                        
		   movl			4(%esp), %edx
		   movl			8(%esp), %eax
		   movl			12(%esp), %ecx
		   pushl 		%edi
		   pushl		%ebx
		   mov 			%edx, %edi
           cpuid                        
           movl         %eax, (%edi)
           movl         %ebx, 4(%edi)
           movl         %ecx, 8(%edi)
           movl         %edx, 12(%edi)
		   pop			%ebx
		   pop			%edi
           ret
