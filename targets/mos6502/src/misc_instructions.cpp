#include <mos6502/mos6502_target.h>
#include "opcodes.h"
#include <ca/instruction_operand.h>

void mos6502_target::translate_pushl(const ca::instruction_operand &o1, const ca::instruction_operand &o2)
{
    if (o1.is_register())
    {
        emit(mos6502_opcode::lda, get_register(o1.register_number()));
        emit(mos6502_opcode::pha);
        emit(mos6502_opcode::lda, get_register(o1.register_number(), 1));
        emit(mos6502_opcode::pha);
    }
    else if (o1.is_literal())
    {
        const auto value = parse_literal(o1.value());

        if (is_8bit_literal(value))
        {
            emit(mos6502_opcode::lda, o1);
            emit(mos6502_opcode::pha);
        }
        else
        {
            emit(mos6502_opcode::lda,
                 ca::instruction_operand(ca::operand_type::literal, create_8bit_literal(get_16bit_msb(value))));
            emit(mos6502_opcode::pha);
            emit(mos6502_opcode::lda,
                 ca::instruction_operand(ca::operand_type::literal, create_8bit_literal(get_16bit_lsb(value))));
            emit(mos6502_opcode::pha);
        }
    }
    else
    {
        throw std::runtime_error("Cannot translate pushl instruction");
    }
}

void mos6502_target::translate_rep(const ca::instruction_operand &o1, const ca::instruction_operand &o2)
{
    throw std::runtime_error("Cannot translate rep instruction");
}
