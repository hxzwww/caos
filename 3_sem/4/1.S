	.text
	.global main

main:
	sub sp, sp, 16
	str x30, [sp, 8]

	adr x0, int_format
	add x1, sp, 4

	bl scanf

	adr x0, int_format
	mov x1, sp

	bl scanf

	ldr w1, [sp]
	ldr w2, [sp, 4]

	add w1, w1, w2

	adr x0, int_format

	bl printf

	ldr x30, [sp, 8]
	add sp, sp, 16

	mov x0, 0
	ret

int_format:
	.string "%d"

