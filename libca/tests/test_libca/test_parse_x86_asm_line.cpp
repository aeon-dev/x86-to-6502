#include <ca/i386.h>
#include <gtest/gtest.h>

static void test_line(const std::string &str, const ca::asm_line::line_type expected)
{
    ASSERT_NO_THROW(const auto line = ca::i386::parse(str, 0); EXPECT_EQ(expected, line.type()););
}

TEST(test_parse_x86_asm_line, test_empty_line)
{
    ca::i386 line = ca::i386::parse("", 0);
    EXPECT_TRUE(line.is_empty());
    EXPECT_FALSE(line.is_comment());
    EXPECT_FALSE(line.is_label());
    EXPECT_FALSE(line.is_instruction());
    EXPECT_FALSE(line.is_directive());
    EXPECT_FALSE(line.is_missing_opcode());
}

TEST(test_parse_x86_asm_line, test_comment_line)
{
    test_line("#", ca::asm_line::line_type::Comment);
    test_line("####", ca::asm_line::line_type::Comment);
    test_line("# %bb.0:", ca::asm_line::line_type::Comment);
}

TEST(test_parse_x86_asm_line, test_directive_line)
{
    test_line("\t.text", ca::asm_line::line_type::Directive);
    test_line(".set @feat.00, 1", ca::asm_line::line_type::Directive); // Clang on Windows adds this line
    test_line("\t.def\t_main;", ca::asm_line::line_type::Directive);
    test_line("\t.section\t.debug_line,\"dr\"", ca::asm_line::line_type::Directive);
}

TEST(test_parse_x86_asm_line, test_basic_instruction_line)
{
    test_line("\tretl", ca::asm_line::line_type::Instruction);
}

TEST(test_parse_x86_asm_line, test_basic_unairy_instruction_line)
{
    test_line("\tpushl\t$86", ca::asm_line::line_type::Instruction);
    test_line("\tcalll\t_test", ca::asm_line::line_type::Instruction);
    test_line("\tdecb\t%al", ca::asm_line::line_type::Instruction);

    // Emitted by clang when calling functions with a 16-bit value.
    test_line("\tpushl\t$4660                   # imm = 0x1234", ca::asm_line::line_type::Instruction);
    test_line("\tpushl\t$4660\t# imm = 0x1234", ca::asm_line::line_type::Instruction);
}

TEST(test_parse_x86_asm_line, test_basic_binary_instruction_line)
{
    test_line("\txorl\t%eax, %eax", ca::asm_line::line_type::Instruction);
    test_line("\taddl\t$8, %esp", ca::asm_line::line_type::Instruction);
    test_line("\tmovb\t$-1, 24577", ca::asm_line::line_type::Instruction);
}
