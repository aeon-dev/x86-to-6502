#include "mos6502.h"
#include <algorithm>

mos6502::mos6502(const OpCode o)
    : ASMLine(Type::Instruction, to_string(o))
    , opcode(o)
    , is_branch(get_is_branch(o))
    , is_comparison(get_is_comparison(o))
{
}

mos6502::mos6502(const Type t, std::string s)
    : ASMLine(t, std::move(s))
{
}

mos6502::mos6502(const OpCode o, Operand t_o)
    : ASMLine(Type::Instruction, to_string(o))
    , opcode(o)
    , op(std::move(t_o))
    , is_branch(get_is_branch(o))
    , is_comparison(get_is_comparison(o))
{
}

auto mos6502::get_is_branch(const OpCode o) -> bool
{
    switch (o)
    {
        case OpCode::beq:
        case OpCode::bne:
        case OpCode::bmi:
            return true;
        case OpCode::lda:
        case OpCode::ldy:
        case OpCode::tay:
        case OpCode::tya:
        case OpCode::cpy:
        case OpCode::eor:
        case OpCode::sta:
        case OpCode::sty:
        case OpCode::pha:
        case OpCode::pla:
        case OpCode::php:
        case OpCode::plp:
        case OpCode::lsr:
        case OpCode::ror:
        case OpCode::AND:
        case OpCode::inc:
        case OpCode::dec:
        case OpCode::ORA:
        case OpCode::cmp:
        case OpCode::jmp:
        case OpCode::adc:
        case OpCode::sbc:
        case OpCode::rts:
        case OpCode::clc:
        case OpCode::sec:
        case OpCode::bit:
        case OpCode::jsr:
        case OpCode::unknown:
            break;
    }

    return false;
}

auto mos6502::get_is_comparison(const OpCode o) -> bool
{
    switch (o)
    {
        case OpCode::cmp:
        case OpCode::cpy:
        case OpCode::bit:
            return true;
        case OpCode::lda:
        case OpCode::ldy:
        case OpCode::tay:
        case OpCode::tya:
        case OpCode::eor:
        case OpCode::sta:
        case OpCode::sty:
        case OpCode::pha:
        case OpCode::pla:
        case OpCode::php:
        case OpCode::plp:
        case OpCode::lsr:
        case OpCode::ror:
        case OpCode::AND:
        case OpCode::inc:
        case OpCode::dec:
        case OpCode::ORA:
        case OpCode::jmp:
        case OpCode::bne:
        case OpCode::bmi:
        case OpCode::beq:
        case OpCode::adc:
        case OpCode::sbc:
        case OpCode::rts:
        case OpCode::clc:
        case OpCode::sec:
        case OpCode::unknown:
        default:
            break;
    }

    return false;
}

auto mos6502::to_string(const OpCode o) -> std::string
{
    switch (o)
    {
        case OpCode::lda:
            return "lda";
        case OpCode::ldy:
            return "ldy";
        case OpCode::tay:
            return "tay";
        case OpCode::tya:
            return "tya";
        case OpCode::cpy:
            return "cpy";
        case OpCode::eor:
            return "eor";
        case OpCode::sta:
            return "sta";
        case OpCode::sty:
            return "sty";
        case OpCode::pha:
            return "pha";
        case OpCode::pla:
            return "pla";
        case OpCode::php:
            return "php";
        case OpCode::plp:
            return "plp";
        case OpCode::lsr:
            return "lsr";
        case OpCode::ror:
            return "ror";
        case OpCode::AND:
            return "and";
        case OpCode::inc:
            return "inc";
        case OpCode::dec:
            return "dec";
        case OpCode::ORA:
            return "ora";
        case OpCode::cmp:
            return "cmp";
        case OpCode::bne:
            return "bne";
        case OpCode::bmi:
            return "bmi";
        case OpCode::beq:
            return "beq";
        case OpCode::jmp:
            return "jmp";
        case OpCode::adc:
            return "adc";
        case OpCode::sbc:
            return "sbc";
        case OpCode::rts:
            return "rts";
        case OpCode::clc:
            return "clc";
        case OpCode::sec:
            return "sec";
        case OpCode::bit:
            return "bit";
        case OpCode::jsr:
            return "jsr";
        case OpCode::unknown:
            return "";
    }

    return "";
}

auto mos6502::to_string() const -> std::string
{
    switch (type)
    {
        case ASMLine::Type::Label:
            return text; // + ':';
        case ASMLine::Type::Directive:
        case ASMLine::Type::Instruction:
        {
            const std::string line = "    " + text + ' ' + op.value;
            return line + std::string(static_cast<size_t>(std::max(15 - static_cast<int>(line.size()), 1)), ' ') +
                   "; " + comment;
        }
        case ASMLine::Type::MissingOpcode:
        {
            return ";   " + text + " !!!MISSING!!! !!!MISSING!!!";
        }
    }

    throw std::runtime_error("Unable to render: " + text);
}
