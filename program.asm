; malloc + pointer demo
; int* ptr = malloc(64)
movi R0, 64
call malloc         ; R0 = pointer to 64 bytes on heap

; store the pointer in R0 for later use
mov R1, R0

; 64/4 = 16,
movi R7, 16
loop:
    movi R2, 1
    mul R2, r3,
    muli R2, 4
    storew R2, 0(R1)
    addi R1, 4
    subi R7, 1
    addi R3, R3, 1
    jnez R7, loop



; *(ptr + 4) = 99
movi R1, 99
storew R1, 4(R0)

; R2 = *ptr, R3 = *(ptr + 4)
readw R2, 0(R0)
readw R3, 4(R0)

; store a single byte
movi R1, 0xFF
storeb R1, 0(R0)

; read it back as a byte
readb R4, 0(R0)

halt
