cmake_minimum_required(VERSION 3.10)
project(z6502_emulator LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)


# Enable CTest
#enable_testing()

# Find the Python interpreter installed on the system
#find_package(Python3 REQUIRED COMPONENTS Interpreter)

# Register the Python test script
#add_test(
#    NAME run_instruction_test
#    COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/test/test.sh" $<TARGET_FILE:z6502_emulator> "${CMAKE_CURRENT_SOURCE_DIR}/test/test_engine.py"
#)


include_directories(${PROJECT_SOURCE_DIR}/include)

add_subdirectory(src)
add_subdirectory(lib)

