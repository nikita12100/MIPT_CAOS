#include <asm/unistd.h>

	.intel_syntax noprefix
	.text
	.global _start

_start:
	mov edi, 0 # count of str
	read:
		mov eax, __NR_read	#__NR_read 3
		mov ebx, 0	# 0 -read, 1-write, 2-errors
		mov ecx, esp
		mov edx, 1
		int 0x80
		
		
		mov ebx, [esp]
		cmp ebx, 10 # /n code 
		jz save_str
		cmp eax, 0 # end of reading
		jz print		
	save_str:
		push esp
		push eax
		inc edi
		jmp read	
	print:
		pop eax
		mov edx, eax
		pop esp
		mov eax, __NR_write # 4
		mov ebx, 1
		mov ecx, esp
		#mov edx, 1
		int 0x80
		dec edi	
		cmp edi, 0
		jnz print	

	mov eax, __NR_exit # 1
	mov ebx, 1
	int 0x80
