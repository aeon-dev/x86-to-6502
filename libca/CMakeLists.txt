set(LIBCA_SOURCES
    src/cross_assembler.cpp
    include/ca/cross_assembler.h
    src/i386.cpp
    src/logger.h
    src/logger.cpp
    include/ca/i386.h
    include/ca/i386_register.h
    include/ca/asm_line.h
    include/ca/instruction_operand.h
    src/target.cpp
    include/ca/target.h
)

source_group(libca FILES ${LIBCA_SOURCES})

add_library(libca STATIC
    ${LIBCA_SOURCES}
)

set_target_properties(libca PROPERTIES
    FOLDER libraries
)

target_include_directories(libca
    PRIVATE src
    PUBLIC include
)

add_subdirectory(tests)
