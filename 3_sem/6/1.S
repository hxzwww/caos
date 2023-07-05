	.intel_syntax noprefix
	.text
	.global avx_sin

avx_sin:
	mov r10, 1
	mov rcx, 20
	xorpd xmm1, xmm1
	movapd xmm2, xmm0
	addpd xmm1, xmm2
	mulpd xmm0, xmm0
	loop_counter:
		mov rax, 1
		add r10, 1
		mul r10
		add r10, 1
		mul r10
		neg rax
		cvtsi2sd xmm3, rax
		vdivsd xmm4, xmm0, xmm3
		mulpd xmm2, xmm4
		addpd xmm1, xmm2
	loop loop_counter
	movapd xmm0, xmm1
	ret
