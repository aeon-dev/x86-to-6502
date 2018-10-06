#include <mos6502/mos6502_target.h>
#include "opcodes.h"
#include <ca/instruction_operand.h>

void mos6502_target::translate_movb(const ca::instruction_operand &o1, const ca::instruction_operand &o2)
{
    if (o1.is_literal() && o2.is_literal())
    {
        emit(mos6502_opcode::lda, ca::instruction_operand(ca::operand_type::literal, fixup_8bit_literal(o1.value())));
        emit(mos6502_opcode::sta, o2);
    }
    else if (o1.is_literal() && o2.is_register())
    {
        emit(mos6502_opcode::lda, ca::instruction_operand(ca::operand_type::literal, fixup_8bit_literal(o1.value())));
        emit(mos6502_opcode::sta, get_register(o2.reg()));
    }
    else if (o1.is_register() && o2.is_literal())
    {
        emit(mos6502_opcode::lda, get_register(o1.reg()));
        emit(mos6502_opcode::sta, o2);
    }
    else if (o1.is_register() && o2.is_register())
    {
        emit(mos6502_opcode::lda, get_register(o1.reg()));
        emit(mos6502_opcode::sta, get_register(o2.reg()));
    }
    else
    {
        throw std::runtime_error("Cannot translate movb instruction");
    }
}

void mos6502_target::translate_movl(const ca::instruction_operand &o1, const ca::instruction_operand &o2)
{
    if (o1.is_register() && o2.is_register())
    {
        emit(mos6502_opcode::lda, get_register(o1.reg()));
        emit(mos6502_opcode::sta, get_register(o2.reg()));
        emit(mos6502_opcode::lda, get_register(o1.reg(), 1));
        emit(mos6502_opcode::sta, get_register(o2.reg(), 1));
    }
    else if (o1.is_literal() && o2.is_register())
    {
        emit(mos6502_opcode::lda, ca::instruction_operand(ca::operand_type::literal, "#<" + o1.value()));
        emit(mos6502_opcode::sta, get_register(o2.reg()));
        emit(mos6502_opcode::lda, ca::instruction_operand(ca::operand_type::literal, "#>" + o1.value()));
        emit(mos6502_opcode::sta, get_register(o2.reg(), 1));
    }
    else
    {
        throw std::runtime_error("Cannot translate movl instruction");
    }
}

void mos6502_target::translate_movzbl(const ca::instruction_operand &o1, const ca::instruction_operand &o2)
{
    if (o1.is_literal() && o2.is_register())
    {
        emit(mos6502_opcode::lda, o1);
        emit(mos6502_opcode::sta, get_register(o2.reg()));
    }
    else
    {
        throw std::runtime_error("Cannot translate movzbl instruction");
    }
}

void mos6502_target::translate_movzwl(const ca::instruction_operand &o1, const ca::instruction_operand &o2)
{
    throw std::runtime_error("Cannot translate movzwl instruction");
}
