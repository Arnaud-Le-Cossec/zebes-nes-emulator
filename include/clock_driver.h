/*                       
     _____     _           
    |__   |___| |_ ___ ___      ZEBES NES Emulator - Clock driver
    |   __| -_| . | -_|_ -|     Copyright (C) 2026 - Arnaud LE COSSEC
    |_____|___|___|___|___|     version 1.0.0
                       
    This program is free software; you can redistribute it and/or modify
    it under the terms of the MIT License.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    MIT License for more details.    
*/

#ifndef CLOCK_DRIVER_H_INCLUDED
#define CLOCK_DRIVER_H_INCLUDED

#define NTSC_MASTER_CLOCK_FREQUENCY_HZ 21477272 
#define PAL_MASTER_CLOCK_FREQUENCY_HZ 26601712

#define NTSC_CPU_MASTER_CLOCK_DIVIDER 12
#define PAL_CPU_MASTER_CLOCK_DIVIDER 16

#define NTSC_PPU_MASTER_CLOCK_DIVIDER 4
#define PAL_PPU_MASTER_CLOCK_DIVIDER 5

typedef struct
{
    unsigned int clock_counter;
    unsigned int clock_divider;
    unsigned int skip_clock_cycles;
    
} clock_t;

/**
 * @brief Init clock
 * @param clock_s clock structure pointer
 * @param clock_divider clock divider value
 */
void clock_init(clock_t* clock_s, unsigned int clock_divider);

/**
 * @brief Advance clock and check divider
 * @param clock_s clock structure pointer
 * @returns divider output (True/False)
 */
int clock_check(clock_t* clock_s);

/**
 * @brief Allows to skip future divider pulses
 * @param clock_s clock structure pointer
 * @param skip_clock_cycles amount of divider pulses to skip
 */
void clock_skip(clock_t* clock_s, unsigned int skip_clock_cycles);


#endif //CLOCK_DRIVER_H_INCLUDED