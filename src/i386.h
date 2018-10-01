#pragma once

#include "asm_line.h"
#include "operand.h"

struct i386 : ASMLine
{
    enum class OpCode
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

    i386(const int t_line_num, std::string t_line_text, Type t, std::string t_opcode, std::string o1 = "",
         std::string o2 = "");

    static auto parse_opcode(Type t, const std::string &o) -> OpCode;
    static auto parse_operand(std::string o) -> Operand;

    int line_num;
    std::string line_text;
    OpCode opcode;
    Operand operand1;
    Operand operand2;
};
