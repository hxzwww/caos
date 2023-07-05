    .global very_important_function
    .intel_syntax noprefix
    .text

very_important_function:
    push r12
    push r14
    push r15

    mov rcx, rdi
    mov r12, 0

	loop_counter:
		mov r13, rcx

    	mov rdi, offset scanf_format
    	mov rsi, rsp
    	call scanf

    	mov r14, A
    	movsxd r14, [r14 + 4 * r12]
    	movsxd r15, [rsp]
    	imul r15, r14

    	mov rdi, offset printf_format
    	mov rsi, r15
    	call printf

		mov rcx, r13
    	inc r12
	loop loop_counter

    pop r12
    pop r14
    pop r15

    mov rax, 0
    ret


.data
scanf_format:
    .string "%d"

printf_format:
    .string "%lld\n"
