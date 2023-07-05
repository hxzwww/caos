    .global sum
    .intel_syntax noprefix
    .text

sum:
    mov r8, 0
    mov r13, rcx
	mov rcx, rdi

	loop_counter:
        mov r14d, [rsi + 4 * r8]
        mov r15d, [rdx + 4 * r8]
        add r14d, r15d
        mov [r13 + 4 * r8], r14d
        inc r8

    loop loop_counter

    mov rax, 0
    ret
