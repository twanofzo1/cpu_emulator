#include "modern_types.h"
#include "file.h"   
#include "defer.h"
#include "emulator.h"
#include "assembler.h"



int main(){
    FILE* input = file_open("program.asm", FILE_MODE_READ_BINARY);
    if (input == NULL) {
        printf("Error opening input file\n");
        return 1;
    }
    char* file_contents = file_get_contents(input);
    if (file_contents == NULL) {
        printf("Error File is empty\n");
        file_close(input);
        return 1;
    }
    file_close(input);
    defer(free(file_contents));

    Cpu* cpu = cpu_create();
    defer(free(cpu));

    byte program[PROGRAM_END - PROGRAM_START];
    memset(program, 0, sizeof(program));
    size_t program_size = assemble(file_contents, program, sizeof(program));
    if (program_size == 0) {
        fprintf(stderr, "Assembly failed or empty program\n");
        return 1;
    }

    printf("Assembled %zu bytes\n", program_size);
    cpu_load_program(cpu, program, program_size);
    cpu_run_program(cpu);
    cpu_print_memory(cpu);
    cpu_print_registers(cpu);

    return 0;
}