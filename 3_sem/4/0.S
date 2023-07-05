.global R, calculate

.data
R: .quad

.text
calculate:
	ldr x1, A
    ldr x2, B
    ldr x3, C
    ldr x4, D

    mul x5, x1, x2
    mul x6, x3, x4
    add x0, x5, x6

	adr x7, R
    str x0, [x7]

    ret
