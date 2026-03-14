#include "assembler.h"

typedef struct {
    const char* name;
    int opcode;
} OpcodeEntry;

OpcodeEntry opcode_table[] = {
    { "NOP"  ,  NOP  },
    { "MOV"  ,  MOV  },
    { "ADD"  ,  ADD  },
    { "SUB"  ,  SUB  },
    { "MUL"  ,  MUL  },
    { "DIV"  ,  DIV  },
    { "MOVI" ,  MOVI },
    { "ADDI" ,  ADDI },
    { "SUBI" ,  SUBI },
    { "MULI" ,  MULI },
    { "DIVI" ,  DIVI },
    { "JMP"  ,  JMP  },
    { "CMP"  ,  CMP  },
    { "JE"  ,   JE   },
    { "JNE" ,   JNE  },
    { "JG"  ,   JG   },
    { "JL"  ,   JL   },
    { "POP"  ,  POP  },
    { "PUSH" ,  PUSH },
    { "CALL" ,  CALL },
    { "RET"  ,  RET  },
    { "HALT" ,  HALT },
    { "READW",  READW },
    { "READB",  READB },
    { "STOREW", STOREW },
    { "STOREB", STOREB },
    { "MALLOC", MALLOC }
};

#define OPCODE_TABLE_SIZE (sizeof(opcode_table)/sizeof(opcode_table[0]))

int get_opcode_from_string(const char* name) {
    for (size_t i = 0; i < OPCODE_TABLE_SIZE; ++i) {
        if (strcmp(opcode_table[i].name, name) == 0) {
            return opcode_table[i].opcode;
        }
    }
    return -1; // Not found
}






typedef struct {
    char name[255];
    u32 address;
} Label;

typedef struct {
    Label  labels[255];
    size_t label_count;
    u32 offset;
} Assembler;


// ---- Assembler ----

static char* skip_whitespace(char* s) {
    while (*s == ' ' || *s == '\t' || *s == '\r') s++;
    return s;
}

static void trim_trailing(char* s) {
    size_t len = strlen(s);
    while (len > 0 && (s[len-1]==' '||s[len-1]=='\t'||s[len-1]=='\r'||s[len-1]=='\n')) {
        len--;
        s[len] = '\0';
    }
}

static void strip_comment(char* line) {
    char* p = strchr(line, ';');
    if (p) *p = '\0';
}

static int parse_register(const char* s) {
    while (*s == ' ' || *s == '\t') s++;
    if ((*s == 'R' || *s == 'r') && s[1] >= '0' && s[1] <= '7')
        return s[1] - '0';
    return -1;
}

static i32 resolve_immediate(const char* token, Assembler* assembler) {
    char buf[256];
    strncpy(buf, token, 255);
    buf[255] = '\0';
    trim_trailing(buf);
    char* s = buf;
    while (*s == ' ' || *s == '\t') s++;

    if (s[0] == '0' && (s[1] == 'x' || s[1] == 'X'))
        return (i32)strtol(s, NULL, 16);
    if ((s[0] >= '0' && s[0] <= '9') || s[0] == '-')
        return (i32)strtol(s, NULL, 10);

    for (size_t i = 0; i < assembler->label_count; i++) {
        if (strcmp(assembler->labels[i].name, s) == 0)
            return (i32)assembler->labels[i].address;
    }
    fprintf(stderr, "Error: unknown label or value '%s'\n", s);
    return -1;
}

typedef enum {
    ITYPE_NONE,       // NOP, HALT, MALLOC
    ITYPE_REG_REG,    // MOV, ADD, SUB, MUL, DIV
    ITYPE_REG_IMM,    // MOVI, ADDI, SUBI, MULI, DIVI
    ITYPE_IMM,        // JMP
    ITYPE_REG_ADDR,   // JE, JNE, JG, JL
    ITYPE_REG,        // PUSH, POP
    ITYPE_REG_MEM,    // READW, READB, STOREW, STOREB: reg, offset(reg)
} InstrType;

static InstrType get_instr_type(int opcode) {
    switch (opcode) {
        case NOP: 
        case HALT: 

        return ITYPE_NONE;
        case MOV: 
        case ADD: 
        case SUB: 
        case MUL: 
        case DIV: 
        case CMP:
        return ITYPE_REG_REG;

        case MOVI: 
        case ADDI: 
        case SUBI: 
        case MULI: 
        case DIVI: 
        return ITYPE_REG_IMM;

        case JMP: 
        case JE: 
        case JNE: 
        case JG: 
        case JL: 
        return ITYPE_IMM;

        case POP: 
        case PUSH: 
        return ITYPE_REG;

        case CALL:
        return ITYPE_IMM;

        case RET:
        return ITYPE_NONE;

        case READW:
        case READB:
        case STOREW:
        case STOREB:
        return ITYPE_REG_MEM;

        case MALLOC:
        return ITYPE_NONE;

        default: 
        return ITYPE_NONE;
    }
}

// ---- Pass 1: scan source and record label addresses ----
static int pass1_collect_labels(Assembler* assembler, const char* source) {
    const char* p = source;
    while (*p) {
        const char* nl = strchr(p, '\n');
        size_t line_len = nl ? (size_t)(nl - p) : strlen(p);

        char line[256];
        size_t clen = line_len < 255 ? line_len : 255;
        memcpy(line, p, clen);
        line[clen] = '\0';

        strip_comment(line);
        trim_trailing(line);
        char* t = skip_whitespace(line);

        if (*t) {
            char* colon = strchr(t, ':');
            if (colon) {
                *colon = '\0';
                char* name = skip_whitespace(t);
                trim_trailing(name);
                if (assembler->label_count >= 255) {
                    fprintf(stderr, "Error: labels cant exeed 255\n");
                    return 0;
                }
                Label* lbl = &assembler->labels[assembler->label_count++];
                strncpy(lbl->name, name, 254);
                lbl->name[254] = '\0';
                lbl->address = PROGRAM_START + assembler->offset;

                char* rest = skip_whitespace(colon + 1);
                if (*rest) assembler->offset += INSTRUCTION_SIZE;
            } else {
                assembler->offset += INSTRUCTION_SIZE;
            }
        }

        p += line_len;
        if (*p == '\n') p++;
    }
    return 1;
}

// ---- Encode operands for a single instruction ----
static int encode_operands(InstrType type, char* ops, Assembler* assembler,
                           int line_num, byte* reg_byte_out, u16* imm_out) {
    switch (type) {
        case ITYPE_NONE:
            break;
        case ITYPE_REG_REG: {
            char* comma = strchr(ops, ',');
            if (!comma) { fprintf(stderr, "Error on line %d: expected two registers\n", line_num); return 0; }
            *comma = '\0';
            int r1 = parse_register(ops), r2 = parse_register(comma + 1);
            if (r1 < 0 || r2 < 0) { fprintf(stderr, "Error on line %d: invalid register\n", line_num); return 0; }
            *reg_byte_out = (r1 << 4) | r2;
            break;
        }
        case ITYPE_REG_IMM:
        case ITYPE_REG_ADDR: {
            char* comma = strchr(ops, ',');
            if (!comma) { fprintf(stderr, "Error on line %d: expected register and operand\n", line_num); return 0; }
            *comma = '\0';
            int r1 = parse_register(ops);
            if (r1 < 0) { fprintf(stderr, "Error on line %d: invalid register\n", line_num); return 0; }
            i32 val = resolve_immediate(comma + 1, assembler);
            *reg_byte_out = (r1 << 4);
            *imm_out = (u16)val;
            break;
        }
        case ITYPE_IMM: {
            i32 val = resolve_immediate(ops, assembler);
            *imm_out = (u16)val;
            break;
        }
        case ITYPE_REG: {
            int r1 = parse_register(ops);
            if (r1 < 0) { fprintf(stderr, "Error on line %d: invalid register\n", line_num); return 0; }
            *reg_byte_out = (r1 << 4);
            break;
        }
        case ITYPE_REG_MEM: {
            char* comma = strchr(ops, ',');
            if (!comma) { fprintf(stderr, "Error on line %d: expected register and offset(register)\n", line_num); return 0; }
            *comma = '\0';
            int r1 = parse_register(ops);
            if (r1 < 0) { fprintf(stderr, "Error on line %d: invalid register\n", line_num); return 0; }

            char* rest = skip_whitespace(comma + 1);
            char* open_paren = strchr(rest, '(');
            if (!open_paren) { fprintf(stderr, "Error on line %d: expected offset(register) syntax\n", line_num); return 0; }
            *open_paren = '\0';

            char* offset_str = skip_whitespace(rest);
            trim_trailing(offset_str);
            i32 offset = 0;
            if (*offset_str) {
                offset = (i32)strtol(offset_str, NULL, 0);
            }

            char* reg_str = open_paren + 1;
            char* close_paren = strchr(reg_str, ')');
            if (!close_paren) { fprintf(stderr, "Error on line %d: missing closing ')'\n", line_num); return 0; }
            *close_paren = '\0';

            int r2 = parse_register(reg_str);
            if (r2 < 0) { fprintf(stderr, "Error on line %d: invalid register in offset(register)\n", line_num); return 0; }

            *reg_byte_out = (r1 << 4) | r2;
            *imm_out = (u16)(i16)offset;
            break;
        }
    }
    return 1;
}

// ---- Pass 2: emit bytecode for each instruction ----
static size_t pass2_emit_bytecode(Assembler* assembler, const char* source,
                                  byte* output, size_t max_size) {
    size_t out_off = 0;
    int line_num = 0;
    const char* p = source;

    while (*p) {
        line_num++;
        const char* nl = strchr(p, '\n');
        size_t line_len = nl ? (size_t)(nl - p) : strlen(p);

        char line[256];
        size_t clen = line_len < 255 ? line_len : 255;
        memcpy(line, p, clen);
        line[clen] = '\0';

        strip_comment(line);
        trim_trailing(line);
        char* t = skip_whitespace(line);

        if (*t) {
            // skip past label definition
            char* colon = strchr(t, ':');
            if (colon) {
                t = skip_whitespace(colon + 1);
                if (!*t) { p += line_len; if (*p == '\n') p++; continue; }
            }

            // extract mnemonic
            char mnemonic[16] = {0};
            char* sp = t;
            while (*sp && *sp != ' ' && *sp != '\t') sp++;
            size_t mlen = (size_t)(sp - t);
            if (mlen > 15) mlen = 15;
            memcpy(mnemonic, t, mlen);
            mnemonic[mlen] = '\0';
            for (size_t i = 0; mnemonic[i]; i++)
                if (mnemonic[i] >= 'a' && mnemonic[i] <= 'z') mnemonic[i] -= 32;

            int opcode = get_opcode_from_string(mnemonic);
            if (opcode < 0) {
                fprintf(stderr, "Error on line %d: unknown mnemonic '%s'\n", line_num, mnemonic);
                return 0;
            }

            char* ops = skip_whitespace(sp);
            InstrType type = get_instr_type(opcode);

            // Handle "CALL malloc" as built-in MALLOC instruction
            if (opcode == CALL) {
                char target[256];
                strncpy(target, skip_whitespace(ops), 255);
                target[255] = '\0';
                trim_trailing(target);
                for (size_t i = 0; target[i]; i++)
                    if (target[i] >= 'a' && target[i] <= 'z') target[i] -= 32;
                if (strcmp(target, "MALLOC") == 0) {
                    opcode = MALLOC;
                    type = ITYPE_NONE;
                }
            }

            byte reg_byte = 0;
            u16 imm = 0;

            if (!encode_operands(type, ops, assembler, line_num, &reg_byte, &imm))
                return 0;

            if (out_off + INSTRUCTION_SIZE > max_size) {
                fprintf(stderr, "Error: program too large\n");
                return 0;
            }
            output[out_off]     = (byte)opcode;
            output[out_off + 1] = reg_byte;
            output[out_off + 2] = (imm >> 8) & 0xFF;
            output[out_off + 3] = imm & 0xFF;
            out_off += INSTRUCTION_SIZE;
        }

        p += line_len;
        if (*p == '\n') p++;
    }

    return out_off;
}

size_t assemble(const char* source, byte* output, size_t max_size) {
    Assembler assembler = {0};

    if (!pass1_collect_labels(&assembler, source))
        return 0;

    return pass2_emit_bytecode(&assembler, source, output, max_size);
}