#pragma once

#include "asm_line.h"
#include "instruction_operand.h"

enum class mos6502_opcode
{
    unknown,
    lda,
    ldy,
    tay,
    tya,
    cpy,
    eor,
    sta,
    sty,
    pha,
    pla,
    php,
    plp,
    lsr,
    ror,
    AND,
    inc,
    dec,
    ORA,
    cmp,
    bne,
    beq,
    bmi,
    jmp,
    adc,
    sbc,
    rts,
    clc,
    sec,
    bit,
    jsr
};

struct mos6502 : asm_line
{
    explicit mos6502(const mos6502_opcode o);
    mos6502(const line_type t, std::string s);
    mos6502(const mos6502_opcode o, instruction_operand t_o);

    static auto to_string(const mos6502_opcode o) -> std::string;

    auto to_string() const -> std::string;

    auto opcode() const noexcept
    {
        return opcode_;
    }

    const auto &operand() const noexcept
    {
        return operand_;
    }

    const auto &comment() const noexcept
    {
        return comment_;
    }

    void set_comment(std::string comment) noexcept
    {
        comment_ = std::move(comment);
    }

    auto is_branch() const noexcept
    {
        return is_branch_;
    }

    auto is_comparison() const noexcept
    {
        return is_comparison_;
    }

private:
    mos6502_opcode opcode_ = mos6502_opcode::unknown;
    instruction_operand operand_;
    std::string comment_;
    bool is_branch_ = false;
    bool is_comparison_ = false;
};
