#include <mos6502/mos6502_target.h>
#include "opcodes.h"
#include <ca/instruction_operand.h>

void mos6502_target::translate_andb(const ca::instruction_operand &o1, const ca::instruction_operand &o2)
{
    if (o1.is_literal() && o2.is_register())
    {
        const auto reg = get_register(o2.reg());
        emit(mos6502_opcode::lda, reg);
        emit(mos6502_opcode::AND, ca::instruction_operand(ca::operand_type::literal, fixup_8bit_literal(o1.value())));
        emit(mos6502_opcode::sta, reg);
    }
    else if (o1.is_literal() && o2.is_literal())
    {
        const auto reg = get_register(o2.reg());
        emit(mos6502_opcode::lda, o2);
        emit(mos6502_opcode::AND, ca::instruction_operand(ca::operand_type::literal, fixup_8bit_literal(o1.value())));
        emit(mos6502_opcode::sta, o2);
    }
    else
    {
        throw std::runtime_error("Cannot translate andb instruction");
    }
}

void mos6502_target::translate_andl(const ca::instruction_operand &o1, const ca::instruction_operand &o2)
{
    throw std::runtime_error("Cannot translate andl instruction");
}

void mos6502_target::translate_notb(const ca::instruction_operand &o1, const ca::instruction_operand &o2)
{
    if (o1.is_register())
    {
        // exclusive or against 0xff to perform a logical not
        emit(mos6502_opcode::lda, get_register(o1.reg()));
        emit(mos6502_opcode::eor, ca::instruction_operand(ca::operand_type::literal, "#$ff"));
        emit(mos6502_opcode::sta, get_register(o1.reg()));
    }
    else
    {
        throw std::runtime_error("Cannot translate notb instruction");
    }
}

void mos6502_target::translate_orb(const ca::instruction_operand &o1, const ca::instruction_operand &o2)
{
    if (o1.is_literal() && o2.is_literal())
    {
        emit(mos6502_opcode::lda, ca::instruction_operand(ca::operand_type::literal, fixup_8bit_literal(o1.value())));
        emit(mos6502_opcode::ORA, o2);
        emit(mos6502_opcode::sta, o2);
    }
    else if (o1.is_register() && o2.is_register())
    {
        emit(mos6502_opcode::lda, get_register(o1.reg()));
        emit(mos6502_opcode::ORA, get_register(o2.reg()));
        emit(mos6502_opcode::sta, get_register(o2.reg()));
    }
    else if (o1.is_literal() && o2.is_register())
    {
        emit(mos6502_opcode::lda, ca::instruction_operand(ca::operand_type::literal, fixup_8bit_literal(o1.value())));
        emit(mos6502_opcode::ORA, get_register(o2.reg()));
        emit(mos6502_opcode::sta, get_register(o2.reg()));
    }
    else
    {
        throw std::runtime_error("Cannot translate orb instruction");
    }
}

void mos6502_target::translate_orl(const ca::instruction_operand &o1, const ca::instruction_operand &o2)
{
    throw std::runtime_error("Cannot translate orl instruction");
}

void mos6502_target::translate_shrb(const ca::instruction_operand &o1, const ca::instruction_operand &o2)
{
    if (o1.is_register() || o2.is_register())
    {
        const auto do_shift = [this](const auto reg) { emit(mos6502_opcode::lsr, get_register(reg)); };

        if (o1.is_literal())
        {
            const auto count = parse_literal(o1.value());
            for (int i = 0; i < count; ++i)
            {
                do_shift(o2.reg());
            }
        }
        else
        {
            do_shift(o1.reg());
        }
    }
    else
    {
        throw std::runtime_error("Cannot translate shrb instruction");
    }
}

void mos6502_target::translate_shrl(const ca::instruction_operand &o1, const ca::instruction_operand &o2)
{
    if (o1.is_register() || o2.is_register())
    {
        const auto do_shift = [this](const auto reg) {
            emit(mos6502_opcode::lsr, get_register(reg, 1));
            emit(mos6502_opcode::ror, get_register(reg));
        };

        if (o1.is_literal())
        {
            const auto count = parse_literal(o1.value());
            for (int i = 0; i < count; ++i)
            {
                do_shift(o2.reg());
            }
        }
        else
        {
            do_shift(o1.reg());
        }
    }
    else
    {
        throw std::runtime_error("Cannot translate shrl instruction");
    }
}

void mos6502_target::translate_xorl(const ca::instruction_operand &o1, const ca::instruction_operand &o2)
{
    if (o1.is_register() && o2.is_register() && o1.reg() == o2.reg())
    {
        emit(mos6502_opcode::lda, ca::instruction_operand(ca::operand_type::literal, "#$00"));
        emit(mos6502_opcode::sta, get_register(o2.reg()));
        emit(mos6502_opcode::sta, get_register(o2.reg(), 1));
    }
    else
    {
        throw std::runtime_error("Cannot translate xorl instruction");
    }
}
