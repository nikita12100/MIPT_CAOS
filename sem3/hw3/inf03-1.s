	.text
	.global main
	

main:
	push {lr}	
	
	read:
		ldr r0, =stdin	// here r0 adress of stdin in talbe		  
		ldr r0, [r0]	
		bl fgetc
	
		cmp r0, #0	// ascii numbers 48-57
		bgt check_number2
		
		pop {lr}
	        bx lr

check_number2:
	cmp r0, #47
	bgt check_number
	b read
check_number:
	cmp r0, #58
	blt write
	b read

write:
	ldr r1, =stdout
	ldr r1, [r1]
	bl fputc
	b read

