# Z6502 CPU CORE

A 6502 CPU instruction set simulator, written in C

This is a library / submodule, designed to be compiled with your project using CMake.

To see it in action, take a look at my [6502 emulator](https://github.com/Arnaud-Le-Cossec/z6502-emulator)

## CMAKE integration

Here is your typical CMAKE project structure 

```
myProject
├── CMakeLists.txt (3)
├── README.md
├── src/
│   ├── CMakeLists.txt (4)
│   └── main.c
├── include/
│   └── main.h
├── lib/
│   ├── CMakeLists.txt (2)
│   └── z6502-cpu-core (1)
│       ├── src/
│       ├── include/
│       ├── CMakeLists.txt
│       └── README.md
└── test/
    └── ...
```

### Steps:

- **Copy** this repository in your project (**1**) (for example in a `lib` folder). ***Using a submodule is highly recommended***
- **Include** the folder all parent `CMakeLists.txt`. For example: 
    - In file (**2**), add this line:
        ```cmake
        add_subdirectory(z6502-cpu-core)
        ```
    - In file (**3**), add this line:
        ```cmake
        add_subdirectory(lib)
        ```
- **Link** the library to your project
    - In file (**4**), add this line:
        ```cmake
        target_link_libraries(myProject PRIVATE z6502_core)
        ```

### Result:

Your root `CMakeLists.txt` (**3**) should look like this :

```cmake
cmake_minimum_required(VERSION 3.10)
project(myProject LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

include_directories(${PROJECT_SOURCE_DIR}/include)

add_subdirectory(src)
add_subdirectory(libs) # <- We added this line
```

And the source `CMakeLists.txt` (**4**) should look like this :

```cmake
add_executable(myProject
    main.c
)

target_include_directories(myProject PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(myProject PRIVATE z6502_core) # <- We added this line
```

From now on you should be able to compile your project with the library. `z6502.h` should be visible from anywhere in the project.

## Usage example:

Here is a quick usage example in C

```c
#include "z6502.h"

int main(int argc,char ** argv) {
    /*Allocate memory and io spaces*/
    uint8_t* memory_space = (uint8_t*)malloc(Z6502_MAX_MEMORY_SIZE_BYTES*sizeof(uint8_t));

    if(memory_space == NULL){
        fprintf(stderr, "[CRITICAL] Memory allocation error\n");
        exit(EXIT_FAILURE);
    }

    /*Load memory*/
    // Implement your own logic here

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

        /*Exit condition*/
        // Implement your own logic here

    }

    free(memory_space);
    return 0;
}
```

## API

### Structure

This library relies on the `cpu_t` structure found in `z6502.h`. Here is a breakdown of its composition:

- **`reg`** (`z6502_register_set_t`) contains the cpu registers :
    - **`program_counter`** (`uint16_t`)
    - **`stack_pointer`** (`uint16_t`)
    - **`accumulator`** (`uint8_t`)
    - **`x`** (`uint8_t`)
    - **`y`** (`uint8_t`)
    - **`processor_status`** (`z6502_flag_t`)
        - **`carry`** (`uint8_t`)
        - **`zero`** (`uint8_t`)
        - **`irq_disable`** (`uint8_t`)
        - **`decimal_mode`** (`uint8_t`)
        - **`break_flg`** (`uint8_t`)
        - **`overflow`** (`uint8_t`)
        - **`negative`** (`uint8_t`)
- **`memory_ptr`** (`uint8_t*`) is a pointer to the cpu memory space
- **`ir_opcode`** (`uint8_t`) contains the opcode of the current instruction
- **`ir_addr`** (`uint16_t`) contains the address of the current instruction
- **`ir_mode`** (`z6502_addressing_mode_t`) contains the addressing mode of the current instruction. Possible values (enum):
    - **`___`** Undefined
    - **`IMP`** Implied
    - **`ACC`** Accumulator
    - **`IMM`** Immediate
    - **`ZP,`** Zero Page
    - **`ZPX`** Zero Page,X
    - **`ZPY`** Zero Page,Y
    - **`REL`** Relative
    - **`ABS`** Absolute
    - **`ABX`** Absolute,X
    - **`ABY`** Absolute,Y
    - **`IND`** Indirect
    - **`INX`** X-indexed, indirect - aka (Indirect,X)
    - **`INY`** Indirect, Y-indexed	- aka (Indirect),Y
- **`ir_cycles`** (`int`) contains the number of clock cycles consummed by the current instruction

> **Note:** members of the structure starting with `_` are considered private. It is advised to not use them as they can have unexpected behevior and change without notice between versions.

### Public Functions

- **`z6502_init`**

    Initialise the CPU structure. Called once. Should be followed by a `z6502_reset`

    Inputs:
    - `cpu_s` CPU structure pointer 
    - `memory_ptr` memory pointer (Usualy 64535 bytes, even if the emulated hardware has less)

    ```c
    void z6502_init(z6502_cpu_t* cpu_s, uint8_t* memory_ptr);
    ```
- **`z6502_reset`**

    Reset the CPU (equivalent to trigger the RES/pin 40 on a real 6502)

    Inputs:
    - `cpu_s` CPU structure pointer

    ```c
    void z6502_reset(z6502_cpu_t* cpu_s);
    ```

- **`z6502_step`**

    Execute one instruction from memory at program counter

    Inputs:
    - `cpu_s` CPU structure pointer

    Returns:
    - number of clock cycles spent

    ```c
    int z6502_step(z6502_cpu_t* cpu_s);
    ```

- **`z6502_get_instruction_mnemonic`**

    Get the current instruction mnemonic as a string

    Inputs:
    - `cpu_s` CPU structure pointer

    Returns
    - Pointer to mnemonic string

    ```c
    const char* z6502_get_instruction_mnemonic(z6502_cpu_t* cpu_s);
    ```

- **`z6502_get_instruction_mnemonic`**

    Get the current addressing mode as a string

    Inputs:
    - `cpu_s` CPU structure pointer

    Returns
    - Pointer to addressing mode string

    ```c
    const char* z6502_get_addressing_mode_str(z6502_cpu_t* cpu_s);
    ```

## Licence

Copyright © 2026 Arnaud LE COSSEC

MIT licence. See [licence file](./LICENCE.md) for details