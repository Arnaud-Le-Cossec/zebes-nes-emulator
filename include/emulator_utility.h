#include <stdint.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include "z6502.h"

#define EMU_LINE_BUFFER_SIZE 80
#define EMU_MEMMON_ROWSIZE_DEFAULT 16
#define EMU_MEMMON_PAGESIZE_DEFAULT 256

/**
 * @brief Load memory from dumpfile
 * @param filename Dumpfile file name
 * @param address Memory start address
 * @param memory_ptr Load target
 * @param memory_size Max memory size
 * @returns Number of bytes loaded. 0 if error
 */
size_t emu_memory_load(char* filename, uint16_t start_address, uint8_t* memory_ptr, size_t memory_size);


/**
 * @brief Dump memory to console
 * @param memory_ptr Memory pointer
 * @param start_address Memory start address
 * @param length Number of bytes to dump
 */
void emu_memory_dump(uint8_t* memory_ptr, size_t memory_size, uint16_t start_address, uint16_t length);

/**
 * @brief Execute memory monitor in console
 * @param memory_ptr Memory space pointer
 * @param memory_size Memory space size
 * @returns None
 */
void emu_memory_monitor(uint8_t* memory_ptr, size_t memory_size);

/**
 * @brief Dump CPU state to console
 * @param cpu Pointer to Z6502 CPU instance
 */
void emu_cpuState_dump(z6502_cpu_t* cpu_s, uint8_t json_flag);