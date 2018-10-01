include(Unittests)

add_unit_test_suite(
    NO_GTEST_MAIN
    TARGET test_libx86-to-6502
    SOURCES
        main.cpp
        test_parse_x86_asm_line.cpp
    LIBRARIES libx86-to-6502
)
