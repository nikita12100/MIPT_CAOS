	.intel_syntax noprefix
	.text
	.global my_sin

my_sin:
	push ebp
	mov ebp, esp
	push ebx

	mov eax, -1 # (-1)^i
	mov edx, 1

	mov ebx, 0
	cvtsi2sd xmm0, ebx # result
	movsd xmm3, [ebp + 8] # (-1)^i*x^(2*i+1)/(2*i+1)!
	movsd xmm2, [ebp + 8] # х

	while:
		movsd xmm1, xmm0
		addsd xmm0, xmm3 # + new term 

		cvtsi2sd xmm4, eax
		imul eax, -1 # (-1)^1

		mulsd xmm3, xmm4 # (-1)^*x
		mulsd xmm3, xmm2 # *= x
		mulsd xmm3, xmm2 # *= x

		add edx, 1 # /= (n+2)(n+1)	# out factorial
		cvtsi2sd xmm4, edx
		divsd xmm3, xmm4
		add edx, 1
		cvtsi2sd xmm4, edx
		divsd xmm3, xmm4

		comisd xmm0, xmm1	# double compare xmm0, xmm1
		jne while

	sub esp, 8	# allocate for result
	movsd [esp], xmm0	# write result
	fld qword ptr [esp]
	add esp, 8
	
	pop ebx
	mov esp, ebp
	pop ebp
	ret
