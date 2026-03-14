#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "modern_types.h"


typedef enum {
    NOP     = 0x00,
    MOV     ,
    ADD     ,
    SUB     ,
    MUL     ,
    DIV     ,

    MOVI    ,
    ADDI    ,
    SUBI    ,
    MULI    ,
    DIVI    ,
    
    JMP     ,
    CMP     ,
    JE      ,
    JNE     ,
    JG      ,
    JL      ,

    POP     ,
    PUSH    ,
    CALL    ,
    RET     ,
    HALT    ,

    READW   ,
    READB   ,
    STOREW  ,
    STOREB  ,
    MALLOC  ,
}Operations;


// oppcode(8) / registers1(4) register2(4) / immediate(16)
#define INSTRUCTION_SIZE  4
#define MEMORY_SIZE     KiB(4)
#define REGISTERS_COUNT 8

// memory layout
#define RESERVED_START  0x0000
#define RESERVED_END    0x00FF

#define PROGRAM_START   0x0100
#define PROGRAM_END     0x07FF

#define DATA_START      0x0800
#define DATA_END        0x0EFF

#define STACK_START     0x0F00
#define STACK_END       0x1000


enum {
    CPU_FLAG_LESS = 1 << 0,
    CPU_FLAG_EQUAL = 1 << 1,
    CPU_FLAG_GREATER = 1 << 2,
};


typedef struct
{
    i32 registers[REGISTERS_COUNT];
    u64 program_counter;
    u64 stack_pointer;
    u64 heap_pointer;
    byte memory[MEMORY_SIZE];
    u8 flags;
} Cpu;


Cpu* cpu_create();
void cpu_print_memory(Cpu* cpu);
void cpu_print_registers(Cpu* cpu);
void cpu_load_program(Cpu* cpu, byte* program, size_t program_size);
void cpu_run_program(Cpu* cpu);