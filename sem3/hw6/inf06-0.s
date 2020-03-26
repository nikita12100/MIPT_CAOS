#include <asm/unistd_32.h>

	.intel_syntax noprefix
	.text
	.global _start

_start:
	mov eax, __NR_write
	mov ebx, 1
	mov ecx, hello_world_ptr
	mov edx, 13
	int 0x80

	mov eax, __NR_exit
	mov ebx, 1
	int 0x80

	.data
hello_world:
	.string "Hello, World!"
hello_world_ptr:
	.long hello_world
