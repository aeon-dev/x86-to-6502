cmake_minimum_required(VERSION 3.10)

set (CMAKE_CXX_STANDARD 17)

project(x86-to-6502)

if(CMAKE_COMPILER_IS_GNUCC)
    add_definitions(-Wall -Wextra -Wconversion -Wshadow -Wnon-virtual-dtor -Wold-style-cast -Wcast-align -Wcast-qual -Wunused -Woverloaded-virtual -pedantic)
endif()

add_executable(x86-to-6502
    src/asm_line.h
    src/i386.cpp
    src/i386.h
    src/main.cpp
    src/mos6502.cpp
    src/mos6502.h
    src/operand.h
)

target_include_directories(x86-to-6502
    PRIVATE src
)
