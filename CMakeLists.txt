cmake_minimum_required(VERSION 3.10)

project(x86-to-6502)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}")
set(CMAKE_CXX_STANDARD 17)

# Create compile_commands.json when possible, which can be used for
# analysis tools.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(CTest)
enable_testing()

if (MSVC)
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 19.10)
        message(FATAL_ERROR "Requires Visual Studio 2017 or higher!")
    endif ()

    add_definitions(/WX /std:c++latest /permissive-)
endif ()

if(CMAKE_COMPILER_IS_GNUCC)
    add_definitions(-Wall -Wextra -Wconversion -Wshadow -Wnon-virtual-dtor -Wold-style-cast -Wcast-align -Wcast-qual -Wunused -Woverloaded-virtual -pedantic)
endif()

add_subdirectory(dep/googletest)
add_subdirectory(libx86-to-6502)
add_subdirectory(x86-to-6502)
