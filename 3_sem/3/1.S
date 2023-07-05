.text
.global sum

sum:
    mov x8, x1
    mov x9, 0

    LoopBegin:
        cmp     xzr, x8
        bge     LoopEnd

        ldr x10, [x2, x9]
        add x0, x0, x10

        add x8, x8, -1
        add x9, x9, 4

        b LoopBegin

    LoopEnd:

    ret
