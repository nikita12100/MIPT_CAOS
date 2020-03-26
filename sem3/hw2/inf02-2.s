        .text
        .global solve



solve:

        push r4
        push #0

        try:
                pop r4

                mul r0, r0, r4
                mul r0, r0, r4
                mul r0, r0, r4

                mul r1, r1, r4
                mul r1, r1, r4

                mul r3, r3, r4

                add r0, r0, r1          // ax^3 + bx^2
                add r0, r0, r2          // .. + cx
                add r0, r0, r3          // .. + d

                add r4, r4, #1
                push r4

                cmp r0, #0
                bne try

        pop r4
        pop r4
        bx lr

