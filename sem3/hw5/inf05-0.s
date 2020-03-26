	.intel_syntax noprefix
	.text
	.global calc
	.global vector_sum

calc:
	push ebp
	mov ebp,esp

	fld 	qword ptr [ebp+16]	;//st(0)=B, stack_top=st(0)
	fadd	qword ptr [ebp+8]	;//st(0)+=A
					;// now st(0)->st(1)
	fild 	dword ptr [ebp+32]	;// st(0) = (double)D
	fadd 	qword ptr [ebp+24]	;// st(0) += C
	fdivp	st(1), st(0)		;// st(1) /= st(0), in st(1)= A+B, st(0)=C+D

	mov esp,ebp
	pop ebp
	ret 

vector_sum:
	push ebp
	mov ebp, esp
	push esi
	push edi
	push ebx

	mov ecx, [ebp+8]	;// N
	mov esi, [ebp+12]	;// A
	mov edi, [ebp+16]	;// B
	mov ebx, [ebp+20]	;// R
	mov eax, 0
Loop:
	movaps	xmm0, [esi+eax*4]	;// xmm0 = A[i..i+3]
	movaps	xmm1, [edi+eax*4]	;// xmm1 = B[i..i+3]
	addps xmm0, xmm1
	movaps [ebx+eax*4], xmm0	;// R[i..i+3] = xmm0
	add eax, 4
	cmp eax, ecx
	jl Loop

	pop ebx
	pop edi
	pop esi
	mov esp, ebp
	pop ebp
	ret
