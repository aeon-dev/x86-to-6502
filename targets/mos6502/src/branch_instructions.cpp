#include <mos6502/mos6502_target.h>
#include "opcodes.h"
#include <ca/instruction_operand.h>

void mos6502_target::translate_calll(const ca::instruction_operand &o1, const ca::instruction_operand &o2)
{
    if (o1.is_literal())
    {
        emit(mos6502_opcode::jsr, o1);
    }
    else
    {
        throw std::runtime_error("Cannot translate calll instruction");
    }
}

void mos6502_target::translate_jne(const ca::instruction_operand &o1, const ca::instruction_operand &o2)
{
    emit(mos6502_opcode::bne, o1);
}

void mos6502_target::translate_je(const ca::instruction_operand &o1, const ca::instruction_operand &o2)
{
    emit(mos6502_opcode::beq, o1);
}

void mos6502_target::translate_js(const ca::instruction_operand &o1, const ca::instruction_operand &o2)
{
    emit(mos6502_opcode::bmi, o1);
}

void mos6502_target::translate_jmp(const ca::instruction_operand &o1, const ca::instruction_operand &o2)
{
    emit(mos6502_opcode::jmp, o1);
}

void mos6502_target::translate_ret(const ca::instruction_operand &o1, const ca::instruction_operand &o2)
{
    // If we're inside of an interrupt handler, we should emit a "return from interrupt" instruction instead.
    if (current_label() == "nmi" || current_label() == "irq")
    {
        emit(mos6502_opcode::rti);
    }
    else
    {
        emit(mos6502_opcode::rts);
    }
}

void mos6502_target::translate_retl(const ca::instruction_operand &o1, const ca::instruction_operand &o2)
{
    // If we're inside of an interrupt handler, we should emit a "return from interrupt" instruction instead.
    if (current_label() == "nmi" || current_label() == "irq")
    {
        emit(mos6502_opcode::rti);
    }
    else
    {
        /// \todo I don't know if this is completely correct for retl translation
        emit(mos6502_opcode::rts);
    }
}
