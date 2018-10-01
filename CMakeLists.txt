cmake_minimum_required(VERSION 3.10)

set (CMAKE_CXX_STANDARD 17)

project(x86-to-6502)

if(CMAKE_COMPILER_IS_GNUCC)
    add_definitions(-Wall -Wextra -Wconversion -Wshadow -Wnon-virtual-dtor -Wold-style-cast -Wcast-align -Wcast-qual -Wunused -Woverloaded-virtual -pedantic)
endif()

add_subdirectory(libx86-to-6502)
add_subdirectory(x86-to-6502)
