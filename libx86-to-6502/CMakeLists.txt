set(LIB_X86_TO_6502_SOURCES
    src/i386.cpp
    src/mos6502.cpp
    include/x86-to-6502/i386.h
    include/x86-to-6502/asm_line.h
    include/x86-to-6502/mos6502.h
    include/x86-to-6502/instruction_operand.h
)

# Group headers and source files together in a single folder in IDE's like Visual Studio
source_group(libx86-to-6502 FILES ${LIB_X86_TO_6502_SOURCES})

add_library(libx86-to-6502 STATIC
    ${LIB_X86_TO_6502_SOURCES}
)

target_include_directories(libx86-to-6502
    PRIVATE src
    PUBLIC include
)

add_subdirectory(tests)
