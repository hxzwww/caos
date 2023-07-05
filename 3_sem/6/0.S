	.intel_syntax noprefix
	.text
	.global very_important_function

very_important_function:
	mov r12, 0
	mov r13, rcx
	mov rcx, rdi

    vxorps ymm2, ymm2, ymm2
	loop_counter:
        vmovups ymm0, [rsi + r12]
        vmovups ymm1, [rdx + r12]

        vaddps ymm0, ymm0, ymm1

        vmovaps [r13 + r12], ymm0

        vxorps ymm3, ymm3, ymm3
        vhaddps ymm0, ymm0, ymm3
        vhaddps ymm0, ymm0, ymm3

        vaddps ymm2, ymm2, ymm0

        sub rcx, 7
        add r12, 32

	loop loop_counter

    vextractf128 xmm1, ymm2, 1
    addps xmm2, xmm1

	vcvtss2sd xmm2, xmm2, xmm2
    vcvtsi2sd xmm0, xmm0, rdi

	vdivsd xmm0, xmm2, xmm0

	ret
