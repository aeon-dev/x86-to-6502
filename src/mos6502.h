#pragma once

#include "asm_line.h"
#include "operand.h"

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
    mos6502(const mos6502_opcode o, operand t_o);

    static auto get_is_branch(const mos6502_opcode o) -> bool;

    static auto get_is_comparison(const mos6502_opcode o) -> bool;

    static auto to_string(const mos6502_opcode o) -> std::string;

    auto to_string() const -> std::string;

    mos6502_opcode opcode = mos6502_opcode::unknown;
    operand op;
    std::string comment;
    bool is_branch = false;
    bool is_comparison = false;
};
