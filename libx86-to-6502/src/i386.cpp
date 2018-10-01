#include <x86-to-6502/i386.h>

namespace internal
{

static auto parse_opcode(asm_line::line_type t, const std::string &o) -> i386_opcode
{
    switch (t)
    {
        case asm_line::line_type::Label:
            return i386_opcode::unknown;
        case asm_line::line_type::Directive:
            return i386_opcode::unknown;
        case asm_line::line_type::Instruction:
        {
            if (o == "movzwl")
                return i386_opcode::movzwl;
            if (o == "movzbl")
                return i386_opcode::movzbl;
            if (o == "shrb")
                return i386_opcode::shrb;
            if (o == "shrl")
                return i386_opcode::shrl;
            if (o == "xorl")
                return i386_opcode::xorl;
            if (o == "andl")
                return i386_opcode::andl;
            if (o == "ret")
                return i386_opcode::ret;
            if (o == "movb")
                return i386_opcode::movb;
            if (o == "cmpb")
                return i386_opcode::cmpb;
            if (o == "movl")
                return i386_opcode::movl;
            if (o == "jmp")
                return i386_opcode::jmp;
            if (o == "testb")
                return i386_opcode::testb;
            if (o == "incl")
                return i386_opcode::incl;
            if (o == "sarl")
                return i386_opcode::sarl;
            if (o == "decl")
                return i386_opcode::decl;
            if (o == "jne")
                return i386_opcode::jne;
            if (o == "je")
                return i386_opcode::je;
            if (o == "js")
                return i386_opcode::js;
            if (o == "subl")
                return i386_opcode::subl;
            if (o == "subb")
                return i386_opcode::subb;
            if (o == "addl")
                return i386_opcode::addl;
            if (o == "addb")
                return i386_opcode::addb;
            if (o == "sall")
                return i386_opcode::sall;
            if (o == "orl")
                return i386_opcode::orl;
            if (o == "andb")
                return i386_opcode::andb;
            if (o == "orb")
                return i386_opcode::orb;
            if (o == "decb")
                return i386_opcode::decb;
            if (o == "incb")
                return i386_opcode::incb;
            if (o == "rep")
                return i386_opcode::rep;
            if (o == "notb")
                return i386_opcode::notb;
            if (o == "negb")
                return i386_opcode::negb;
            if (o == "sbbb")
                return i386_opcode::sbbb;
            if (o == "pushl")
                return i386_opcode::pushl;
            if (o == "retl")
                return i386_opcode::retl;
            if (o == "calll")
                return i386_opcode::calll;

            return i386_opcode::unknown;
        }
        default:
            break;
    }

    throw std::runtime_error("Unknown opcode: " + o);
}

static auto parse_operand(std::string o) -> instruction_operand
{
    if (o.empty())
        return {};

    if (o[0] != '%')
        return instruction_operand(operand_type::literal, std::move(o));

    if (o == "%al")
    {
        return instruction_operand(operand_type::reg, 0x00);
    }
    else if (o == "%ah")
    {
        return instruction_operand(operand_type::reg, 0x01);
    }
    else if (o == "%bl")
    {
        return instruction_operand(operand_type::reg, 0x02);
    }
    else if (o == "%bh")
    {
        return instruction_operand(operand_type::reg, 0x03);
    }
    else if (o == "%cl")
    {
        return instruction_operand(operand_type::reg, 0x04);
    }
    else if (o == "%ch")
    {
        return instruction_operand(operand_type::reg, 0x05);
    }
    else if (o == "%dl")
    {
        return instruction_operand(operand_type::reg, 0x06);
    }
    else if (o == "%dh")
    {
        return instruction_operand(operand_type::reg, 0x07);
    }
    else if (o == "%sil")
    {
        return instruction_operand(operand_type::reg, 0x08);
    }
    else if (o == "%dil")
    {
        return instruction_operand(operand_type::reg, 0x0A);
    }
    else if (o == "%ax" || o == "%eax")
    {
        return instruction_operand(operand_type::reg, 0x10);
    }
    else if (o == "%bx" || o == "%ebx")
    {
        return instruction_operand(operand_type::reg, 0x11);
    }
    else if (o == "%cx" || o == "%ecx")
    {
        return instruction_operand(operand_type::reg, 0x12);
    }
    else if (o == "%dx" || o == "%edx")
    {
        return instruction_operand(operand_type::reg, 0x13);
    }
    else if (o == "%si" || o == "%esi")
    {
        return instruction_operand(operand_type::reg, 0x14);
    }
    else if (o == "%di" || o == "%edi")
    {
        return instruction_operand(operand_type::reg, 0x15);
    }
    else if (o == "%sp" || o == "%esp")
    {
        return instruction_operand(operand_type::reg, 0x16);
    }

    throw std::runtime_error("Unknown register operand: '" + o + "'");
}

} // namespace internal

i386::i386(const int line_number)
    : asm_line{}
    , line_number_{line_number}
    , line_text_{}
    , opcode_{}
    , operand1_{}
    , operand2_{}
{
}

i386::i386(const int line_number, std::string line_text)
    : asm_line{std::move(line_text)}
    , line_number_{line_number}
    , line_text_{}
    , opcode_{}
    , operand1_{}
    , operand2_{}
{
}

i386::i386(const int line_number, std::string line_text, line_type type, std::string opcode, std::string operand1,
           std::string operand2)
    : asm_line{type, opcode}
    , line_number_{line_number}
    , line_text_{std::move(line_text)}
    , opcode_{internal::parse_opcode(type, opcode)}
    , operand1_{internal::parse_operand(operand1)}
    , operand2_{internal::parse_operand(operand2)}
{
}
