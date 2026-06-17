/*                       
     _____ ___ ___ ___ ___ 
    |__   |  _|  _|   |_  |     Z6502 CPU Emulator
    |   __| . |_  | | |  _|     Copyright (C) 2025 - Arnaud LE COSSEC
    |_____|___|___|___|___|     version 1.0.0
                       
    This program is free software; you can redistribute it and/or modify
    it under the terms of the MIT License.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    MIT License for more details.    
*/

#ifndef Z6502_CORE_H_INCLUDED
#define Z6502_CORE_H_INCLUDED

#include <stdint.h>
#include <stddef.h>

#define Z6502_MAX_MEMORY_SIZE_BYTES 65536U

#define FALSE 0U
#define TRUE 1U

#define Z6502_STACK_BASE_ADDRESS 0x0100U
#define Z6502_RESET_VECTOR_ADDRESS 0xFFFCU
#define Z6502_IRQ_VECTOR_ADDRESS 0xFFFEU

/*Addressing modes*/
typedef enum 
{
    ___, /* Undefined*/
    IMP, /* Implied */
    ACC, /* Accumulator */
    IMM, /* Immediate */
    ZP,  /* Zero Page */
    ZPX, /* Zero Page,X */
    ZPY, /* Zero Page,Y */
    REL, /* Relative */
    ABS, /* Absolute */
    ABX, /* Absolute,X */
    ABY, /* Absolute,Y */
    IND, /* Indirect */
    INX, /* X-indexed, indirect - aka (Indirect,X) */
    INY, /* Indirect, Y-indexed	- aka (Indirect),Y */
} z6502_addressing_mode_t;

/*Status indicator flags structure*/
typedef struct
{
    uint8_t carry;
    uint8_t zero;
    uint8_t irq_disable;
    uint8_t decimal_mode;
    uint8_t break_flg;
    uint8_t overflow;
    uint8_t negative;
} z6502_flag_t;

/*Register set structure*/
typedef struct
{
    uint16_t program_counter;
    uint16_t stack_pointer;
    uint8_t accumulator;
    uint8_t x;
    uint8_t y;
    z6502_flag_t processor_status;
} z6502_register_set_t;

/*CPU structure*/
typedef struct
{
    /*Registers*/
    z6502_register_set_t reg;
    
    /*Memory*/
    uint8_t* memory_ptr;

    /*Instruction register*/
    uint8_t ir_opcode;
    uint16_t ir_addr;
    z6502_addressing_mode_t ir_mode;
    int ir_cycles;

    /*Misc. flag*/
    uint8_t page_crossing_flag;
} z6502_cpu_t;



/*Instruction function prototypes*/
void z6502_op_ADC(z6502_cpu_t* cpu_s);
void z6502_op_AND(z6502_cpu_t* cpu_s);
void z6502_op_ASL(z6502_cpu_t* cpu_s);
void z6502_op_BCC(z6502_cpu_t* cpu_s);
void z6502_op_BCS(z6502_cpu_t* cpu_s);
void z6502_op_BEQ(z6502_cpu_t* cpu_s);
void z6502_op_BIT(z6502_cpu_t* cpu_s);
void z6502_op_BMI(z6502_cpu_t* cpu_s);
void z6502_op_BNE(z6502_cpu_t* cpu_s);
void z6502_op_BPL(z6502_cpu_t* cpu_s);
void z6502_op_BRK(z6502_cpu_t* cpu_s);
void z6502_op_BVC(z6502_cpu_t* cpu_s);
void z6502_op_BVS(z6502_cpu_t* cpu_s);
void z6502_op_CLC(z6502_cpu_t* cpu_s);
void z6502_op_CLD(z6502_cpu_t* cpu_s);
void z6502_op_CLI(z6502_cpu_t* cpu_s);
void z6502_op_CLV(z6502_cpu_t* cpu_s);
void z6502_op_CMP(z6502_cpu_t* cpu_s);
void z6502_op_CPX(z6502_cpu_t* cpu_s);
void z6502_op_CPY(z6502_cpu_t* cpu_s);
void z6502_op_DEC(z6502_cpu_t* cpu_s);
void z6502_op_DEX(z6502_cpu_t* cpu_s);
void z6502_op_DEY(z6502_cpu_t* cpu_s);
void z6502_op_EOR(z6502_cpu_t* cpu_s);
void z6502_op_INC(z6502_cpu_t* cpu_s);
void z6502_op_INX(z6502_cpu_t* cpu_s);
void z6502_op_INY(z6502_cpu_t* cpu_s);
void z6502_op_JMP(z6502_cpu_t* cpu_s);
void z6502_op_JSR(z6502_cpu_t* cpu_s);
void z6502_op_LDA(z6502_cpu_t* cpu_s);
void z6502_op_LDX(z6502_cpu_t* cpu_s);
void z6502_op_LDY(z6502_cpu_t* cpu_s);
void z6502_op_LSR(z6502_cpu_t* cpu_s);
void z6502_op_NOP(z6502_cpu_t* cpu_s);
void z6502_op_ORA(z6502_cpu_t* cpu_s);
void z6502_op_PHA(z6502_cpu_t* cpu_s);
void z6502_op_PHP(z6502_cpu_t* cpu_s);
void z6502_op_PLA(z6502_cpu_t* cpu_s);
void z6502_op_PLP(z6502_cpu_t* cpu_s);
void z6502_op_ROL(z6502_cpu_t* cpu_s);
void z6502_op_ROR(z6502_cpu_t* cpu_s);
void z6502_op_RTI(z6502_cpu_t* cpu_s);
void z6502_op_RTS(z6502_cpu_t* cpu_s);
void z6502_op_SBC(z6502_cpu_t* cpu_s);
void z6502_op_SEC(z6502_cpu_t* cpu_s);
void z6502_op_SED(z6502_cpu_t* cpu_s);
void z6502_op_SEI(z6502_cpu_t* cpu_s);
void z6502_op_STA(z6502_cpu_t* cpu_s);
void z6502_op_STX(z6502_cpu_t* cpu_s);
void z6502_op_STY(z6502_cpu_t* cpu_s);
void z6502_op_TAX(z6502_cpu_t* cpu_s);
void z6502_op_TAY(z6502_cpu_t* cpu_s);
void z6502_op_TSX(z6502_cpu_t* cpu_s);
void z6502_op_TXA(z6502_cpu_t* cpu_s);
void z6502_op_TXS(z6502_cpu_t* cpu_s);
void z6502_op_TYA(z6502_cpu_t* cpu_s);


typedef void (*z6502_instruction_t)(z6502_cpu_t* cpu_s);

static const z6502_instruction_t z6502_instruction_set[256] = {
    /* 0x00 - 0x0F */
    &z6502_op_BRK,  &z6502_op_ORA,   NULL,          NULL,       NULL,           &z6502_op_ORA,   &z6502_op_ASL,   NULL,   &z6502_op_PHP,    &z6502_op_ORA,  &z6502_op_ASL,  NULL,   NULL,           &z6502_op_ORA,   &z6502_op_ASL,   NULL,
    /* 0x10 - 0x1F */
    &z6502_op_BPL,  &z6502_op_ORA,   NULL,          NULL,       NULL,           &z6502_op_ORA,   &z6502_op_ASL,   NULL,   &z6502_op_CLC,    &z6502_op_ORA,  NULL,           NULL,   NULL,           &z6502_op_ORA,   &z6502_op_ASL,   NULL,
    /* 0x20 - 0x2F */
    &z6502_op_JSR,  &z6502_op_AND,   NULL,          NULL,       &z6502_op_BIT,  &z6502_op_AND,   &z6502_op_ROL,   NULL,   &z6502_op_PLP,    &z6502_op_AND,  &z6502_op_ROL,  NULL,   &z6502_op_BIT,  &z6502_op_AND,   &z6502_op_ROL,   NULL,
    /* 0x30 - 0x3F */
    &z6502_op_BMI,  &z6502_op_AND,   NULL,          NULL,       NULL,           &z6502_op_AND,   &z6502_op_ROL,   NULL,   &z6502_op_SEC,    &z6502_op_AND,  NULL,           NULL,   NULL,           &z6502_op_AND,   &z6502_op_ROL,   NULL,
    /* 0x40 - 0x4F */
    &z6502_op_RTI,  &z6502_op_EOR,   NULL,	        NULL,       NULL,           &z6502_op_EOR,   &z6502_op_LSR,   NULL,   &z6502_op_PHA,    &z6502_op_EOR,  &z6502_op_LSR,  NULL,	&z6502_op_JMP,  &z6502_op_EOR,   &z6502_op_LSR,   NULL,
    /* 0x50 - 0x5F */
    &z6502_op_BVC,  &z6502_op_EOR,   NULL,	        NULL,	    NULL,           &z6502_op_EOR,   &z6502_op_LSR,   NULL,   &z6502_op_CLI,	&z6502_op_EOR,  NULL,	        NULL,	NULL,	        &z6502_op_EOR,   &z6502_op_LSR,   NULL,
    /* 0x60 - 0x6F */
    &z6502_op_RTS,	&z6502_op_ADC,   NULL,	        NULL,	    NULL,	        &z6502_op_ADC,   &z6502_op_ROR,   NULL,   &z6502_op_PLA,	&z6502_op_ADC,  &z6502_op_ROR,  NULL,	&z6502_op_JMP,  &z6502_op_ADC,   &z6502_op_ROR,   NULL,
    /* 0x70 - 0x7F */
    &z6502_op_BVS,  &z6502_op_ADC,   NULL,	        NULL,	    NULL,	        &z6502_op_ADC,   &z6502_op_ROR,   NULL,   &z6502_op_SEI,	&z6502_op_ADC,  NULL,	        NULL,   NULL,	        &z6502_op_ADC,   &z6502_op_ROR,   NULL,
    /* 0x80 - 0x8F */
    NULL,           &z6502_op_STA,   NULL,	        NULL,	    &z6502_op_STY,  &z6502_op_STA,   &z6502_op_STX,   NULL,   &z6502_op_DEY,	NULL,	        &z6502_op_TXA,  NULL,	&z6502_op_STY,  &z6502_op_STA,   &z6502_op_STX,   NULL,
    /* 0x90 - 0x9F */
    &z6502_op_BCC,	&z6502_op_STA,   NULL,	        NULL,	    &z6502_op_STY,  &z6502_op_STA,   &z6502_op_STX,   NULL,   &z6502_op_TYA,	&z6502_op_STA,  &z6502_op_TXS,  NULL,	NULL,	        &z6502_op_STA,   NULL,            NULL,
    /* 0xA0 - 0xAF */
    &z6502_op_LDY,  &z6502_op_LDA,   &z6502_op_LDX, NULL,	    &z6502_op_LDY,  &z6502_op_LDA,   &z6502_op_LDX,   NULL,   &z6502_op_TAY,	&z6502_op_LDA,  &z6502_op_TAX,  NULL,	&z6502_op_LDY,  &z6502_op_LDA,   &z6502_op_LDX,   NULL,
    /* 0xB0 - 0xBF */
    &z6502_op_BCS,  &z6502_op_LDA,   NULL,	        NULL,	    &z6502_op_LDY,  &z6502_op_LDA,   &z6502_op_LDX,   NULL,   &z6502_op_CLV,	&z6502_op_LDA,  &z6502_op_TSX,  NULL,	&z6502_op_LDY,  &z6502_op_LDA,   &z6502_op_LDX,   NULL,
    /* 0xC0 - 0xCF */
    &z6502_op_CPY,  &z6502_op_CMP,   NULL,	        NULL,	    &z6502_op_CPY,  &z6502_op_CMP,   &z6502_op_DEC,   NULL,   &z6502_op_INY,	&z6502_op_CMP,  &z6502_op_DEX,  NULL,	&z6502_op_CPY,  &z6502_op_CMP,   &z6502_op_DEC,   NULL,
    /* 0xD0 - 0xDF */
    &z6502_op_BNE,	&z6502_op_CMP,   NULL,	        NULL,	    NULL,	        &z6502_op_CMP,   &z6502_op_DEC,   NULL,   &z6502_op_CLD,	&z6502_op_CMP,  NULL,	        NULL,	NULL,	        &z6502_op_CMP,   &z6502_op_DEC,   NULL,
    /* 0xE0 - 0xEF */
    &z6502_op_CPX,  &z6502_op_SBC,   NULL,	        NULL,	    &z6502_op_CPX,  &z6502_op_SBC,   &z6502_op_INC,   NULL,   &z6502_op_INX,	&z6502_op_SBC,  &z6502_op_NOP,  NULL,	&z6502_op_CPX,  &z6502_op_SBC,   &z6502_op_INC,   NULL,
    /* 0xF0 - 0xFF */
    &z6502_op_BEQ,	&z6502_op_SBC,   NULL,	        NULL,	    NULL,	        &z6502_op_SBC,   &z6502_op_INC,   NULL,   &z6502_op_SED,	&z6502_op_SBC,  NULL,	        NULL,	NULL,	        &z6502_op_SBC,   &z6502_op_INC,   NULL,
};

static const int z6502_instruction_cycles[256] = {
    /* 0x00 - 0x0F */
    7, 6, 0, 0, 0, 3, 5, 0, 3, 2, 2, 0, 0, 4, 6, 0,
    /* 0x10 - 0x1F */
    2, 5, 0, 0, 0, 4, 6, 0, 2, 4, 0, 0, 0, 4, 7, 0,
    /* 0x20 - 0x2F */
    6, 6, 0, 0, 3, 3, 5, 0, 4, 2, 2, 0, 4, 4, 6, 0,
    /* 0x30 - 0x3F */
    2, 5, 0, 0, 0, 4, 6, 0, 2, 4, 0, 0, 0, 4, 7, 0,
    /* 0x40 - 0x4F */
    6, 6, 0, 0, 0, 3, 5, 0, 3, 2, 2, 0, 3, 4, 6, 0,
    /* 0x50 - 0x5F */
    2, 5, 0, 0, 0, 4, 6, 0, 2, 4, 0, 0, 0, 4, 7, 0,
    /* 0x60 - 0x6F */
    6, 6, 0, 0, 0, 3, 5, 0, 4, 2, 2, 0, 5, 4, 6, 0,
    /* 0x70 - 0x7F */
    2, 5, 0, 0, 0, 4, 6, 0, 2, 4, 0, 0, 0, 4, 7, 0,
    /* 0x80 - 0x8F */
    0, 6, 0, 0, 3, 3, 3, 0, 2, 0, 2, 0, 4, 4, 4, 0,
    /* 0x90 - 0x9F */
    2, 6, 0, 0, 4, 4, 4, 0, 2, 5, 2, 0, 0, 5, 0, 0,
    /* 0xA0 - 0xAF */
    2, 6, 2, 0, 3, 3, 3, 0, 2, 2, 2, 0, 4, 4, 4, 0,
    /* 0xB0 - 0xBF */
    2, 5, 0, 0, 4, 4, 4, 0, 2, 4, 2, 0, 4, 4, 4, 0,
    /* 0xC0 - 0xCF */
    2, 6, 0, 0, 3, 3, 5, 0, 2, 2, 2, 0, 4, 4, 6, 0,
    /* 0xD0 - 0xDF */
    2, 5, 0, 0, 0, 4, 6, 0, 2, 4, 0, 0, 0, 4, 7, 0,
    /* 0xE0 - 0xEF */
    2, 6, 0, 0, 3, 3, 5, 0, 2, 2, 2, 2, 4, 4, 6, 0,
    /* 0xF0 - 0xFF */
    2, 5, 0, 0, 0, 4, 6, 0, 2, 4, 0, 0, 0, 4, 7, 0,
};

static const z6502_addressing_mode_t z6502_instruction_mode[256] = {
    /* 0x00 - 0x0F */
    IMP, INX, ___, ___, ___, ZP,  ZP,  ___, IMP, IMM, ACC, ___, ___, ABS, ABS, ___,
    /* 0x10 - 0x1F */
    REL, INY, ___, ___, ___, ZPX, ZPX, ___, IMP, ABY, ___, ___, ___, ABX, ABX, ___,
    /* 0x20 - 0x2F */
    ABS, INX, ___, ___, ZP,  ZP,  ZP,  ___, IMP, IMM, ACC, ___, ABS, ABS, ABS, ___,
    /* 0x30 - 0x3F */
    REL, INY, ___, ___, ___, ZPX, ZPX, ___, IMP, ABY, ___, ___, ___, ABX, ABX, ___,
    /* 0x40 - 0x4F */
    IMP, INX, ___, ___, ___, ZP,  ZP,  ___, IMP, IMM, ACC, ___, ABS, ABS, ABS, ___,
    /* 0x50 - 0x5F */
    REL, INY, ___, ___, ___, ZPX, ZPX, ___, IMP, ABY, ___, ___, ___, ABX, ABX, ___,
    /* 0x60 - 0x6F */
    IMP, INX, ___, ___, ___, ZP,  ZP,  ___, IMP, IMM, ACC, ___, IND, ABS, ABS, ___,
    /* 0x70 - 0x7F */
    REL, INY, ___, ___, ___, ZPX, ZPX, ___, IMP, ABY, ___, ___, ___, ABX, ABX, ___,
    /* 0x80 - 0x8F */
    ___, INX, ___, ___, ZP,  ZP,  ZP,  ___, IMP, ___, IMP, ___, ABS, ABS, ABS, ___,
    /* 0x90 - 0x9F */
    REL, INY, ___, ___, ZPX, ZPX, ZPY, ___, IMP, ABY, IMP, ___, ___, ABX, ___, ___,
    /* 0xA0 - 0xAF */
    IMM, INX, IMM, ___, ZP,  ZP,  ZP,  ___, IMP, IMM, IMP, ___, ABS, ABS, ABS, ___,
    /* 0xB0 - 0xBF */
    REL, INY, ___, ___, ZPX, ZPX, ZPY, ___, IMP, ABY, IMP, ___, ABX, ABX, ABY, ___,
    /* 0xC0 - 0xCF */
    IMM, INX, ___, ___, ZP,  ZP,  ZP,  ___, IMP, IMM, IMP, ___, ABS, ABS, ABS, ___,
    /* 0xD0 - 0xDF */
    REL, INY, ___, ___, ___, ZPX, ZPX, ___, IMP, ABY, ___, ___, ___, ABX, ABX, ___,
    /* 0xE0 - 0xEF */
    IMM, INX, ___, ___, ZP,  ZP,  ZP,  ___, IMP, IMM, IMP, ___, ABS, ABS, ABS, ___,
    /* 0xF0 - 0xFF */
    REL, INY, ___, ___, ___, ZPX, ZPX, ___, IMP, ABY, ___, ___, ___, ABX, ABX, ___,
};

static const char* z6502_instruction_mnemonic[256] = {
    /* 0x00 - 0x0F */
    "BRK", "ORA", "---", "---", "---", "ORA", "ASL", "---", "PHP", "ORA", "ASL", "---", "---", "ORA", "ASL", "---",
    /* 0x10 - 0x1F */
    "BPL", "ORA", "---", "---", "---", "ORA", "ASL", "---", "CLC", "ORA", "---", "---", "---", "ORA", "ASL", "---",
    /* 0x20 - 0x2F */
    "JSR", "AND", "---", "---", "BIT", "AND", "ROL", "---", "PLP", "AND", "ROL", "---", "BIT", "AND", "ROL", "---",
    /* 0x30 - 0x3F */
    "BMI", "AND", "---", "---", "---", "AND", "ROL", "---", "SEC", "AND", "---", "---", "---", "AND", "ROL", "---",
    /* 0x40 - 0x4F */
    "RTI", "EOR", "---", "---", "---", "EOR", "LSR", "---", "PHA", "EOR", "LSR", "---", "JMP", "EOR", "LSR", "---",
    /* 0x50 - 0x5F */
    "BVC", "EOR", "---", "---", "---", "EOR", "LSR", "---", "CLI", "EOR", "---", "---", "---", "EOR", "LSR", "---",
    /* 0x60 - 0x6F */
    "RTS", "ADC", "---", "---", "---", "ADC", "ROR", "---", "PLA", "ADC", "ROR", "---", "JMP", "ADC", "ROR", "---",
    /* 0x70 - 0x7F */
    "BVS", "ADC", "---", "---", "---", "ADC", "ROR", "---", "SEI", "ADC", "---", "---", "---", "ADC", "ROR", "---",
    /* 0x80 - 0x8F */
    "---", "STA", "---", "---", "STY", "STA", "STX", "---", "DEY", "---", "TXA", "---", "STY", "STA", "STX", "---",
    /* 0x90 - 0x9F */
    "BCC", "STA", "---", "---", "STY", "STA", "STX", "---", "TYA", "STA", "TXS", "---", "---", "STA", "---", "---",
    /* 0xA0 - 0xAF */
    "LDY", "LDA", "LDX", "---", "LDY", "LDA", "LDX", "---", "TAY", "LDA", "TAX", "---", "LDY", "LDA", "LDX", "---",
    /* 0xB0 - 0xBF */
    "BCS", "LDA", "---", "---", "LDY", "LDA", "LDX", "---", "CLV", "LDA", "TSX", "---", "LDY", "LDA", "LDX", "---",
    /* 0xC0 - 0xCF */
    "CPY", "CMP", "---", "---", "CPY", "CMP", "DEC", "---", "INY", "CMP", "DEX", "---", "CPY", "CMP", "DEC", "---",
    /* 0xD0 - 0xDF */
    "BNE", "CMP", "---", "---", "---", "CMP", "DEC", "---", "CLD", "CMP", "---", "---", "---", "CMP", "DEC", "---",
    /* 0xE0 - 0xEF */
    "CPX", "SBC", "---", "---", "CPX", "SBC", "INC", "---", "INX", "SBC", "NOP", "---", "CPX", "SBC", "INC", "---",
    /* 0xF0 - 0xFF */
    "BEQ", "SBC", "---", "---", "---", "SBC", "INC", "---", "SED", "SBC", "---", "---", "---", "SBC", "INC", "---",
};

static const char* z6502_addressing_mode_str[] = {
    "___", "IMP", "ACC", "IMM", "ZP", "ZPX", "ZPY", "REL",
    "ABS", "ABX", "ABY", "IND", "INX", "INY"
};


/*CPU workers prototypes*/

/**
 * @brief Init CPU
 * @param cpu_s CPU structure pointer
 * @param memory_ptr memory pointer
 */
void z6502_init(z6502_cpu_t* cpu_s, uint8_t* memory_ptr);

/**
 * @brief Reset CPU registers
 * @param cpu_s CPU structure pointer
 */
void z6502_reset(z6502_cpu_t* cpu_s);

/**
 * @brief execute one instruction from memory at program counter
 * @param cpu_s CPU structure pointer
 * @returns number of clock cycles spent
 */
int z6502_step(z6502_cpu_t* cpu_s);

/**
 * @brief Get instruction mnemonic from opcode
 * @param cpu_s CPU structure pointer
 * @returns Pointer to mnemonic string
 */
const char* z6502_get_instruction_mnemonic(z6502_cpu_t* cpu_s);

/**
 * @brief Get instruction mnemonic from opcode
 * @param cpu_s CPU structure pointer
 * @returns Pointer to mnemonic string
 */
const char* z6502_get_addressing_mode_str(z6502_cpu_t* cpu_s);

#endif // Z6502_CORE_H_INCLUDED