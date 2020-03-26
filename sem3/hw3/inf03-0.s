	 .text
	.global main
	
main:
	push {lr} 
	
	sub sp, #8		// allocate memory 
	ldr r0, scanf_fmt_ptr 	// "%d%d"
	mov r1, sp		// &var1
	mov r2, sp
	add r2, #4		// &var2
	bl scanf
	ldr r2, [sp]
	add sp, #4
	ldr r3, [sp]
	add sp, #4	
	
	ldr r0, printf_fmt_ptr
	
	add r1, r2, r3		
	bl printf
	
	pop {lr}		
	bx lr

scanf_fmt_ptr:
	.word scanf_fmt

scanf_fmt:
	.string "%d %d"

printf_fmt_ptr:
	.word printf_fmt

printf_fmt:
	.string "%d\n\0"
