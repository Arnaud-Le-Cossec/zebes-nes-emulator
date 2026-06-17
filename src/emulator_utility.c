
#include "emulator_utility.h"

size_t emu_memory_load(char* filename, uint16_t start_address, uint8_t* memory_ptr, size_t memory_size){
    size_t count;
    /*Open file*/
    FILE *file = fopen(filename, "rb");
    if(file == NULL){
        printf("Could not open file %s\n", filename);
        fclose(file);
        return 0;
    }

    /*Read file*/
    count = fread(memory_ptr, sizeof(uint8_t), memory_size, file);

    if(ferror(file) != 0){
        printf("Error reading file %s\n", filename);
        fclose(file);
        return 0;
    }
    printf("Loaded %zu bytes from file %s\n", count, filename);
    fclose(file);
    return count;
}

void emu_memory_dump(uint8_t* memory_ptr, size_t memory_size, uint16_t start_address, uint16_t length){
    char ascii[EMU_MEMMON_ROWSIZE_DEFAULT+1];
    /*Cap length if memory overflow*/
    if(start_address+length >= memory_size) length = memory_size-start_address;
    for(uint16_t i = 0; i < length; i++){
        /*Address*/
        if(i % EMU_MEMMON_ROWSIZE_DEFAULT == 0){
            printf("%04X: ", start_address + i);
        }
        /*Data*/
        printf("%02X ", memory_ptr[start_address + i]);
        /*ASCII representation*/
        ascii[i % EMU_MEMMON_ROWSIZE_DEFAULT] = isprint(memory_ptr[start_address + i]) ? (char)memory_ptr[start_address + i] : '.';
        if(i % EMU_MEMMON_ROWSIZE_DEFAULT == EMU_MEMMON_ROWSIZE_DEFAULT-1){
            ascii[EMU_MEMMON_ROWSIZE_DEFAULT] = '\0';
            printf("%s\n", ascii);
        }
        else if(i == length - 1){
            ascii[(length)%EMU_MEMMON_ROWSIZE_DEFAULT] = '\0';
            printf("%s\n", ascii);
        }
    }
    printf("\n");
}

void emu_memory_monitor(uint8_t* memory_ptr, size_t memory_size){
    char buffer[EMU_LINE_BUFFER_SIZE];
    char *buffer_idx_ptr, *buffer_end_ptr;
    char cmd=' ';
    uint32_t addr=0;
    uint32_t val=0;
    
    while (1)
    {
        printf("\n>>> ");
        if(fgets(buffer, sizeof(buffer), stdin) == NULL){
            /*EOF or error*/
            break;
        }
        
        /*Init index pointer to the start of buffer*/
        buffer_idx_ptr = buffer;

        /*Skip whitespace*/
        while(isspace(*buffer_idx_ptr)) buffer_idx_ptr++;

        /*Check for empty line*/
        if(*buffer_idx_ptr == '\0'){
            break;
        }

        /*Get command character*/
        char cmd = *buffer_idx_ptr++;

        /*Quit command*/
        if(cmd == 'Q' || cmd == 'q'){
            break;
        }

        /*Help command*/
        if(cmd == 'H' || cmd == 'h'){
            printf("Commands:\n - Exit/Next \t:=\tQ|[newline]\n - Read \t:=\tR <address(hex)>(,<page size(dec)>)\n - Write \t:=\tW <address(hex)>=<value(hex)>\n - Show help \t:=\tH");
            continue;
        }

        /*Read command*/
        else if(cmd == 'R' || cmd == 'r'){

            /*Get address*/

            addr = (uint32_t)strtol(buffer_idx_ptr, &buffer_end_ptr, 16);
            if(buffer_idx_ptr == buffer_end_ptr){
                printf("Syntax Error ! Read: R <address(hex)>\n");
                continue;
            } 
            if(addr < 0 || addr >= memory_size){
                printf("Invalid address ! Range: 0x0 - 0x%lx)\n", memory_size-1);
                continue;
            }

            /*Here 'val' is used to set page_size*/
            val = EMU_MEMMON_PAGESIZE_DEFAULT;

            /*Get optional ',' character*/

            buffer_idx_ptr=buffer_end_ptr;
            /*Skip whitespace*/
            while(isspace(*buffer_idx_ptr)) buffer_idx_ptr++;
            /*Check ',' presence*/
            if(*buffer_idx_ptr == ','){
                buffer_idx_ptr++;
                val = (uint32_t)strtol(buffer_idx_ptr, &buffer_end_ptr, 10);
                if(buffer_idx_ptr == buffer_end_ptr ){
                    printf("Syntax Error ! Read := R <address(hex)>(,<page size(dec)>)\n");
                    continue;
                }
                if(val < 0){
                    printf("Invalid page size ! Range: > 0\n");
                    continue;
                }
            }

            /*Read*/

            emu_memory_dump(memory_ptr, memory_size, addr, val);
        }

        /*Write command*/
        else if(cmd == 'W' || cmd == 'w'){

            /*Get address*/

            addr = (uint32_t)strtol(buffer_idx_ptr, &buffer_end_ptr, 16);
            if(buffer_idx_ptr == buffer_end_ptr){
                printf("Syntax Error ! Write: W <address(hex)>=<value(hex)>\n");
                continue;
            }
            if(addr < 0 || addr >= memory_size){
                printf("Invalid address ! Range: 0x0 - 0x%lx)\n", memory_size-1);
                continue;
            }

            /*Get '=' character*/

            buffer_idx_ptr=buffer_end_ptr;
            /*Skip whitespace*/
            while(isspace(*buffer_idx_ptr)) buffer_idx_ptr++;
            /*Check '=' presence*/
            if(*buffer_idx_ptr != '='){
                printf("Syntax Error ! Write: W <address(hex)>=<value(hex)>\n");
                continue;
            }

            /*Get value to be written*/

            buffer_idx_ptr++;
            val = (uint32_t)strtol(buffer_idx_ptr, &buffer_end_ptr, 16);
            if(buffer_idx_ptr == buffer_end_ptr){
                printf("Syntax Error ! Write: W <address(hex)>=<value(hex)>3\n");
                continue;
            }
        
            /*Write*/

            memory_ptr[(size_t)addr] = (uint32_t)val;
        }
        else{
            printf("Unknown command '%c'\n", cmd);
        }
    }
}

void emu_cpuState_dump(z6502_cpu_t* cpu_s, uint8_t json_flag){
    const char std_format_str[] = "PC:%04X NEXT:%04X A:%02X X:%02X Y:%02X SP:%02X NV-BDIZC:%d%d%d%d%d%d%d%d opcode:%02X (%s %s) cycles:%d\n";
    const char json_format_str[] = "{ \"PC\": \"0x%04X\", \"NEXT\": \"0x%04X\", \"A\": \"0x%02X\", \"X\": \"0x%02X\", \"Y\": \"0x%02X\", \"SP\": \"0x%02X\", \"NV-BDIZC\": \"0b%d%d%d%d%d%d%d%d\", \"opcode\": \"0x%02X\", \"mnemonic\": \"%s\", \"addressing_mode\": \"%s\", \"cycles\":\"%d\" }\n";

    printf(json_flag?json_format_str:std_format_str,
           cpu_s->ir_addr,
           cpu_s->reg.program_counter,
           cpu_s->reg.accumulator,
           cpu_s->reg.x,
           cpu_s->reg.y,
           cpu_s->reg.stack_pointer,
           cpu_s->reg.processor_status.negative,
           cpu_s->reg.processor_status.overflow,
           1,
           cpu_s->reg.processor_status.break_flg,
           cpu_s->reg.processor_status.decimal_mode,
           cpu_s->reg.processor_status.irq_disable,
           cpu_s->reg.processor_status.zero,
           cpu_s->reg.processor_status.carry,
           cpu_s->ir_opcode,
           z6502_get_instruction_mnemonic(cpu_s),
           z6502_get_addressing_mode_str(cpu_s),
           cpu_s->ir_cycles
           );
}