/*                       
     _____ ___ ___ ___ ___ 
    |__   |  _|  _|   |_  |     Z6502 CPU Emulator
    |   __| . |_  | | |  _|     Copyright (C) 2026 - Arnaud LE COSSEC
    |_____|___|___|___|___|     version 1.0.0
                       
    This program is free software; you can redistribute it and/or modify
    it under the terms of the MIT License.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    MIT License for more details.    
*/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "emulator_utility.h"
#include "z6502.h"

int main(int argc,char ** argv) {
    int opt;
    int verbose_flag = 0;
    int json_flag = 0;
    int stop_on_brk = 1; //Default: stop on BRK instruction
    int step_mode = 0;
    int clock_frequency = 1; //Default 1Hz

    /*Parse command line options*/
    while ((opt = getopt(argc, argv, "vjbBsf:")) != -1) {
        switch (opt) {
            case 'v':
                verbose_flag = 1;
                break;
            case 'j':
                json_flag = 1;
                break;
            case 'b':
                stop_on_brk = 1;
                break;
            case 'B':
                stop_on_brk = 0;
                break;
            case 's':
                step_mode = 1;
                break;
            case 'f':
                clock_frequency = strtol(optarg, NULL, 10);
                if(clock_frequency < 0){
                    fprintf(stderr, "Invalid clock frequency\n");
                    exit(EXIT_FAILURE);
                }
                break;
            default:
                fprintf(stderr, "Usage: %s [-v] [-j] [-s] [-f frequency] ROM_file\n", argv[0]);
                fprintf(stderr, "  -v : Verbose mode (dumps CPU state after each instruction)\n");
                fprintf(stderr, "  -j : Output CPU state in JSON format (only with -v)\n");
                fprintf(stderr, "  -b : Stop on BRK instruction (default)\n");
                fprintf(stderr, "  -B : Continue execution on BRK instruction\n");
                fprintf(stderr, "  -s : Step mode (waits for user input after each instruction)\n");
                fprintf(stderr, "  -f frequency : Set clock frequency in Hz (default: 1Hz)\n");
                exit(EXIT_FAILURE);
        }
    }
    
    if (optind >= argc) {
               fprintf(stderr, "Expected ROM file path after options\n");
               exit(EXIT_FAILURE);
    }
    
    printf("ROM file path argument = %s\n", argv[optind]);

    /*Allocate memory and io spaces*/
    uint8_t* memory_space = (uint8_t*)malloc(Z6502_MAX_MEMORY_SIZE_BYTES*sizeof(uint8_t));

    if(memory_space == NULL){
        fprintf(stderr, "[CRITICAL] Memory allocation error\n");
        exit(EXIT_FAILURE);
    }

    /*Load memory*/
    if(emu_memory_load(argv[optind], 0x0000U, memory_space, (size_t)Z6502_MAX_MEMORY_SIZE_BYTES) == 0){
        fprintf(stderr, "[ ERROR  ] Could not load ROM file\n");
        free(memory_space);
        exit(EXIT_FAILURE);
    }

    /*Create components*/
    z6502_cpu_t cpu_s;
    z6502_init(&cpu_s, memory_space);

    /*Reset CPU*/
    z6502_reset(&cpu_s);

    /*Emulation loop*/
    while(1){


        if (z6502_step(&cpu_s) != 0){
            fprintf(stderr, "[CRITICAL] Unhandled opcode 0x%02X at address 0x%04X\n", cpu_s.ir_opcode, cpu_s.ir_addr);
            free(memory_space);
            exit(EXIT_FAILURE);
        }
        

        if(verbose_flag){
            emu_cpuState_dump(&cpu_s, (uint8_t)json_flag);
        }

        if(step_mode){
            /*Start memory monitor. Blocks execution until user input*/
            printf("Press [ENTER] to execute next instruction. [h] for memory monitor commands\n");
            emu_memory_monitor(memory_space, (size_t)Z6502_MAX_MEMORY_SIZE_BYTES);
        }
        else{
            if(clock_frequency > 0) usleep(1000000 / clock_frequency);
        }

        /*Check for BRK instruction*/
        if(stop_on_brk && cpu_s.reg.processor_status.break_flg == 1U){
            printf("BRK flag set. Stopping execution.\n");
            break;
        }

    }

    free(memory_space);
    return 0;
}
