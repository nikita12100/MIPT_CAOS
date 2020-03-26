	.text
	.global summ

summ:
ldr r3, [r2] 	// записываем в r3 перем которая ледит по адрессу в r2, r3=[r2] 
add r2, r2, #4	// сдвинули на 4 байта, след. эл-т, x=&x[1]
add r0, r0, r3	//x0 += x[0]
sub r1, r1, #1	//n--

cmp r1, #0
bgt summ

bx lr 
