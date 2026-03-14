# CPU Emulator
This is a simple RISC inspired CPU emulator written in C. It supports a basic set of instructions, including arithmetic operations, jumps, and memory access. 
it also has a simple custom assembler with its own syntax for writing assembly code that can be assembled into machine code and executed by the emulator.


# Building and running
To build there is a build.sh script that compiles the source code and creates an executable called `emulator`. You can run the emulator with an assembly file as an argument, for example:
```bash
./emulator program.asm
```




# Assembly instructions

| commands  | example            | function                          |
| ---       |  ---               | ---                               |
| ADD       |  `ADD R1, R2`      | `reg1 = reg1 + reg2`              |
| SUB       |  `SUB R1, R2`      | `reg1 = reg1 - reg2`              |
| MUL       |  `MUL R1, R2`      | `reg1 = reg1 * reg2`              |
| DIV       |  `DIV R1, R2`      | `reg1 = reg1 / reg2`              |
| MOV       |  `MOV R1, R2`      | `reg1 = reg2`                     |
| JMP       |  `JMP LABEL`       | `jump to label`                   |
| CMP       |  `CMP R1, R2`      | `compare reg1 and reg2`           |
| JE        |  `JE LABEL`        | `jump to label if equal`          |
| JNE       |  `JNE LABEL`       | `jump to label if not equal`      |
| JG        |  `JG LABEL`        | `jump to label if greater`        |
| JL        |  `JL LABEL`        | `jump to label if less`           |
| CALL      |  `CALL FUNCTION`   | `call function`                   |
| RET       |  `RET`             | `return from function`            |
| PUSH      |  `PUSH R1`         | `push r1 onto stack`              |
| POP       |  `POP R1`          | `pop from stack into r1`          |
| :         |  `LABEL:`          | `define a label`                  |
| ;         |  `; COMMENT`       | `comment`                         |
| ADDI      |  `ADDI R1, 5`      | `reg1 = reg1 + 5`                 |
| SUBI      |  `SUBI R1, 5`      | `reg1 = reg1 - 5`                 |
| MULI      |  `MULI R1, 5`      | `reg1 = reg1 * 5`                 |
| DIVI      |  `DIVI R1, 5`      | `reg1 = reg1 / 5`                 |
| MOVI      |  `MOVI R1, 5`      | `reg1 = 5`                        |
| AND       |  `AND R1, R2`      | `reg1 = reg1 & reg2`              |
| OR        |  `OR R1, R2`       | `reg1 = reg1 \| reg2`             |
| XOR       |  `XOR R1, R2`      | `reg1 = reg1 ^ reg2`              |
| NOT       |  `NOT R1`          | `reg1 = ~reg1`                    |
| STOREW    |  `STOREW R1, [R2]` | `store word from r1 into memory at address in r2` |
| READW     |  `READW R1, [R2]`  | `read word from memory at address in r2 into r1`  |
| STOREB    |  `STOREB R1, [R2]` | `store byte from r1 into memory at address in r2` |
| READB     |  `READB R1, [R2]`  | `read byte from memory at address in r2 into r1`  |


## notes
all instructions are not case sensitive, so `add r1, r2` is the same as `ADD R1, R2`.
Malloc is also supported, but it is not an instruction, it is a function that can be called using the `CALL` instruction. For example, to allocate 16 bytes of memory, you can do:

```assembly
MOVI R0, 16      ; size of memory to allocate
CALL malloc       ; call malloc function
```
each instruction is encoded into 4 bytes, 
the first byte is the opcode, 
the second byte is the encoded as 2 4-bit values for the first and second register , 
and the last 2 bytes are used for the immediate value (if applicable). 

there is memory-mapped I/O for printing and reading characters.
- writing a byte to address `0x0EFF` will print the corresponding character to the console.
- reading a byte from address `0x0EFF` will read a character from the console and store it in the specified register.


