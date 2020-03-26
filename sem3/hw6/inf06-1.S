#include <asm/unistd.h>

	.intel_syntax noprefix
	.text
	.global _start

_start:
	read:
		mov eax, __NR_read	#__NR_read 3
		mov ebx, 0	# 0 -read, 1-write, 2-errors
		mov ecx, esp
		mov edx, 1
		int 0x80

		cmp eax, 0 # count of returning bytes 
		jz quit	
	
		mov eax, __NR_write # 4
		mov ebx, 1
		mov ecx, esp
		mov edx, 1
		int 0x80

		jmp read	

	quit:
		mov eax, __NR_exit # 1
		mov ebx, 1
		int 0x80
