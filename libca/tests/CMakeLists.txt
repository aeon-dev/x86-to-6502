include(Unittests)

add_unit_test_suite(
    NO_GTEST_MAIN
    TARGET test_libca
    SOURCES
        main.cpp
        test_parse_x86_asm_line.cpp
    LIBRARIES libca
    FOLDER libraries/tests
)
