.text
.global solve

solve:
	mov w8, 0
	mov w11, 0
	LoopBegin:
		mul w9, w0, w8
		add w9, w9, w1
		mul w9, w9, w8
		add w9, w9, w2
		mul w9, w9, w8
		add w9, w9, w3

		cmp w9, 0
		beq LoopEnd

		add w8, w8, 1

		b LoopBegin
	LoopEnd:
	mov w0, w8
	ret

