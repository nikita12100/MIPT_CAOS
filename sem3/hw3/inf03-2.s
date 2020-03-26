	.text
        .global main


main:
        push {r4, r5, r6, r7, lr}
	
	mov r0, #0	//NULL
	mov r1, #40000	// chars
	mov r5, #1	// count of bytes
	mov r6, r1	// curr size of p
	bl realloc	// void *realloc( void *ptr, size_t new_size ); 	
	mov r3, r0	// adress to str p
	mov r7, r3	// adress p[0]
	mov r4, #0	// current index p[i]

	read:
                ldr r0, =stdin                
                ldr r0, [r0]
                bl fgetc
		cmp r0, #-1
		bne save	
        
	b write	
		
	cont:	
		mov r0, r3
		bl free 
               
				
	pop {r4, r5, r6, r7, lr}
        bx lr

my_realloc:
	mov r0, r7	// p[0]
	add r5, r5, r6	// new empty
	mov r1, #512	// 512
	mul r6, r6, r1	// size *= 512, because only once i can realloc coorectly(
	mov r1, r6	// new size 
	bl realloc
	mov r7, r0	// new adress
	mov r0, r4	// i
	mov r1, #4	// 4
	mul r0, r0, r1  	// i*4
	add r0, r0, r7	// r7 + i*4
	mov r3, r0	// r3 = r7 + i*4, r7 is new
	b read


save:
	str r0, [r3]
	add r3, #4	// next char
	add r4, #1	// i++
	sub r5, #1	// empty size 
	cmp r5, #0
	beq my_realloc
	b read

write:
	sub r3, #4
	sub r4, #1
	push {r3}
	ldr r0, [r3]
      	ldr r1, =stdout
        ldr r1, [r1]
	bl fputc
	pop {r3}
	cmp r4, #0
        bgt write
	b cont

