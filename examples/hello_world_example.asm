movi R0, 104 ; 'h'
call print_char
movi R0, 101 ; 'e'
call print_char
movi R0, 108 ; 'l'
call print_char
movi R0, 108 ; 'l'
call print_char
movi R0, 111 ; 'o'
call print_char

movi R0, 32    ' '
call print_char


call print_char
movi R0, 119 ; 'w'
call print_char
movi R0, 111 ; 'o'
call print_char
movi R0, 114 ; 'r'
call print_char
movi R0, 108 ; 'l'
call print_char
movi R0, 100 ; 'd'
call print_char
    


halt

print_char:
    movi R1, 0x0EFF
    storeb R0, 0(R1)
    ret

get_char:
    movi R1, 0x0EFF
    readb R0, 0(R1)
    ret


