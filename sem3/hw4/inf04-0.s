	.global summ
	.intel_syntax noprefix
	.text

summ:
	push ebx 
	
	mov ecx, N	;// index i + 1

.Loop:
	mov ebx, ecx
	dec ebx	;// ebx = N - 1, index i
	mov edx, A

	mov eax, [edx + 4*ebx]	;// adress to A and load
	mov edx, B
	add eax, [edx + 4*ebx] ;// +=b[n-1]
	mov edx, R
	mov [edx+4*ebx],eax ;//r[n-1]=%eax
	loop .Loop 

	pop ebx
	ret

	.global everyday795

everyday795:
	push ebp
	mov ebp, esp
	sub esp, 4	;// int n; &n == [ebp-4]
	push ebx	;// our n

	;//scanf("")
	mov ebx, ebp	;// ebx = &n
	sub ebx, 4	;// ebx = &n
	push ebx
	mov ebx, offset scanf_format_string
	push ebx
	call scanf
	add esp, 8

	mov ebx, [ebp-4]	;// ebp=n
	imul ebx, [ebp+8]	;// n *= x
	add ebx, [ebp+12]	;// n+=y

	;//print("%d", n)
	push ebx	
	mov ebx, printf_format_string_ptr
	push ebx 
	call printf 
	add esp, 8

	pop ebx
	mov esp, ebp
	pop ebp
	ret

scanf_format_string:
	.string "%d"

printf_format_string_ptr:
	.long printf_format_string

	.data
printf_format_string:
	.string "%d\n"
 
