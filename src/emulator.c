#include "emulator.h"

Cpu* cpu_create(){
    Cpu* cpu = (Cpu*)malloc(sizeof(Cpu));
    cpu->program_counter = PROGRAM_START;
    cpu->stack_pointer = STACK_START;
    cpu->heap_pointer = DATA_START;
    return cpu;
}

typedef struct {
    const char* name;
    u32 start;
    u32 end;
} Memory_section;

void cpu_print_memory(Cpu* cpu){
    const int PER_ROW = 32;
    Memory_section sections[] = {
        {"Reserved", RESERVED_START, RESERVED_END},
        {"Program" , PROGRAM_START , PROGRAM_END},
        {"Heap"    , DATA_START    , DATA_END},
        {"Stack"   , STACK_START   , STACK_END}
    };
    // this just looks like a clusterfuck
    for (int s = 0; s < 4; ++s) {
        printf("\n%s memory [%04X-%04X]:\n", sections[s].name, sections[s].start, sections[s].end-1);
        for (u32 i = sections[s].start; i < sections[s].end; ++i) {
            if ((i-sections[s].start) % PER_ROW == 0) {
                printf("%04X: ", i);
            }
            if (i == cpu->stack_pointer || i == cpu->heap_pointer) {
                printf("\033[1;33m%02X\033[0m ", cpu->memory[i]);
            } else {
                printf("%02X ", cpu->memory[i]);
            }
            if ((i-sections[s].start+1) % PER_ROW == 0 || i == sections[s].end-1) {
                printf("\n");
            }
        }
    }
    printf("\n");
}


void cpu_print_registers(Cpu* cpu){
    printf("Register memory:\n");
    for (u8 i = 0; i <REGISTERS_COUNT; i++){
        printf("Register %i: %#018x\n",i,cpu->registers[i]);
    }
    printf("\n");
}


void cpu_load_program(Cpu* cpu, byte* program, size_t program_size){
    if (program_size > (PROGRAM_END - PROGRAM_START)) {
        fprintf(stderr, "Program size exceeds available memory\n");
        return;
    }
    memcpy(cpu->memory + PROGRAM_START, program, program_size);
}


typedef struct 
{
    byte opp_code;
    byte registers;
    u16 immediate;
} Instruction;

static inline Instruction cpu_fetch_instruction(Cpu* cpu){
    if (cpu->program_counter > PROGRAM_END) {
        fprintf(stderr, "Program counter out of bounds\n");
        exit(1);
    }
    Instruction instruction;
    instruction.opp_code = cpu->memory[cpu->program_counter];
    instruction.registers = cpu->memory[cpu->program_counter + 1];
    instruction.immediate = (cpu->memory[cpu->program_counter + 2] << 8) | cpu->memory[cpu->program_counter + 3];
    cpu->program_counter += INSTRUCTION_SIZE;
    return instruction;
}

#define DECODE_REGISTER1(instr) (instr.registers >> 4)
#define DECODE_REGISTER2(instr) (instr.registers & 0x0F)



static inline void opp_add(Cpu* cpu, Instruction instr){
    cpu->registers[DECODE_REGISTER1(instr)] += cpu->registers[DECODE_REGISTER2(instr)];
}

static inline void opp_sub(Cpu* cpu, Instruction instr){
    cpu->registers[DECODE_REGISTER1(instr)] -= cpu->registers[DECODE_REGISTER2(instr)];
}

static inline void opp_mul(Cpu* cpu, Instruction instr){
    cpu->registers[DECODE_REGISTER1(instr)] *= cpu->registers[DECODE_REGISTER2(instr)];
}

static inline void opp_div(Cpu* cpu, Instruction instr){
    cpu->registers[DECODE_REGISTER1(instr)] /= cpu->registers[DECODE_REGISTER2(instr)];
}

static inline void opp_mov(Cpu* cpu, Instruction instr){
    cpu->registers[DECODE_REGISTER1(instr)] = cpu->registers[DECODE_REGISTER2(instr)];
}

static inline void opp_addi(Cpu* cpu, Instruction instr){
    cpu->registers[DECODE_REGISTER1(instr)] += instr.immediate;
}

static inline void opp_subi(Cpu* cpu, Instruction instr){
    cpu->registers[DECODE_REGISTER1(instr)] -= instr.immediate;
}

static inline void opp_muli(Cpu* cpu, Instruction instr){
    cpu->registers[DECODE_REGISTER1(instr)] *= instr.immediate;
}

static inline void opp_divi(Cpu* cpu, Instruction instr){
    cpu->registers[DECODE_REGISTER1(instr)] /= instr.immediate;
}

static inline void opp_movi(Cpu* cpu, Instruction instr){
    cpu->registers[DECODE_REGISTER1(instr)] = instr.immediate;
}

static inline void opp_jmp(Cpu* cpu, Instruction instr){
    cpu->program_counter = instr.immediate;
}



static inline void opp_jez(Cpu* cpu, Instruction instr){
    if (cpu->registers[DECODE_REGISTER1(instr)] == 0) {
        cpu->program_counter = instr.immediate;
    }
}

static inline void opp_jnez(Cpu* cpu, Instruction instr){
    if (cpu->registers[DECODE_REGISTER1(instr)] != 0) {
        cpu->program_counter = instr.immediate;
    }
}

static inline void opp_jgz(Cpu* cpu, Instruction instr){
    if (cpu->registers[DECODE_REGISTER1(instr)] > 0) {
        cpu->program_counter = instr.immediate;
    }
}

static inline void opp_jlz(Cpu* cpu, Instruction instr){
    if (cpu->registers[DECODE_REGISTER1(instr)] < 0) {
        cpu->program_counter = instr.immediate;
    }
}

static inline void opp_jgez(Cpu* cpu, Instruction instr){
    if (cpu->registers[DECODE_REGISTER1(instr)] >= 0) {
        cpu->program_counter = instr.immediate;
    }
}

static inline void opp_jlez(Cpu* cpu, Instruction instr){
    if (cpu->registers[DECODE_REGISTER1(instr)] <= 0) {
        cpu->program_counter = instr.immediate;
    }
}

static inline void opp_call(Cpu* cpu, Instruction instr){
    if (cpu->stack_pointer + 4 > STACK_END) {
        fprintf(stderr, "Stack overflow error\n");
        exit(1);
    }
    u32 return_addr = (u32)cpu->program_counter;
    memcpy(cpu->memory + cpu->stack_pointer, &return_addr, 4);
    cpu->stack_pointer += 4;
    cpu->program_counter = instr.immediate;
}

static inline void opp_ret(Cpu* cpu, Instruction instr){
    (void)instr;
    if (cpu->stack_pointer <= STACK_START) {
        fprintf(stderr, "Stack underflow error\n");
        exit(1);
    }
    cpu->stack_pointer -= 4;
    u32 return_addr;
    memcpy(&return_addr, cpu->memory + cpu->stack_pointer, 4);
    cpu->program_counter = return_addr;
}

static inline void opp_pop(Cpu* cpu, Instruction instr){
    if (cpu->stack_pointer <= STACK_START) { 
        fprintf(stderr, "Stack underflow error\n");
        exit(1);
    }
    cpu->stack_pointer -= 4;
    memcpy(&cpu->registers[DECODE_REGISTER1(instr)], cpu->memory + cpu->stack_pointer, 4);
}


static inline void opp_push(Cpu* cpu, Instruction instr){
    if (cpu->stack_pointer + 4 > STACK_END) {
        fprintf(stderr, "Stack overflow error\n");
        exit(1);
    }
    memcpy(cpu->memory + cpu->stack_pointer, &cpu->registers[DECODE_REGISTER1(instr)], 4);
    cpu->stack_pointer += 4;
}


static inline void opp_readw(Cpu* cpu, Instruction instr){
    u32 addr = (u32)((i32)cpu->registers[DECODE_REGISTER2(instr)] + (i16)instr.immediate);
    if (addr + 4 > MEMORY_SIZE) {
        fprintf(stderr, "Memory read out of bounds: address 0x%04X (word)\n", addr);
        exit(1);
    }
    u32 val;
    memcpy(&val, cpu->memory + addr, 4);
    cpu->registers[DECODE_REGISTER1(instr)] = val;
}

static inline void opp_readb(Cpu* cpu, Instruction instr){
    u32 addr = (u32)((i32)cpu->registers[DECODE_REGISTER2(instr)] + (i16)instr.immediate);
    if (addr >= MEMORY_SIZE) {
        fprintf(stderr, "Memory read out of bounds: address 0x%04X (byte)\n", addr);
        exit(1);
    }
    cpu->registers[DECODE_REGISTER1(instr)] = cpu->memory[addr];
}

static inline void opp_storew(Cpu* cpu, Instruction instr){
    u32 addr = (u32)((i32)cpu->registers[DECODE_REGISTER2(instr)] + (i16)instr.immediate);
    if (addr + 4 > MEMORY_SIZE) {
        fprintf(stderr, "Memory write out of bounds: address 0x%04X (word)\n", addr);
        exit(1);
    }
    u32 val = cpu->registers[DECODE_REGISTER1(instr)];
    memcpy(cpu->memory + addr, &val, 4);
}

static inline void opp_storeb(Cpu* cpu, Instruction instr){
    u32 addr = (u32)((i32)cpu->registers[DECODE_REGISTER2(instr)] + (i16)instr.immediate);
    if (addr >= MEMORY_SIZE) {
        fprintf(stderr, "Memory write out of bounds: address 0x%04X (byte)\n", addr);
        exit(1);
    }
    cpu->memory[addr] = (byte)(cpu->registers[DECODE_REGISTER1(instr)] & 0xFF);
}

static inline void opp_malloc(Cpu* cpu, Instruction instr){
    (void)instr;
    u32 size = cpu->registers[0];
    if (size == 0) {
        fprintf(stderr, "Malloc error: cannot allocate 0 bytes\n");
        exit(1);
    }
    if (cpu->heap_pointer + size > DATA_END) {
        fprintf(stderr, "Malloc error: out of heap memory (requested %u bytes, %u available)\n",
                size, (u32)(DATA_END - cpu->heap_pointer));
        exit(1);
    }
    u32 ptr = (u32)cpu->heap_pointer;
    cpu->heap_pointer += size;
    cpu->registers[0] = ptr;
}
   





void cpu_run_program(Cpu* cpu){
    cpu->program_counter = PROGRAM_START;
    while (cpu->program_counter < PROGRAM_END)
    {
        Instruction instr = cpu_fetch_instruction(cpu); 
        switch (instr.opp_code)
        {
            // register to register opps
            case ADD:   opp_add(cpu, instr); break;
            case SUB:   opp_sub(cpu, instr); break;
            case MUL:   opp_mul(cpu, instr); break;
            case DIV:   opp_div(cpu, instr); break;
            case MOV:   opp_mov(cpu, instr); break;

            // register to immediate opps
            case ADDI:  opp_addi(cpu, instr); break;
            case SUBI:  opp_subi(cpu, instr); break;
            case MULI:  opp_muli(cpu, instr); break;
            case DIVI:  opp_divi(cpu, instr); break;
            case MOVI:  opp_movi(cpu, instr); break;

            // jump opps
            case JMP:   opp_jmp(cpu, instr); break;
            case JEZ:   opp_jez(cpu, instr); break;
            case JNEZ:  opp_jnez(cpu, instr); break;
            case JGZ:   opp_jgz(cpu, instr); break;
            case JLZ:   opp_jlz(cpu, instr); break;
            case JGEZ:  opp_jgez(cpu, instr); break;
            case JLEZ:  opp_jlez(cpu, instr); break;

            // stack ops
            case POP:   opp_pop(cpu, instr); break;
            case PUSH:  opp_push(cpu, instr); break;

            // call / return
            case CALL:  opp_call(cpu, instr); break;
            case RET:   opp_ret(cpu, instr); break;

            // memory ops
            case READW:  opp_readw(cpu, instr); break;
            case READB:  opp_readb(cpu, instr); break;
            case STOREW: opp_storew(cpu, instr); break;
            case STOREB: opp_storeb(cpu, instr); break;

            // heap
            case MALLOC: opp_malloc(cpu, instr); break;

            case NOP:   break; // do nothing
            case HALT:  goto function_exit;
            
            default:
                fprintf(stderr, "Unknown instruction: 0x%02x\n", instr.opp_code);
                return;
            }

    }
    function_exit:
    //idk some shit maybe
    return;
}