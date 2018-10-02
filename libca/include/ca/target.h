#pragma once

#include <ca/i386.h>
#include <string>
#include <vector>

namespace ca
{

class instruction_operand;

class target
{
public:
    virtual ~target() = default;

    target(target &&) noexcept = delete;
    auto operator=(target &&) noexcept -> target & = delete;

    target(const target &) noexcept = delete;
    auto operator=(const target &) noexcept -> target & = delete;

    virtual void translate_unknown(const std::string &line) = 0;
    virtual void translate_label(const std::string &line) = 0;
    virtual void translate_directive(const std::string &line) = 0;

    void translate(const i386 &opcode);

    virtual void set_current_text(const std::string &text) = 0;
    virtual void finalize() = 0;

    // Bitwise instructions
    virtual void translate_andb(const instruction_operand &o1, const instruction_operand &o2) = 0;
    virtual void translate_andl(const instruction_operand &o1, const instruction_operand &o2) = 0;
    virtual void translate_notb(const instruction_operand &o1, const instruction_operand &o2) = 0;
    virtual void translate_orb(const instruction_operand &o1, const instruction_operand &o2) = 0;
    virtual void translate_orl(const instruction_operand &o1, const instruction_operand &o2) = 0;
    virtual void translate_shrb(const instruction_operand &o1, const instruction_operand &o2) = 0;
    virtual void translate_shrl(const instruction_operand &o1, const instruction_operand &o2) = 0;
    virtual void translate_xorl(const instruction_operand &o1, const instruction_operand &o2) = 0;

    // Branch instructions
    virtual void translate_calll(const instruction_operand &o1, const instruction_operand &o2) = 0;
    virtual void translate_jne(const instruction_operand &o1, const instruction_operand &o2) = 0;
    virtual void translate_je(const instruction_operand &o1, const instruction_operand &o2) = 0;
    virtual void translate_js(const instruction_operand &o1, const instruction_operand &o2) = 0;
    virtual void translate_jmp(const instruction_operand &o1, const instruction_operand &o2) = 0;
    virtual void translate_ret(const instruction_operand &o1, const instruction_operand &o2) = 0;
    virtual void translate_retl(const instruction_operand &o1, const instruction_operand &o2) = 0;

    // Math instructions
    virtual void translate_addb(const instruction_operand &o1, const instruction_operand &o2) = 0;
    virtual void translate_addl(const instruction_operand &o1, const instruction_operand &o2) = 0;
    virtual void translate_cmpb(const instruction_operand &o1, const instruction_operand &o2) = 0;
    virtual void translate_decb(const instruction_operand &o1, const instruction_operand &o2) = 0;
    virtual void translate_decl(const instruction_operand &o1, const instruction_operand &o2) = 0;
    virtual void translate_incb(const instruction_operand &o1, const instruction_operand &o2) = 0;
    virtual void translate_incl(const instruction_operand &o1, const instruction_operand &o2) = 0;
    virtual void translate_negb(const instruction_operand &o1, const instruction_operand &o2) = 0;
    virtual void translate_sall(const instruction_operand &o1, const instruction_operand &o2) = 0;
    virtual void translate_sarl(const instruction_operand &o1, const instruction_operand &o2) = 0;
    virtual void translate_sbbb(const instruction_operand &o1, const instruction_operand &o2) = 0;
    virtual void translate_subb(const instruction_operand &o1, const instruction_operand &o2) = 0;
    virtual void translate_subl(const instruction_operand &o1, const instruction_operand &o2) = 0;
    virtual void translate_testb(const instruction_operand &o1, const instruction_operand &o2) = 0;

    // Move instructions
    virtual void translate_movb(const instruction_operand &o1, const instruction_operand &o2) = 0;
    virtual void translate_movl(const instruction_operand &o1, const instruction_operand &o2) = 0;
    virtual void translate_movzbl(const instruction_operand &o1, const instruction_operand &o2) = 0;
    virtual void translate_movzwl(const instruction_operand &o1, const instruction_operand &o2) = 0;

    // Misc instructions
    virtual void translate_pushl(const instruction_operand &o1, const instruction_operand &o2) = 0;
    virtual void translate_rep(const instruction_operand &o1, const instruction_operand &o2) = 0;

protected:
    target() = default;
};

} // namespace ca
