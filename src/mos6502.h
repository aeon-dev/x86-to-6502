#pragma once

#include "asm_line.h"
#include "operand.h"

struct mos6502 : ASMLine
{
    enum class OpCode
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

    explicit mos6502(const OpCode o);
    mos6502(const Type t, std::string s);
    mos6502(const OpCode o, Operand t_o);

    static auto get_is_branch(const OpCode o) -> bool;

    static auto get_is_comparison(const OpCode o) -> bool;

    static auto to_string(const OpCode o) -> std::string;

    auto to_string() const -> std::string;

    OpCode opcode = OpCode::unknown;
    Operand op;
    std::string comment;
    bool is_branch = false;
    bool is_comparison = false;
};
