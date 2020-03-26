//extern void mergesort(int from, int to, const int *in, int *out);
.global mergesort
	.intel_syntax noprefix
	.text
mergesort:
	push ebp
	mov ebp, esp
	sub esp, 8
	push ebx
	push esi
	push edi
	mov eax, [ebp+8]
	mov ebx, [ebp+12]
	sub ebx, eax
	//length
	mov [ebp-8], ebx
	cmp ebx, 1
	je one_word
	mov ebx, [ebp+12]
	add ebx, eax
	sar ebx, 1
	//mid
	mov [ebp-4], ebx
	push [ebp+20]
	push [ebp+16]
	push ebx
	push eax
	call mergesort
	pop eax
	add esp, 12
	push eax
	push [ebp+20]
	push [ebp+16]
	push [ebp+12]
	push ebx
	call mergesort
	add esp, 16
	pop eax
	mov ecx, eax
	mov ebx, [ebp+12]
	mov edx, [ebp+20]
Loop:
	push [edx+4*ecx]
	add ecx, 1
	cmp ecx, ebx
	jl Loop
	mov esi, [ebp-8]
	sub esi, 1
	add esi, [ebp+8]
	mov ebx, [ebp-4]
	mov edi, eax
MergeLoop:
	sub esi, eax
	mov ecx, [esp+4*esi]
	add esi, eax
	sub esi, ebx
	mov edx, [esp+4*esi]
	add esi, ebx
	cmp ebx, [ebp+12]
	je pass_first
	cmp eax, [ebp-4]
	je pass_second
	cmp ecx, edx
	jl pass_first
	jmp pass_second
pass_first:
	mov edx, [ebp+20]
	mov [edx+4*edi], ecx
	add eax, 1
	jmp finish
pass_second:
	mov ecx, [ebp+20]
	mov [ecx+4*edi], edx
	add ebx, 1
finish:
	add edi, 1
	cmp ebx, [ebp+12]
	je second_full
	jmp MergeLoop
second_full:
	cmp eax, [ebp-4]
	jne MergeLoop
exit:
	mov ebx, [ebp-8]
	cmp ebx, 1
	jne clear_stack
after_clear:
	pop edi
	pop esi
	pop ebx
	add esp, 8
	leave
	ret
one_word:
	mov eax, [ebp+16]
	mov ebx, [ebp+20]
	mov ecx, [ebp+8]
	mov edx, [eax+4*ecx]
	mov [ebx+4*ecx], edx
	jmp exit
clear_stack:
	imul ebx, 4
	add esp, ebx
	jmp after_clear
