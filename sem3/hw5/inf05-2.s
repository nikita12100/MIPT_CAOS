   	.intel_syntax noprefix
	.text
	.global dot_product
 
dot_product:
	push ebp
	mov ebp, esp

	sub esp, 4	# allocate
 
	push ebx
	push edi
	
	mov eax, [ebp + 8]	# N
	mov ebx, [ebp + 12] 	# *A
	mov ecx, [ebp + 16] 	# *B
	
	mov edi, 0
	mov edx, 0
	cvtsi2ss xmm0, edx
	.Loop:
		cmp edi, eax
		jge continue
		movups xmm1, [ebx+edi*4] # a[i]
		movups xmm2, [ecx+edi*4] # b[i]
 
		dpps xmm1, xmm2, 0xF1
		addss xmm0, xmm1
 
		add edi, 4
		jmp .Loop
 
	continue:
	movups xmm1, [ebx+edi*4]
	movups xmm2, [ecx+edi*4]
 
	dpps xmm1, xmm2, 0xF1
	addss xmm0, xmm1
 
	movss [ebp-4], xmm0
	fld dword ptr [ebp-4]
 
	pop edi
	pop ebx

	mov esp, ebp
	pop ebp
	ret
