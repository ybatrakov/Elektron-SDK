extern void get_cpuid_info_X64(void *info, const unsigned int func, const unsigned int subfunc);

void get_cpuid_info(void *info, const unsigned int func, const unsigned int subfunc)
{
#ifdef _WIN64
	get_cpuid_info_X64(info, func, subfunc);
#else
	__asm
	{
		mov edx, info
		mov eax, func
		mov ecx, subfunc
		push edi
		push ebx
		mov edi, edx
		cpuid
		mov DWORD PTR [edi], eax
 		mov DWORD PTR [edi+4], ebx
		mov DWORD PTR [edi+8], ecx
		mov DWORD PTR [edi+12], edx
		pop ebx
		pop edi
	}
#endif
}
