set(LIB_TARGET_MOS6502_SOURCES
    include/mos6502/mos6502_target.h
    src/bitwise_instructions.cpp
    src/branch_instructions.cpp
    src/math_instructions.cpp
    src/misc_instructions.cpp
    src/move_instructions.cpp
    src/mos6502_target.cpp
    src/mos6502_instruction.cpp
    include/mos6502/mos6502_instruction.h
    src/opcodes.h
)

source_group(mos6502 FILES ${LIB_TARGET_MOS6502_SOURCES})

add_library(target_mos6502 STATIC
    ${LIB_TARGET_MOS6502_SOURCES}
)

target_link_libraries(target_mos6502
    libca
)

target_include_directories(target_mos6502
    PRIVATE src
    PUBLIC include
)
