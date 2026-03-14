movi R1, 10      ; Load value 10 into R1
push R1          ; Push R1 onto stack

movi R2, 20      ; Load value 20 into R2
push R2          ; Push R2 onto stack

pop R3           ; Pop top of stack into R3 (should be 20)
pop R4           ; Pop next value into R4 (should be 10)

halt