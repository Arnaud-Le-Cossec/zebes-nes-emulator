#include "clock_driver.h"

void clock_init(clock_t* clock_s, unsigned int clock_divider){
    clock_s->clock_divider = clock_divider;
    clock_s->clock_counter = 0U;
    clock_s->skip_clock_cycles = 0U;
}

int clock_check(clock_t* clock_s){
    clock_s->clock_counter++;
    if(clock_s->clock_counter >= clock_s->clock_divider){
        if(clock_s->skip_clock_cycles == 0U){
            clock_s->clock_counter = 0U;
            return 1;
        }
        clock_s->skip_clock_cycles --;
    }
    return 0;
}

void clock_skip(clock_t* clock_s, unsigned int skip_clock_cycles){
    clock_s->skip_clock_cycles = skip_clock_cycles;
}