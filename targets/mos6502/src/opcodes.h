#pragma once

enum class mos6502_opcode : int
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
    rti,
    clc,
    sec,
    bit,
    jsr
};
