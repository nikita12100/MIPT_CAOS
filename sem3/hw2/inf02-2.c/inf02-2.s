        .text
        .global solve



solve:

        push {r4, r5, r6, r7}	// save
	mov r4, #0	// begin from x = 0
	push {r4}	// our x 

	mov r5, r0	// A - not to remove
	mov r6, r1	// B - not to rm
	mov r7, r2	// C - not to rm

        try:
                pop {r4}

                mov r0, r5
		mov r1, r6
		mov r2, r7

		mul r0, r0, r4
                mul r0, r0, r4
                mul r0, r0, r4

                mul r1, r1, r4
                mul r1, r1, r4

                mul r2, r2, r4

                add r0, r0, r1          // ax^3 + bx^2
                add r0, r0, r2          // .. + cx
                add r0, r0, r3          // .. + d

                add r4, r4, #1
                
		push {r4}

                cmp r0, #0
                bne try

	sub r4, #1
        mov r0, r4
	pop {r4}	// it x
	pop {r4, r5, r6, r7}	// it save
        bx lr

