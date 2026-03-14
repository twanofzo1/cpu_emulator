movi R0, 64
call malloc         ; R0 = pointer to 64 bytes on heap

mov  R1, R0         
movi R2, 32         
movi R3, 96         


not R2

;loop:
;    storeb R2, 0(R1) ; store counter at pointer
;    addi R1, 1       ; increment pointer
;    addi R2, 1       ; increment counter
;    cmp R2, R3       ; compare counter to limit
;    jne loop         ; jump if not equal
;
;halt