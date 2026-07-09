
add_executable(z6502_emulator
    emulator_utility.c
    clock_driver.c
    main.c
    # Add other source files here
)

target_include_directories(z6502_emulator PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(z6502_emulator PRIVATE z6502_core)
