movi R0, 64
call malloc         ; R0 = pointer to 64 bytes on heap

movi R1, 32          ; Save original pointer
movi R2, 0          ; Counter
movi R3, 96         ; Limit


not r1
halt

;loop:
;    storeb R2, 0(R1) ; store counter at pointer
;    addi R1, 1       ; increment pointer
;    addi R2, 1       ; increment counter
;    cmp R2, R3       ; compare counter to limit
;    jne loop         ; jump if not equal
;
;halt