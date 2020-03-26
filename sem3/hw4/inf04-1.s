	.global summ
	.intel_syntax noprefix
	.text

summ:
	push ebx
	mov ecx, [esp + 8]	;// N
	mov eax, [esp + 12]	;// A[0]
	mov ebx, [esp + 16]	;// B[0]
	mov edx, [esp + 20]	;// R[0]

.Loop:
	push eax
	mov eax, [eax + 4*(ecx-1)]	;// A[i], i from N to 0
	add eax, [ebx + 4*(ecx-1)]	;// A[i] + B[i]
	
	mov [edx + 4*(ecx-1)], eax	;// R[i] = A[i] + B[i]
	pop eax
	loop .Loop 

	pop ebx
	ret

