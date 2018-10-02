add_executable(x86-to-6502
    src/main.cpp
)

target_link_libraries(x86-to-6502 libca target_mos6502)

target_include_directories(x86-to-6502
    PRIVATE src
)

set_target_properties(
    x86-to-6502 PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
