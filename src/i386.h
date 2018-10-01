#pragma once

#include "asm_line.h"
#include "operand.h"

enum class i386_opcode
{
    unknown,
    movzbl,
    movzwl,
    shrb,
    shrl,
    xorl,
    andl,
    andb,
    addb,
    ret,
    movb,
    cmpb,
    movl,
    jmp,
    jne,
    je,
    js,
    testb,
    incl,
    incb,
    decl,
    decb,
    sarl,
    addl,
    subl,
    subb,
    sall,
    orl,
    orb,
    rep,
    pushl,
    sbbb,
    negb,
    notb,
    retl,
    calll
};

struct i386 : asm_line
{
    i386(const int t_line_num, std::string t_line_text, line_type t, std::string t_opcode, std::string o1 = "",
         std::string o2 = "");

    static auto parse_opcode(line_type t, const std::string &o) -> i386_opcode;
    static auto parse_operand(std::string o) -> operand;

    int line_num;
    std::string line_text;
    i386_opcode opcode;
    operand operand1;
    operand operand2;
};
