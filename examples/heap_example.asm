movi r0, 255
call malloc
movi r1, 255
movi r2, 0

loop:
    addi r2, 1
    storeb r2, 0(r0) ; store byte at address in r0
    addi r0, 1 ; increment pointer
    cmp r2, r1
    jne loop

movi r2, 0

loop2:
    addi r2, 1
    readb r3, 0(r0) ; read byte at address in r0
    subi r0, 1 ; increment pointer
    cmp r2, r1
    jne loop2
