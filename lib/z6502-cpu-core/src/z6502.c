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

#include "z6502.h"

//*****************************************************************************
// Private functions
//*****************************************************************************

/**
 * @brief Get operand based on addressing mode
 * @param mem Pointer to memory space
 * @param reg Pointer to register set
 * @param mode Addressing mode
 * @return Operand address or value
 */
uint16_t _get_operand(z6502_cpu_t* cpu_s){
    uint16_t lo = 0U;
    uint16_t hi = 0U;
    uint16_t operand = 0U;
    switch (cpu_s->ir_mode)
    {
        case IMP:
            /*No operand*/
            return 0;
        case ACC:
            /*No operand*/
            return 0;
        case IMM:
            /*Return 8 bit value*/
            operand = cpu_s->memory_ptr[cpu_s->reg.program_counter];
            cpu_s->reg.program_counter++;
            return operand;
        case ZP:
            /*Return address in zero page (0x0000-0x00FF)*/
            operand = cpu_s->memory_ptr[cpu_s->reg.program_counter];
            cpu_s->reg.program_counter++;
            return operand;
        case ZPX:
            /*Return address in zero page (0x0000-0x00FF), indexed by X*/
            operand = (cpu_s->memory_ptr[cpu_s->reg.program_counter] + cpu_s->reg.x) % 256;
            cpu_s->reg.program_counter++;
            return operand;
        case ZPY:
            /*Return address in zero page (0x0000-0x00FF), indexed by Y*/
            operand = (cpu_s->memory_ptr[cpu_s->reg.program_counter] + cpu_s->reg.y) % 256;
            cpu_s->reg.program_counter++;
            return operand;
        case REL:
            /*Return branch offset value*/
            operand = cpu_s->memory_ptr[cpu_s->reg.program_counter];
            cpu_s->reg.program_counter++;
            return operand;
        case ABS:
            /*Return absolute address*/
            lo = cpu_s->memory_ptr[cpu_s->reg.program_counter];
            hi = cpu_s->memory_ptr[cpu_s->reg.program_counter + 1];
            operand = (hi << 8) | lo;
            cpu_s->reg.program_counter += 2;
            return operand;
        case ABX:
            /*Return absolute address, indexed by X*/
            lo = cpu_s->memory_ptr[cpu_s->reg.program_counter];
            hi = cpu_s->memory_ptr[cpu_s->reg.program_counter + 1];
            operand = (((hi << 8) | lo) + cpu_s->reg.x) % 65536;
            if((lo + cpu_s->reg.x) > 0x00FF) cpu_s->page_crossing_flag = 1;
            cpu_s->reg.program_counter += 2;
            return operand;
        case ABY:
            /*Return absolute address, indexed by Y*/
            lo = cpu_s->memory_ptr[cpu_s->reg.program_counter];
            hi = cpu_s->memory_ptr[cpu_s->reg.program_counter + 1];
            operand = (((hi << 8) | lo) + cpu_s->reg.y) % 65536;
            if((lo + cpu_s->reg.y) > 0x00FF) cpu_s->page_crossing_flag = 1;
            cpu_s->reg.program_counter += 2;
            return operand;
        case IND:
            /*Return indirect address*/
            lo = cpu_s->memory_ptr[cpu_s->reg.program_counter];
            hi = cpu_s->memory_ptr[cpu_s->reg.program_counter + 1];
            operand = (hi << 8) | lo;
            cpu_s->reg.program_counter += 2;
            return cpu_s->memory_ptr[operand] | (cpu_s->memory_ptr[(operand + 1) % 65536] << 8);
        case INX:
            /*Return X-indexed indirect address*/
            operand = (cpu_s->memory_ptr[cpu_s->reg.program_counter] + cpu_s->reg.x) % 256;
            lo = cpu_s->memory_ptr[operand];
            hi = cpu_s->memory_ptr[(operand + 1) % 256];
            operand = (hi << 8) | lo;
            cpu_s->reg.program_counter++;
            return operand;
        case INY:
            /*Return Indirect Y-indexed address*/
            operand = cpu_s->memory_ptr[cpu_s->reg.program_counter];
            lo = cpu_s->memory_ptr[operand];
            hi = cpu_s->memory_ptr[(operand + 1) % 256];
            operand = ((hi << 8) | lo) + cpu_s->reg.y;
            if((lo + cpu_s->reg.y) > 0x00FF) cpu_s->page_crossing_flag = 1;
            cpu_s->reg.program_counter++;
            return operand;
        default:
            return 0;
    }
}

/**
 * @brief Update zero flag
 * @param reg Pointer to register set
 * @param value Value to check
 */
void _update_zero_flag(z6502_register_set_t* reg, uint8_t value){
    if(value == 0U){
        reg->processor_status.zero = 1U;
    }
    else{
        reg->processor_status.zero = 0U;
    }
}

/**
 * @brief Update negative flag
 * @param reg Pointer to register set
 * @param value Value to check
 */
void _update_negative_flag(z6502_register_set_t* reg, uint8_t value){
    reg->processor_status.negative = (value >> 7) & 0x01;
}

/**
 * @brief Update carry flag
 * @param reg Pointer to register set
 * @param value Value to check (uint16_t)
 */
void _update_carry_flag(z6502_register_set_t* reg, uint16_t value){
    if(value > 0xFF){
        reg->processor_status.carry = 1U;
    }
    else{
        reg->processor_status.carry = 0U;
    }
}

/**
 * @brief Update overflow flag
 * @param reg Pointer to register set
 * @param a First operand
 * @param b Second operand
 * @param result Result of the operation
 */
void _update_overflow_flag(z6502_register_set_t* reg, uint8_t a, uint8_t b, uint8_t result){
    if(((a ^ result) & (b ^ result) & 0x80) != 0U){
        reg->processor_status.overflow = 1U;
    }
    else{
        reg->processor_status.overflow = 0U;
    }
}

/**
 * @brief Pull a byte from the stack
 * @param mem Pointer to memory space
 * @param reg Pointer to register set
 * @param value Pointer to store the pulled value
 */
void _pull_stack(uint8_t* mem, z6502_register_set_t* reg, uint8_t* value){
    reg->stack_pointer = (reg->stack_pointer + 1U) % 256;
    *value = mem[Z6502_STACK_BASE_ADDRESS + reg->stack_pointer];
}

/**
 * @brief Push a byte onto the stack
 * @param mem Pointer to memory space
 * @param reg Pointer to register set
 * @param value Value to push onto the stack
 */
void _push_stack(uint8_t* mem, z6502_register_set_t* reg, uint8_t value){
    mem[Z6502_STACK_BASE_ADDRESS + reg->stack_pointer] = value;
    reg->stack_pointer = (reg->stack_pointer - 1U) % 256;
}

/**
 * @brief Pull processor status from the stack
 * @param mem Pointer to memory space
 * @param reg Pointer to register set
 */
void _pull_register_stack(uint8_t* mem, z6502_register_set_t* reg){
    uint8_t tmp;
    reg->stack_pointer = (reg->stack_pointer + 1U) % 256;
    tmp = mem[Z6502_STACK_BASE_ADDRESS + reg->stack_pointer];
    reg->processor_status.negative = (tmp >> 7) & 0x01;
    reg->processor_status.overflow = (tmp >> 6) & 0x01;
    reg->processor_status.decimal_mode = (tmp >> 3) & 0x01;
    reg->processor_status.irq_disable = (tmp >> 2) & 0x01;
    reg->processor_status.zero = (tmp >> 1) & 0x01;
    reg->processor_status.carry = tmp & 0x01;
}

/**
 * @brief Push processor status onto the stack
 * @param mem Pointer to memory space
 * @param reg Pointer to register set
 */
void _push_register_stack(uint8_t* mem, z6502_register_set_t* reg){
    mem[Z6502_STACK_BASE_ADDRESS+ reg->stack_pointer] = (uint8_t)(
        reg->processor_status.negative << 7 |
        reg->processor_status.overflow << 6 |
        1 << 5 |
        1 << 4 |
        reg->processor_status.decimal_mode << 3 |
        reg->processor_status.irq_disable << 2 |
        reg->processor_status.zero << 1 |
        reg->processor_status.carry);
    reg->stack_pointer = (reg->stack_pointer - 1U) % 256;
}

int _check_page_crossing(uint16_t start_addr, uint16_t end_addr){
    return ((start_addr & 0xFF00) == (end_addr & 0xFF00));
}

//*****************************************************************************
// Instruction implementations
//*****************************************************************************

void z6502_op_ADC(z6502_cpu_t* cpu_s){
    uint8_t tmp;
    uint16_t res;
    if(cpu_s->ir_mode == IMM){
        tmp = _get_operand(cpu_s);
    }
    else{
        tmp = cpu_s->memory_ptr[_get_operand(cpu_s)];
    }

    /*Only set in ABX, ABY and INY modes if page crossed*/
    cpu_s->ir_cycles += cpu_s->page_crossing_flag;

    if(cpu_s->reg.processor_status.decimal_mode){
        /*Decimal mode*/
        res = (cpu_s->reg.accumulator & 0x0F) + (tmp & 0x0F) + cpu_s->reg.processor_status.carry;
        if (res>= 0x0A) res = ((res+0x06) & 0x0F) + 0x10;
        res = (cpu_s->reg.accumulator & 0xF0) + (tmp & 0xF0) + res;
        if (res >= 0xA0) res = res + 0x60;
    }
    else{
        /*Binary mode*/
        res = cpu_s->reg.accumulator + tmp + cpu_s->reg.processor_status.carry;
    }

    _update_overflow_flag(&cpu_s->reg, cpu_s->reg.accumulator, tmp, res);
    _update_carry_flag(&cpu_s->reg, res);
    cpu_s->reg.accumulator = (uint8_t)(res % 256);
    _update_zero_flag(&cpu_s->reg, cpu_s->reg.accumulator);
    _update_negative_flag(&cpu_s->reg, cpu_s->reg.accumulator);
}
void z6502_op_AND(z6502_cpu_t* cpu_s){
    if (cpu_s->ir_mode == IMM) {
        cpu_s->reg.accumulator &= (uint8_t)_get_operand(cpu_s);
    }
    else{
        cpu_s->reg.accumulator &= cpu_s->memory_ptr[_get_operand(cpu_s)];
    }

    /*Only set in ABX, ABY and INY modes if page crossed*/
    cpu_s->ir_cycles += cpu_s->page_crossing_flag;

    _update_zero_flag(&cpu_s->reg, cpu_s->reg.accumulator);
    _update_negative_flag(&cpu_s->reg, cpu_s->reg.accumulator);
}
void z6502_op_ASL(z6502_cpu_t* cpu_s){
    uint16_t addr;
    if (cpu_s->ir_mode == ACC) {
        cpu_s->reg.processor_status.carry = (cpu_s->reg.accumulator >> 7) & 0x01;
        cpu_s->reg.accumulator = (cpu_s->reg.accumulator << 1);
        _update_zero_flag(&cpu_s->reg, cpu_s->reg.accumulator);
        _update_negative_flag(&cpu_s->reg, cpu_s->reg.accumulator);
    }
    else{
        addr = _get_operand(cpu_s);
        cpu_s->reg.processor_status.carry = (cpu_s->memory_ptr[addr] >> 7) & 0x01;
        cpu_s->memory_ptr[addr] = (cpu_s->memory_ptr[addr] << 1);
        _update_zero_flag(&cpu_s->reg, cpu_s->memory_ptr[addr]);
        _update_negative_flag(&cpu_s->reg, cpu_s->memory_ptr[addr]);
    }
}
void z6502_op_BCC(z6502_cpu_t* cpu_s){
    int8_t addr = _get_operand(cpu_s);
    if (cpu_s->reg.processor_status.carry == 0U){
        uint16_t tmp = (cpu_s->reg.program_counter + addr) % 65536;
        cpu_s->ir_cycles += _check_page_crossing(cpu_s->reg.program_counter, tmp) + 1;
        cpu_s->reg.program_counter = tmp;
    }
}
void z6502_op_BCS(z6502_cpu_t* cpu_s){
    int8_t addr = _get_operand(cpu_s);
    if (cpu_s->reg.processor_status.carry == 1U){
        uint16_t tmp = (cpu_s->reg.program_counter + addr) % 65536;
        cpu_s->ir_cycles += _check_page_crossing(cpu_s->reg.program_counter, tmp) + 1;
        cpu_s->reg.program_counter = tmp;
    }
}
void z6502_op_BEQ(z6502_cpu_t* cpu_s){
    int8_t addr = _get_operand(cpu_s);
    if (cpu_s->reg.processor_status.zero == 1U){
        uint16_t tmp = (cpu_s->reg.program_counter + addr) % 65536;
        cpu_s->ir_cycles += _check_page_crossing(cpu_s->reg.program_counter, tmp) + 1;
        cpu_s->reg.program_counter = tmp;
    }
}
void z6502_op_BIT(z6502_cpu_t* cpu_s){
    uint8_t tmp = cpu_s->memory_ptr[_get_operand(cpu_s)];
    _update_zero_flag(&cpu_s->reg, cpu_s->reg.accumulator & tmp);
    _update_negative_flag(&cpu_s->reg, tmp);
    cpu_s->reg.processor_status.overflow = (tmp >> 6) & 0x01;
}
void z6502_op_BMI(z6502_cpu_t* cpu_s){
    int8_t addr = _get_operand(cpu_s);
    if (cpu_s->reg.processor_status.negative == 1U){
        uint16_t tmp = (cpu_s->reg.program_counter + addr) % 65536;
        cpu_s->ir_cycles += _check_page_crossing(cpu_s->reg.program_counter, tmp) + 1;
        cpu_s->reg.program_counter = tmp;
    }
}
void z6502_op_BNE(z6502_cpu_t* cpu_s){
    int8_t addr = _get_operand(cpu_s);
    if (cpu_s->reg.processor_status.zero == 0U){
        uint16_t tmp = (cpu_s->reg.program_counter + addr) % 65536;
        cpu_s->ir_cycles += _check_page_crossing(cpu_s->reg.program_counter, tmp) + 1;
        cpu_s->reg.program_counter = tmp;
    }
}
void z6502_op_BPL(z6502_cpu_t* cpu_s){
    int8_t addr = _get_operand(cpu_s);
    if (cpu_s->reg.processor_status.negative == 0U){
        uint16_t tmp = (cpu_s->reg.program_counter + addr) % 65536;
        cpu_s->ir_cycles += _check_page_crossing(cpu_s->reg.program_counter, tmp) + 1;
        cpu_s->reg.program_counter = tmp;
    }
}
void z6502_op_BRK(z6502_cpu_t* cpu_s){
    uint16_t addr = (cpu_s->reg.program_counter + 2U) % 65536;
    _push_stack(cpu_s->memory_ptr, &cpu_s->reg, (uint8_t)((addr >> 8) & 0x00FF));
    _push_stack(cpu_s->memory_ptr, &cpu_s->reg, (uint8_t)(addr & 0x00FF));
    _push_register_stack(cpu_s->memory_ptr, &cpu_s->reg);
    cpu_s->reg.program_counter = Z6502_IRQ_VECTOR_ADDRESS;
    cpu_s->reg.processor_status.irq_disable = 1U;
    cpu_s->reg.processor_status.break_flg = 1U;
}
void z6502_op_BVC(z6502_cpu_t* cpu_s){
    int8_t addr = _get_operand(cpu_s);
    if (cpu_s->reg.processor_status.overflow == 0U){
        uint16_t tmp = (cpu_s->reg.program_counter + addr) % 65536;
        cpu_s->ir_cycles += _check_page_crossing(cpu_s->reg.program_counter, tmp) + 1;
        cpu_s->reg.program_counter = tmp;
    }
}
void z6502_op_BVS(z6502_cpu_t* cpu_s){
    int8_t addr = _get_operand(cpu_s);
    if (cpu_s->reg.processor_status.overflow == 1U){
        uint16_t tmp = (cpu_s->reg.program_counter + addr) % 65536;
        cpu_s->ir_cycles += _check_page_crossing(cpu_s->reg.program_counter, tmp) + 1;
        cpu_s->reg.program_counter = tmp;
    }
}
void z6502_op_CLC(z6502_cpu_t* cpu_s){
    cpu_s->reg.processor_status.carry = 0U;
}
void z6502_op_CLD(z6502_cpu_t* cpu_s){
    cpu_s->reg.processor_status.decimal_mode = 0U;
}
void z6502_op_CLI(z6502_cpu_t* cpu_s){
    cpu_s->reg.processor_status.irq_disable = 0U;
}
void z6502_op_CLV(z6502_cpu_t* cpu_s){
    cpu_s->reg.processor_status.overflow = 0U;
}
void z6502_op_CMP(z6502_cpu_t* cpu_s){
    int8_t tmp;
    if (cpu_s->ir_mode == IMM){
        tmp = _get_operand(cpu_s);
    }
    else{
        tmp = cpu_s->memory_ptr[_get_operand(cpu_s)];
    }

    /*Only set in ABX, ABY and INY modes if page crossed*/
    cpu_s->ir_cycles += cpu_s->page_crossing_flag;

    tmp = cpu_s->reg.accumulator - tmp;
    cpu_s->reg.processor_status.carry = (tmp >= 0)?1U:0U;
    cpu_s->reg.processor_status.zero = (tmp == 0)?1U:0U;
    cpu_s->reg.processor_status.negative = (tmp >> 7) & 0x01;
}
void z6502_op_CPX(z6502_cpu_t* cpu_s){
    int8_t tmp;
    if (cpu_s->ir_mode == IMM){
        tmp = _get_operand(cpu_s);
    }
    else{
        tmp = cpu_s->memory_ptr[_get_operand(cpu_s)];
    }
    tmp = cpu_s->reg.x - tmp;
    cpu_s->reg.processor_status.carry = (tmp >= 0)?1U:0U;
    cpu_s->reg.processor_status.zero = (tmp == 0)?1U:0U;
    cpu_s->reg.processor_status.negative = (tmp >> 7) & 0x01;
}
void z6502_op_CPY(z6502_cpu_t* cpu_s){
    int8_t tmp;
    if (cpu_s->ir_mode == IMM){
        tmp = _get_operand(cpu_s);
    }
    else{
        tmp = cpu_s->memory_ptr[_get_operand(cpu_s)];
    }
    tmp = cpu_s->reg.y - tmp;
    cpu_s->reg.processor_status.carry = (tmp >= 0)?1U:0U;
    cpu_s->reg.processor_status.zero = (tmp == 0)?1U:0U;
    cpu_s->reg.processor_status.negative = (tmp >> 7) & 0x01;
}
void z6502_op_DEC(z6502_cpu_t* cpu_s){
    uint16_t addr = _get_operand(cpu_s);
    cpu_s->memory_ptr[addr] = (cpu_s->memory_ptr[addr] - 1U) % 256;
    _update_zero_flag(&cpu_s->reg, cpu_s->memory_ptr[addr]);
    _update_negative_flag(&cpu_s->reg, cpu_s->memory_ptr[addr]);
}
void z6502_op_DEX(z6502_cpu_t* cpu_s){
    cpu_s->reg.x = (cpu_s->reg.x - 1U) % 256;
    _update_zero_flag(&cpu_s->reg, cpu_s->reg.x);
    _update_negative_flag(&cpu_s->reg, cpu_s->reg.x);
}
void z6502_op_DEY(z6502_cpu_t* cpu_s){
    cpu_s->reg.y = (cpu_s->reg.y - 1U) % 256;
    _update_zero_flag(&cpu_s->reg, cpu_s->reg.y);
    _update_negative_flag(&cpu_s->reg, cpu_s->reg.y);
}
void z6502_op_EOR(z6502_cpu_t* cpu_s){
    if (cpu_s->ir_mode == IMM) {
        cpu_s->reg.accumulator ^= (uint8_t)_get_operand(cpu_s);
    }
    else{
        cpu_s->reg.accumulator ^= cpu_s->memory_ptr[_get_operand(cpu_s)];
    }

    /*Only set in ABX, ABY and INY modes if page crossed*/
    cpu_s->ir_cycles += cpu_s->page_crossing_flag;

    _update_zero_flag(&cpu_s->reg, cpu_s->reg.accumulator);
    _update_negative_flag(&cpu_s->reg, cpu_s->reg.accumulator);
}
void z6502_op_INC(z6502_cpu_t* cpu_s){
    uint16_t addr = _get_operand(cpu_s);
    cpu_s->memory_ptr[addr] = (cpu_s->memory_ptr[addr] + 1U) % 256;
    _update_zero_flag(&cpu_s->reg, cpu_s->memory_ptr[addr]);
    _update_negative_flag(&cpu_s->reg, cpu_s->memory_ptr[addr]);
}
void z6502_op_INX(z6502_cpu_t* cpu_s){
    cpu_s->reg.x = (cpu_s->reg.x + 1U) % 256;
    _update_zero_flag(&cpu_s->reg, cpu_s->reg.x);
    _update_negative_flag(&cpu_s->reg, cpu_s->reg.x);
}
void z6502_op_INY(z6502_cpu_t* cpu_s){
    cpu_s->reg.y = (cpu_s->reg.y + 1U) % 256;
    _update_zero_flag(&cpu_s->reg, cpu_s->reg.y);
    _update_negative_flag(&cpu_s->reg, cpu_s->reg.y);
}
void z6502_op_JMP(z6502_cpu_t* cpu_s){
    cpu_s->reg.program_counter = _get_operand(cpu_s);
}
void z6502_op_JSR(z6502_cpu_t* cpu_s){
    uint16_t tmp = (cpu_s->reg.program_counter + 1U) % 65536;
    _push_stack(cpu_s->memory_ptr, &cpu_s->reg, (uint8_t)((tmp >> 8) & 0x00FF));
    _push_stack(cpu_s->memory_ptr, &cpu_s->reg, (uint8_t)(tmp & 0x00FF));
    cpu_s->reg.program_counter = _get_operand(cpu_s);
}
void z6502_op_LDA(z6502_cpu_t* cpu_s){
    if (cpu_s->ir_mode == IMM) {
        cpu_s->reg.accumulator = _get_operand(cpu_s);
    }
    else{
        cpu_s->reg.accumulator = cpu_s->memory_ptr[_get_operand(cpu_s)];
    }

    /*Only set in ABX, ABY and INY modes if page crossed*/
    cpu_s->ir_cycles += cpu_s->page_crossing_flag;

    _update_zero_flag(&cpu_s->reg, cpu_s->reg.accumulator);
    _update_negative_flag(&cpu_s->reg, cpu_s->reg.accumulator);
}
void z6502_op_LDX(z6502_cpu_t* cpu_s){
    if (cpu_s->ir_mode == IMM) {
        cpu_s->reg.x = _get_operand(cpu_s);
    }
    else{
        cpu_s->reg.x = cpu_s->memory_ptr[_get_operand(cpu_s)];
    }

    /*Only set in ABY mode if page crossed*/
    cpu_s->ir_cycles += cpu_s->page_crossing_flag;

    _update_zero_flag(&cpu_s->reg, cpu_s->reg.x);
    _update_negative_flag(&cpu_s->reg, cpu_s->reg.x);
}
void z6502_op_LDY(z6502_cpu_t* cpu_s){
    if (cpu_s->ir_mode == IMM) {
        cpu_s->reg.y = _get_operand(cpu_s);
    }
    else{
        cpu_s->reg.y = cpu_s->memory_ptr[_get_operand(cpu_s)];
    }

    /*Only set in ABX mode if page crossed*/
    cpu_s->ir_cycles += cpu_s->page_crossing_flag;

    _update_zero_flag(&cpu_s->reg, cpu_s->reg.y);
    _update_negative_flag(&cpu_s->reg, cpu_s->reg.y);
}
void z6502_op_LSR(z6502_cpu_t* cpu_s){
    uint16_t addr;
    if (cpu_s->ir_mode == ACC) {
        cpu_s->reg.processor_status.carry = cpu_s->reg.accumulator & 0x01;
        cpu_s->reg.accumulator = (cpu_s->reg.accumulator >> 1);
        _update_zero_flag(&cpu_s->reg, cpu_s->reg.accumulator);
        _update_negative_flag(&cpu_s->reg, cpu_s->reg.accumulator);
    }
    else{
        addr = _get_operand(cpu_s);
        cpu_s->reg.processor_status.carry = cpu_s->memory_ptr[addr] & 0x01;
        cpu_s->memory_ptr[addr] = (cpu_s->memory_ptr[addr] >> 1);
        _update_zero_flag(&cpu_s->reg, cpu_s->memory_ptr[addr]);
        _update_negative_flag(&cpu_s->reg, cpu_s->memory_ptr[addr]);
    }
}
void z6502_op_NOP(z6502_cpu_t* cpu_s){
    return;
}
void z6502_op_ORA(z6502_cpu_t* cpu_s){
    if (cpu_s->ir_mode == IMM) {
        cpu_s->reg.accumulator |= (uint8_t)_get_operand(cpu_s);
    }
    else{
        cpu_s->reg.accumulator |= cpu_s->memory_ptr[_get_operand(cpu_s)];
    }

    /*Only set in ABX, ABY and INY modes if page crossed*/
    cpu_s->ir_cycles += cpu_s->page_crossing_flag;

    _update_zero_flag(&cpu_s->reg, cpu_s->reg.accumulator);
    _update_negative_flag(&cpu_s->reg, cpu_s->reg.accumulator);
}
void z6502_op_PHA(z6502_cpu_t* cpu_s){
    _push_stack(cpu_s->memory_ptr, &cpu_s->reg, cpu_s->reg.accumulator);
}
void z6502_op_PHP(z6502_cpu_t* cpu_s){
    _push_register_stack(cpu_s->memory_ptr, &cpu_s->reg);
}
void z6502_op_PLA(z6502_cpu_t* cpu_s){
    _pull_stack(cpu_s->memory_ptr, &cpu_s->reg, &cpu_s->reg.accumulator);
    _update_zero_flag(&cpu_s->reg, cpu_s->reg.accumulator);
    _update_negative_flag(&cpu_s->reg, cpu_s->reg.accumulator);
}
void z6502_op_PLP(z6502_cpu_t* cpu_s){
    _pull_register_stack(cpu_s->memory_ptr, &cpu_s->reg);
}
void z6502_op_ROL(z6502_cpu_t* cpu_s){
    uint8_t c;
    uint16_t addr;
    if (cpu_s->ir_mode == ACC) {
        c = (cpu_s->reg.accumulator >> 7) & 0x01;
        cpu_s->reg.accumulator = (cpu_s->reg.accumulator << 1) | (cpu_s->reg.processor_status.carry);
        cpu_s->reg.processor_status.carry = c;
        _update_zero_flag(&cpu_s->reg, cpu_s->reg.accumulator);
        _update_negative_flag(&cpu_s->reg, cpu_s->reg.accumulator);
    }
    else{
        addr = _get_operand(cpu_s);
        c = (cpu_s->memory_ptr[addr] >> 7) & 0x01;
        cpu_s->memory_ptr[addr] = (cpu_s->memory_ptr[addr] << 1) | (cpu_s->reg.processor_status.carry);
        cpu_s->reg.processor_status.carry = c;
        _update_zero_flag(&cpu_s->reg, cpu_s->memory_ptr[addr]);
        _update_negative_flag(&cpu_s->reg, cpu_s->memory_ptr[addr]);
    }
}
void z6502_op_ROR(z6502_cpu_t* cpu_s){
    uint8_t c;
    uint16_t addr;
    if (cpu_s->ir_mode == ACC) {
        c = cpu_s->reg.accumulator & 0x01;
        cpu_s->reg.accumulator = (cpu_s->reg.accumulator >> 1) | (cpu_s->reg.processor_status.carry << 7);
        cpu_s->reg.processor_status.carry = c;
        _update_zero_flag(&cpu_s->reg, cpu_s->reg.accumulator);
        _update_negative_flag(&cpu_s->reg, cpu_s->reg.accumulator);
    }
    else{
        addr = _get_operand(cpu_s);
        c = cpu_s->memory_ptr[addr] & 0x01;
        cpu_s->memory_ptr[addr] = (cpu_s->memory_ptr[addr] >> 1) | (cpu_s->reg.processor_status.carry << 7);
        cpu_s->reg.processor_status.carry = c;
        _update_zero_flag(&cpu_s->reg, cpu_s->memory_ptr[addr]);
        _update_negative_flag(&cpu_s->reg, cpu_s->memory_ptr[addr]);
    }
    
}
void z6502_op_RTI(z6502_cpu_t* cpu_s){
    _pull_register_stack(cpu_s->memory_ptr, &cpu_s->reg);
    _pull_stack(cpu_s->memory_ptr, &cpu_s->reg, (uint8_t*)&cpu_s->reg.program_counter);
    _pull_stack(cpu_s->memory_ptr, &cpu_s->reg, (uint8_t*)&cpu_s->reg.program_counter + 1);
}
void z6502_op_RTS(z6502_cpu_t* cpu_s){
    _pull_stack(cpu_s->memory_ptr, &cpu_s->reg, (uint8_t*)&cpu_s->reg.program_counter);
    _pull_stack(cpu_s->memory_ptr, &cpu_s->reg, (uint8_t*)&cpu_s->reg.program_counter + 1);
    cpu_s->reg.program_counter++;
}
void z6502_op_SBC(z6502_cpu_t* cpu_s){
    uint8_t tmp;
    int16_t res;
    if(cpu_s->ir_mode == IMM){
        tmp = _get_operand(cpu_s);
    }
    else{
        tmp = cpu_s->memory_ptr[_get_operand(cpu_s)];
    }

    /*Only set in ABX, ABY and INY modes if page crossed*/
    cpu_s->ir_cycles += cpu_s->page_crossing_flag;

    if(cpu_s->reg.processor_status.decimal_mode){
        /*Decimal mode*/
        res = (cpu_s->reg.accumulator & 0x0F) - (tmp & 0x0F) - (1U - cpu_s->reg.processor_status.carry);
        if (res < 0x0) res = ((res-0x06) & 0x0F) - 0x10;
        res = (cpu_s->reg.accumulator & 0xF0) - (tmp & 0xF0) + res;
        if (res < 0x0) res = res - 0x60;
    }
    else{
        /*Binary mode*/
        res = cpu_s->reg.accumulator - tmp - (1U - cpu_s->reg.processor_status.carry);
    }

    _update_overflow_flag(&cpu_s->reg, cpu_s->reg.accumulator, ~tmp, res);
    _update_carry_flag(&cpu_s->reg, res);
    cpu_s->reg.accumulator = (uint8_t)(res % 256);
    _update_zero_flag(&cpu_s->reg, cpu_s->reg.accumulator);
    _update_negative_flag(&cpu_s->reg, cpu_s->reg.accumulator);
}
void z6502_op_SEC(z6502_cpu_t* cpu_s){
    cpu_s->reg.processor_status.carry = 1U;
}
void z6502_op_SED(z6502_cpu_t* cpu_s){
    cpu_s->reg.processor_status.decimal_mode = 1U;
}
void z6502_op_SEI(z6502_cpu_t* cpu_s){
    cpu_s->reg.processor_status.irq_disable = 1U;
}
void z6502_op_STA(z6502_cpu_t* cpu_s){
    cpu_s->memory_ptr[_get_operand(cpu_s)] = cpu_s->reg.accumulator;
}
void z6502_op_STX(z6502_cpu_t* cpu_s){
    cpu_s->memory_ptr[_get_operand(cpu_s)] = cpu_s->reg.x;
}
void z6502_op_STY(z6502_cpu_t* cpu_s){
    cpu_s->memory_ptr[_get_operand(cpu_s)] = cpu_s->reg.y;
}
void z6502_op_TAX(z6502_cpu_t* cpu_s){
    cpu_s->reg.x = cpu_s->reg.accumulator;
    _update_zero_flag(&cpu_s->reg, cpu_s->reg.x);
    _update_negative_flag(&cpu_s->reg, cpu_s->reg.x);
}
void z6502_op_TAY(z6502_cpu_t* cpu_s){
    cpu_s->reg.y = cpu_s->reg.accumulator;
    _update_zero_flag(&cpu_s->reg, cpu_s->reg.y);
    _update_negative_flag(&cpu_s->reg, cpu_s->reg.y);
}
void z6502_op_TSX(z6502_cpu_t* cpu_s){
    cpu_s->reg.x = cpu_s->reg.stack_pointer;
    _update_zero_flag(&cpu_s->reg, cpu_s->reg.x);
    _update_negative_flag(&cpu_s->reg, cpu_s->reg.x);
}
void z6502_op_TXA(z6502_cpu_t* cpu_s){
    cpu_s->reg.accumulator = cpu_s->reg.x;
    _update_zero_flag(&cpu_s->reg, cpu_s->reg.accumulator);
    _update_negative_flag(&cpu_s->reg, cpu_s->reg.accumulator);
}
void z6502_op_TXS(z6502_cpu_t* cpu_s){
    cpu_s->reg.stack_pointer = cpu_s->reg.x;
}
void z6502_op_TYA(z6502_cpu_t* cpu_s){
    cpu_s->reg.accumulator = cpu_s->reg.y;
    _update_zero_flag(&cpu_s->reg, cpu_s->reg.accumulator);
    _update_negative_flag(&cpu_s->reg, cpu_s->reg.accumulator);
}


void z6502_init(z6502_cpu_t* cpu_s, uint8_t* memory_ptr){
    cpu_s->memory_ptr = memory_ptr;
}

void z6502_reset(z6502_cpu_t* cpu_s) {
    /*Init special purpose registers*/
    cpu_s->reg.program_counter = (uint16_t)(cpu_s->memory_ptr[Z6502_RESET_VECTOR_ADDRESS+1U]<<8) | (uint16_t)(cpu_s->memory_ptr[Z6502_RESET_VECTOR_ADDRESS]);
    cpu_s->reg.stack_pointer = 0xFFU;
    cpu_s->reg.accumulator = 0U;
    cpu_s->reg.x = 0U;
    cpu_s->reg.y = 0U;

    cpu_s->reg.processor_status.carry = 0U;
    cpu_s->reg.processor_status.zero = 0U;
    cpu_s->reg.processor_status.irq_disable = 0U;
    cpu_s->reg.processor_status.decimal_mode = 0U;
    cpu_s->reg.processor_status.break_flg = 0U;
    cpu_s->reg.processor_status.overflow = 0U;
    cpu_s->reg.processor_status.negative = 0U;

    cpu_s->ir_opcode = 0U;
}

int z6502_step(z6502_cpu_t* cpu_s) {
    /*Read instruction*/
    cpu_s->ir_addr = cpu_s->reg.program_counter;
    cpu_s->ir_opcode = cpu_s->memory_ptr[cpu_s->reg.program_counter];
    cpu_s->ir_mode = z6502_instruction_mode[cpu_s->ir_opcode];
    cpu_s->ir_cycles = z6502_instruction_cycles[cpu_s->ir_opcode];
    cpu_s->page_crossing_flag = 0;
    cpu_s->reg.program_counter++;

    /*Execute instruction*/
    if(z6502_instruction_set[cpu_s->ir_opcode] != NULL){
        z6502_instruction_set[cpu_s->ir_opcode](cpu_s);
    }
    else{
        //Unhandled opcode
        return 1;
    }

    return 0;
}

const char* z6502_get_instruction_mnemonic(z6502_cpu_t* cpu_s) {
    return z6502_instruction_mnemonic[cpu_s->ir_opcode];
}

const char* z6502_get_addressing_mode_str(z6502_cpu_t* cpu_s) {
    return z6502_addressing_mode_str[z6502_instruction_mode[cpu_s->ir_opcode]];
}