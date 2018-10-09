#include <mos6502/mos6502_target.h>
#include "opcodes.h"
#include <ca/instruction_operand.h>

void mos6502_target::translate_addb(const ca::instruction_operand &o1, const ca::instruction_operand &o2)
{
    if (o1.is_literal() && o2.is_register())
    {
        emit(mos6502_opcode::lda, get_register(o2.reg()));
        emit(mos6502_opcode::clc);
        emit(mos6502_opcode::adc, ca::instruction_operand(ca::operand_type::literal, fixup_8bit_literal(o1.value())));
        emit(mos6502_opcode::sta, get_register(o2.reg()));
    }
    else if (o1.is_literal() && o2.is_literal())
    {
        emit(mos6502_opcode::lda, ca::instruction_operand(ca::operand_type::literal, fixup_8bit_literal(o1.value())));
        emit(mos6502_opcode::clc);
        emit(mos6502_opcode::adc, o2);
        emit(mos6502_opcode::sta, o2);
    }
    else if (o1.is_register() && o2.is_literal())
    {
        emit(mos6502_opcode::lda, get_register(o1.reg()));
        emit(mos6502_opcode::clc);
        emit(mos6502_opcode::adc, o2);
        emit(mos6502_opcode::sta, o2);
    }
    else
    {
        throw std::runtime_error("Cannot translate addb instruction");
    }
}

// TODO: Fixup 16-bit add. addl is generated when doing cdecl calling conventions to change the stack pointer.
void mos6502_target::translate_addl(const ca::instruction_operand &o1, const ca::instruction_operand &o2)
{
    if (o1.is_literal() && o2.is_register())
    {
        // TODO: Is it the stack pointer (SP/ESP)? Then skip emitting this code.
        // This should probably be an optimization step instead as it should only
        // be skipped if it's used in context of calling a cdecl function.
        if (o2.reg() != ca::intel_386_register::sp && o2.reg() != ca::intel_386_register::esp)
        {
            emit(mos6502_opcode::lda, get_register(o2.reg()));
            emit(mos6502_opcode::clc);
            emit(mos6502_opcode::adc,
                 ca::instruction_operand(ca::operand_type::literal, fixup_8bit_literal(o1.value())));
            emit(mos6502_opcode::sta, get_register(o2.reg()));
        }
    }
    else if (o1.is_literal() && o2.is_literal())
    {
        emit(mos6502_opcode::lda, ca::instruction_operand(ca::operand_type::literal, fixup_8bit_literal(o1.value())));
        emit(mos6502_opcode::clc);
        emit(mos6502_opcode::adc, o2);
        emit(mos6502_opcode::sta, o2);
    }
    else if (o1.is_register() && o2.is_literal())
    {
        emit(mos6502_opcode::lda, get_register(o1.reg()));
        emit(mos6502_opcode::clc);
        emit(mos6502_opcode::adc, o2);
        emit(mos6502_opcode::sta, o2);
    }
    else
    {
        throw std::runtime_error("Cannot translate addb instruction");
    }
}

void mos6502_target::translate_cmpb(const ca::instruction_operand &o1, const ca::instruction_operand &o2)
{
    if (o1.is_literal() && o2.is_literal())
    {
        emit(mos6502_opcode::lda, o2);
        emit(mos6502_opcode::cmp, ca::instruction_operand(ca::operand_type::literal, fixup_8bit_literal(o1.value())));
    }
    else if (o1.is_literal() && o2.is_register())
    {
        emit(mos6502_opcode::lda, get_register(o2.reg()));
        emit(mos6502_opcode::cmp, ca::instruction_operand(ca::operand_type::literal, fixup_8bit_literal(o1.value())));
    }
    else
    {
        throw std::runtime_error("Cannot translate cmpb instruction");
    }
}

void mos6502_target::translate_decb(const ca::instruction_operand &o1, const ca::instruction_operand &o2)
{
    if (o1.is_register())
    {
        emit(mos6502_opcode::dec, get_register(o1.reg()));
    }
    else
    {
        emit(mos6502_opcode::dec, o1);
    }
}

void mos6502_target::translate_decl(const ca::instruction_operand &o1, const ca::instruction_operand &o2)
{
    throw std::runtime_error("Cannot translate decl instruction");
}

void mos6502_target::translate_incb(const ca::instruction_operand &o1, const ca::instruction_operand &o2)
{
    if (o1.is_register())
    {
        emit(mos6502_opcode::inc, get_register(o1.reg()));
    }
    else
    {
        emit(mos6502_opcode::inc, o1);
    }
}

void mos6502_target::translate_incl(const ca::instruction_operand &o1, const ca::instruction_operand &o2)
{
    throw std::runtime_error("Cannot translate incl instruction");
}

void mos6502_target::translate_negb(const ca::instruction_operand &o1, const ca::instruction_operand &o2)
{
    if (o1.is_register())
    {
        // perform a two's complement of the register location
        emit(mos6502_opcode::lda, get_register(o1.reg()));
        emit(mos6502_opcode::eor, ca::instruction_operand(ca::operand_type::literal, "#$ff"));
        emit(mos6502_opcode::sta, get_register(o1.reg()));
        emit(mos6502_opcode::inc, get_register(o1.reg()));
    }
    else
    {
        throw std::runtime_error("Cannot translate negb instruction");
    }
}

void mos6502_target::translate_sall(const ca::instruction_operand &o1, const ca::instruction_operand &o2)
{
    throw std::runtime_error("Cannot translate sall instruction");
}

void mos6502_target::translate_sarl(const ca::instruction_operand &o1, const ca::instruction_operand &o2)
{
    throw std::runtime_error("Cannot translate sarl instruction");
}

void mos6502_target::translate_sbbb(const ca::instruction_operand &o1, const ca::instruction_operand &o2)
{
    // DEST <- (DEST – (SRC + CF))
    // o2 <- (o2 - (o1 + cf))
    // if o1 and o2 are the same we get
    // o2 <- (o2 - (o2 + cf))
    // o2 <- -cf
    if (o1.is_register() && o2.is_register() && o1.reg() == o2.reg())
    {
        emit(mos6502_opcode::lda, ca::instruction_operand(ca::operand_type::literal, "#$00")); // reset a
        emit(mos6502_opcode::sbc,
             ca::instruction_operand(ca::operand_type::literal, "#$00")); // subtract out the carry flag
        emit(mos6502_opcode::eor, ca::instruction_operand(ca::operand_type::literal, "#$ff")); // invert the bits
        emit(mos6502_opcode::sta, get_register(o2.reg()));                                     // place the value
    }
    else
    {
        throw std::runtime_error("Cannot translate sbbb instruction");
    }
}

void mos6502_target::translate_subb(const ca::instruction_operand &o1, const ca::instruction_operand &o2)
{
    // DEST <- DEST - SRC
    // o2 = o2 - o1
    // Ensure that we set the carry flag before performing the subtraction
    if (o1.is_register() && o2.is_literal())
    {
        emit(mos6502_opcode::lda, o2);
        emit(mos6502_opcode::sec);
        emit(mos6502_opcode::sbc, get_register(o1.reg()));
        emit(mos6502_opcode::sta, o2);
    }
    else
    {
        throw std::runtime_error("Cannot translate subb instruction");
    }
}

void mos6502_target::translate_subl(const ca::instruction_operand &o1, const ca::instruction_operand &o2)
{
    throw std::runtime_error("Cannot translate subl instruction");
}

void mos6502_target::translate_testb(const ca::instruction_operand &o1, const ca::instruction_operand &o2)
{
    if (o1.is_register() && o2.is_register() && o1.reg() == o2.reg())
    {
        // this just tests the register for 0
        emit(mos6502_opcode::lda, get_register(o1.reg()));
        //        instructions.emplace_back(mos6502_opcode::bit, Operand(Operand::Type::literal, "#$00"));
    }
    else if (o1.is_register() && o2.is_register())
    {
        // ands the values
        emit(mos6502_opcode::lda, get_register(o1.reg()));
        emit(mos6502_opcode::bit, get_register(o2.reg()));
    }
    else if (o1.is_literal() && o2.is_register())
    {
        // ands the values
        emit(mos6502_opcode::lda, ca::instruction_operand(ca::operand_type::literal, fixup_8bit_literal(o1.value())));
        emit(mos6502_opcode::bit, get_register(o2.reg()));
    }
    else if (o1.is_literal() && o2.is_literal())
    {
        // ands the values
        emit(mos6502_opcode::lda, ca::instruction_operand(ca::operand_type::literal, fixup_8bit_literal(o1.value())));
        emit(mos6502_opcode::bit, o2);
    }
    else
    {
        throw std::runtime_error("Cannot translate testb instruction");
    }
}
