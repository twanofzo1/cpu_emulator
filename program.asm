movi R0, 64
call malloc         ; R0 = pointer to 64 bytes on heap


movi R1, 0          
movi R2, 64         
loop:
    addi R1, 1       ; add 1 to counter
    storeb R1, 0(R0) ; store at pointer
    addi R0, 1       ; incr pointer 1

    cmp R1, R2       ; compare counter to limit
    jne loop         ; jump if not equal

halt