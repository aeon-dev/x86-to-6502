add_library(x86-to-6502 STATIC
    src/main.cpp
)

target_link_libraries(x86-to-6502 libx86-to-6502)

target_include_directories(x86-to-6502
    PRIVATE src
)
