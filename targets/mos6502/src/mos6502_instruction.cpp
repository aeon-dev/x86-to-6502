#include <mos6502/mos6502_instruction.h>
#include "opcodes.h"
#include <algorithm>
#include <cassert>
#include <stdexcept>

namespace internal
{

static auto is_branch(const mos6502_opcode o) -> bool
{
    switch (o)
    {
        case mos6502_opcode::beq:
        case mos6502_opcode::bne:
        case mos6502_opcode::bmi:
            return true;
        case mos6502_opcode::lda:
        case mos6502_opcode::ldy:
        case mos6502_opcode::tay:
        case mos6502_opcode::tya:
        case mos6502_opcode::cpy:
        case mos6502_opcode::eor:
        case mos6502_opcode::sta:
        case mos6502_opcode::sty:
        case mos6502_opcode::pha:
        case mos6502_opcode::pla:
        case mos6502_opcode::php:
        case mos6502_opcode::plp:
        case mos6502_opcode::lsr:
        case mos6502_opcode::ror:
        case mos6502_opcode::AND:
        case mos6502_opcode::inc:
        case mos6502_opcode::dec:
        case mos6502_opcode::ORA:
        case mos6502_opcode::cmp:
        case mos6502_opcode::jmp:
        case mos6502_opcode::adc:
        case mos6502_opcode::sbc:
        case mos6502_opcode::rts:
        case mos6502_opcode::rti:
        case mos6502_opcode::clc:
        case mos6502_opcode::sec:
        case mos6502_opcode::bit:
        case mos6502_opcode::jsr:
        case mos6502_opcode::unknown:
            return false;
    }

    assert(false && "Missing opcode in mos6502::get_is_branch");
    return false;
}

static auto is_comparison(const mos6502_opcode o) -> bool
{
    switch (o)
    {
        case mos6502_opcode::cmp:
        case mos6502_opcode::cpy:
        case mos6502_opcode::bit:
            return true;
        case mos6502_opcode::lda:
        case mos6502_opcode::ldy:
        case mos6502_opcode::tay:
        case mos6502_opcode::tya:
        case mos6502_opcode::eor:
        case mos6502_opcode::sta:
        case mos6502_opcode::sty:
        case mos6502_opcode::pha:
        case mos6502_opcode::pla:
        case mos6502_opcode::php:
        case mos6502_opcode::plp:
        case mos6502_opcode::lsr:
        case mos6502_opcode::ror:
        case mos6502_opcode::AND:
        case mos6502_opcode::inc:
        case mos6502_opcode::dec:
        case mos6502_opcode::ORA:
        case mos6502_opcode::jmp:
        case mos6502_opcode::bne:
        case mos6502_opcode::bmi:
        case mos6502_opcode::beq:
        case mos6502_opcode::adc:
        case mos6502_opcode::sbc:
        case mos6502_opcode::rts:
        case mos6502_opcode::rti:
        case mos6502_opcode::clc:
        case mos6502_opcode::sec:
        case mos6502_opcode::jsr:
        case mos6502_opcode::unknown:
            return false;
    }

    assert(false && "Missing opcode in mos6502::get_is_comparison");
    return false;
}

} // namespace internal

mos6502::mos6502(const mos6502_opcode o)
    : asm_line{line_type::instruction, to_string(o)}
    , opcode_{o}
    , is_branch_{internal::is_branch(o)}
    , is_comparison_{internal::is_comparison(o)}
{
}

mos6502::mos6502(const line_type t, std::string s)
    : asm_line{t, std::move(s)}
    , opcode_{mos6502_opcode::unknown}
{
}

mos6502::mos6502(const mos6502_opcode o, ca::instruction_operand t_o)
    : asm_line{line_type::instruction, to_string(o)}
    , opcode_{o}
    , operand_{std::move(t_o)}
    , is_branch_{internal::is_branch(o)}
    , is_comparison_{internal::is_comparison(o)}
{
}

auto mos6502::to_string(const mos6502_opcode o) -> std::string
{
    switch (o)
    {
        case mos6502_opcode::lda:
            return "lda";
        case mos6502_opcode::ldy:
            return "ldy";
        case mos6502_opcode::tay:
            return "tay";
        case mos6502_opcode::tya:
            return "tya";
        case mos6502_opcode::cpy:
            return "cpy";
        case mos6502_opcode::eor:
            return "eor";
        case mos6502_opcode::sta:
            return "sta";
        case mos6502_opcode::sty:
            return "sty";
        case mos6502_opcode::pha:
            return "pha";
        case mos6502_opcode::pla:
            return "pla";
        case mos6502_opcode::php:
            return "php";
        case mos6502_opcode::plp:
            return "plp";
        case mos6502_opcode::lsr:
            return "lsr";
        case mos6502_opcode::ror:
            return "ror";
        case mos6502_opcode::AND:
            return "and";
        case mos6502_opcode::inc:
            return "inc";
        case mos6502_opcode::dec:
            return "dec";
        case mos6502_opcode::ORA:
            return "ora";
        case mos6502_opcode::cmp:
            return "cmp";
        case mos6502_opcode::bne:
            return "bne";
        case mos6502_opcode::bmi:
            return "bmi";
        case mos6502_opcode::beq:
            return "beq";
        case mos6502_opcode::jmp:
            return "jmp";
        case mos6502_opcode::adc:
            return "adc";
        case mos6502_opcode::sbc:
            return "sbc";
        case mos6502_opcode::rts:
            return "rts";
        case mos6502_opcode::rti:
            return "rti";
        case mos6502_opcode::clc:
            return "clc";
        case mos6502_opcode::sec:
            return "sec";
        case mos6502_opcode::bit:
            return "bit";
        case mos6502_opcode::jsr:
            return "jsr";
        case mos6502_opcode::unknown:
            return "";
    }

    assert(false && "Missing opcode in mos6502::to_string");
    return "";
}

auto mos6502::to_string() const -> std::string
{
    switch (type())
    {
        case asm_line::line_type::comment:
        case asm_line::line_type::label:
            return text(); // + ':';
        case asm_line::line_type::directive:
        case asm_line::line_type::instruction:
        {
            const std::string line = "    " + text() + ' ' + operand_.value();
            return line + std::string(static_cast<size_t>(std::max(15 - static_cast<int>(line.size()), 1)), ' ') +
                   "; " + comment_;
        }
        case asm_line::line_type::missing_opcode:
        {
            return ";   " + text() + " !!!MISSING!!! !!!MISSING!!!";
        }
        case line_type::empty:
        default:
            break;
    }

    throw std::runtime_error("Unable to render: " + text());
}
