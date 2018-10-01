#include "i386.h"

i386::i386(const int t_line_num, std::string t_line_text, Type t, std::string t_opcode, std::string o1, std::string o2)
    : ASMLine{t, t_opcode}
    , line_num{t_line_num}
    , line_text{std::move(t_line_text)}
    , opcode{parse_opcode(t, t_opcode)}
    , operand1{parse_operand(o1)}
    , operand2{parse_operand(o2)}
{
}

auto i386::parse_opcode(Type t, const std::string &o) -> i386::OpCode
{
    switch (t)
    {
        case Type::Label:
            return OpCode::unknown;
        case Type::Directive:
            return OpCode::unknown;
        case Type::Instruction:
        {
            if (o == "movzwl")
                return OpCode::movzwl;
            if (o == "movzbl")
                return OpCode::movzbl;
            if (o == "shrb")
                return OpCode::shrb;
            if (o == "shrl")
                return OpCode::shrl;
            if (o == "xorl")
                return OpCode::xorl;
            if (o == "andl")
                return OpCode::andl;
            if (o == "ret")
                return OpCode::ret;
            if (o == "movb")
                return OpCode::movb;
            if (o == "cmpb")
                return OpCode::cmpb;
            if (o == "movl")
                return OpCode::movl;
            if (o == "jmp")
                return OpCode::jmp;
            if (o == "testb")
                return OpCode::testb;
            if (o == "incl")
                return OpCode::incl;
            if (o == "sarl")
                return OpCode::sarl;
            if (o == "decl")
                return OpCode::decl;
            if (o == "jne")
                return OpCode::jne;
            if (o == "je")
                return OpCode::je;
            if (o == "js")
                return OpCode::js;
            if (o == "subl")
                return OpCode::subl;
            if (o == "subb")
                return OpCode::subb;
            if (o == "addl")
                return OpCode::addl;
            if (o == "addb")
                return OpCode::addb;
            if (o == "sall")
                return OpCode::sall;
            if (o == "orl")
                return OpCode::orl;
            if (o == "andb")
                return OpCode::andb;
            if (o == "orb")
                return OpCode::orb;
            if (o == "decb")
                return OpCode::decb;
            if (o == "incb")
                return OpCode::incb;
            if (o == "rep")
                return OpCode::rep;
            if (o == "notb")
                return OpCode::notb;
            if (o == "negb")
                return OpCode::negb;
            if (o == "sbbb")
                return OpCode::sbbb;
            if (o == "pushl")
                return OpCode::pushl;
            if (o == "retl")
                return OpCode::retl;
            if (o == "calll")
                return OpCode::calll;

            return OpCode::unknown;
        }
        default:
            break;
    }

    throw std::runtime_error("Unknown opcode: " + o);
}

auto i386::parse_operand(std::string o) -> Operand
{
    if (o.empty())
        return {};

    if (o[0] != '%')
        return Operand(Operand::Type::literal, std::move(o));

    if (o == "%al")
    {
        return Operand(Operand::Type::reg, 0x00);
    }
    else if (o == "%ah")
    {
        return Operand(Operand::Type::reg, 0x01);
    }
    else if (o == "%bl")
    {
        return Operand(Operand::Type::reg, 0x02);
    }
    else if (o == "%bh")
    {
        return Operand(Operand::Type::reg, 0x03);
    }
    else if (o == "%cl")
    {
        return Operand(Operand::Type::reg, 0x04);
    }
    else if (o == "%ch")
    {
        return Operand(Operand::Type::reg, 0x05);
    }
    else if (o == "%dl")
    {
        return Operand(Operand::Type::reg, 0x06);
    }
    else if (o == "%dh")
    {
        return Operand(Operand::Type::reg, 0x07);
    }
    else if (o == "%sil")
    {
        return Operand(Operand::Type::reg, 0x08);
    }
    else if (o == "%dil")
    {
        return Operand(Operand::Type::reg, 0x0A);
    }
    else if (o == "%ax" || o == "%eax")
    {
        return Operand(Operand::Type::reg, 0x10);
    }
    else if (o == "%bx" || o == "%ebx")
    {
        return Operand(Operand::Type::reg, 0x11);
    }
    else if (o == "%cx" || o == "%ecx")
    {
        return Operand(Operand::Type::reg, 0x12);
    }
    else if (o == "%dx" || o == "%edx")
    {
        return Operand(Operand::Type::reg, 0x13);
    }
    else if (o == "%si" || o == "%esi")
    {
        return Operand(Operand::Type::reg, 0x14);
    }
    else if (o == "%di" || o == "%edi")
    {
        return Operand(Operand::Type::reg, 0x15);
    }

    throw std::runtime_error("Unknown register operand: '" + o + "'");
}
