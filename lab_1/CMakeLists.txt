cmake_minimum_required(VERSION 3.10)
project(lab_1 C)

# Set C standard to C99
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# List all source files
set(SOURCES
        main.c
        long_division.c
        long_numbers.c
)

# List all header files
set(HEADERS
        constants.h
        long_numbers.h
        long_division.h
)

# Create executable
add_executable(lab_1 ${SOURCES} ${HEADERS})

# Set output directory for executable
set_target_properties(lab_1 PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Enable more warnings for better code quality
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(lab_1 PRIVATE -Wall -Wextra -Wpedantic -O0 -Wfloat-equal -Wfloat-conversion -Wvla)
endif()