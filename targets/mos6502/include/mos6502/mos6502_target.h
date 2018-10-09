#pragma once

#include <ca/target.h>
#include <ca/instruction_operand.h>
#include <mos6502/mos6502_instruction.h>
#include <string>
#include <vector>
#include <stdexcept>

class mos6502_target final : public ca::target
{
public:
    mos6502_target() = default;
    virtual ~mos6502_target() = default;

    mos6502_target(mos6502_target &&) noexcept = delete;
    auto operator=(mos6502_target &&) noexcept -> mos6502_target & = delete;

    mos6502_target(const mos6502_target &) noexcept = delete;
    auto operator=(const mos6502_target &) noexcept -> mos6502_target & = delete;

private:
    int parse_literal(const std::string &s)
    {
        return std::stoi(std::string(std::next(std::begin(s)), std::end(s)));
    }

    auto is_8bit_literal(const int value) noexcept
    {
        return value <= 0xff;
    }

    auto is_16bit_literal(const int value) noexcept
    {
        return value > 0xff;
    }

    auto get_16bit_msb(const int value) noexcept
    {
        return static_cast<std::uint8_t>((value >> 8) & 0xff);
    }

    auto get_16bit_lsb(const int value) noexcept
    {
        return static_cast<std::uint8_t>(value & 0xff);
    }

    std::string create_8bit_literal(const int value)
    {
        return "#" + std::to_string(static_cast<uint8_t>(value));
    }

    std::string fixup_8bit_literal(const std::string &s)
    {
        if (s[0] == '$')
        {
            return "#" + std::to_string(static_cast<uint8_t>(parse_literal(s)));
        }

        return s;
    }

    auto get_register(const ca::intel_386_register reg, const int offset = 0) const -> ca::instruction_operand
    {
        switch (reg)
        {
            //  http://sta.c64.org/cbm64mem.html
            case ca::intel_386_register::al:
                return ca::instruction_operand(ca::operand_type::literal, "$03"); // unused, fp->int routine pointer
            case ca::intel_386_register::ah:
                return ca::instruction_operand(ca::operand_type::literal, "$04");
            case ca::intel_386_register::bl:
                return ca::instruction_operand(ca::operand_type::literal, "$05"); // unused, int->fp routine pointer
            case ca::intel_386_register::bh:
                return ca::instruction_operand(ca::operand_type::literal, "$06");
            case ca::intel_386_register::cl:
                return ca::instruction_operand(ca::operand_type::literal, "$fb"); // unused
            case ca::intel_386_register::ch:
                return ca::instruction_operand(ca::operand_type::literal, "$fc"); // unused
            case ca::intel_386_register::dl:
                return ca::instruction_operand(ca::operand_type::literal, "$fd"); // unused
            case ca::intel_386_register::dh:
                return ca::instruction_operand(ca::operand_type::literal, "$fe"); // unused
            case ca::intel_386_register::sil:
                return ca::instruction_operand(ca::operand_type::literal, "$22"); // unused
            case ca::intel_386_register::dil:
                return ca::instruction_operand(ca::operand_type::literal, "$39"); // Current BASIC line number
            case ca::intel_386_register::ax:
            case ca::intel_386_register::eax:
                if (offset == 0)
                    return get_register(ca::intel_386_register::al);
                else if (offset == 1)
                    return get_register(ca::intel_386_register::ah);
                else
                    throw std::runtime_error("Unexpected register offset.");
            case ca::intel_386_register::bx:
            case ca::intel_386_register::ebx:
                if (offset == 0)
                    return get_register(ca::intel_386_register::bl);
                else if (offset == 1)
                    return get_register(ca::intel_386_register::bh);
                else
                    throw std::runtime_error("Unexpected register offset.");
            case ca::intel_386_register::cx:
            case ca::intel_386_register::ecx:
                if (offset == 0)
                    return get_register(ca::intel_386_register::cl);
                else if (offset == 1)
                    return get_register(ca::intel_386_register::ch);
                else
                    throw std::runtime_error("Unexpected register offset.");
            case ca::intel_386_register::dx:
            case ca::intel_386_register::edx:
                if (offset == 0)
                    return get_register(ca::intel_386_register::dl);
                else if (offset == 1)
                    return get_register(ca::intel_386_register::dh);
                else
                    throw std::runtime_error("Unexpected register offset.");
            case ca::intel_386_register::si:
                if (offset == 0)
                    return get_register(ca::intel_386_register::sil);
                else if (offset == 1)
                    return ca::instruction_operand(ca::operand_type::literal, "$23"); // unused
                else
                    throw std::runtime_error("Unexpected register offset.");
            case ca::intel_386_register::di:
                if (offset == 0)
                    return get_register(ca::intel_386_register::dil);
                else if (offset == 1)
                    return ca::instruction_operand(ca::operand_type::literal, "$3a"); // Current BASIC line number
                else
                    throw std::runtime_error("Unexpected register offset.");
            case ca::intel_386_register::sp:
            case ca::intel_386_register::esp:
                if (offset == 0)
                    return ca::instruction_operand(ca::operand_type::literal,
                                                   "$3d"); // Pointer to next BASIC instruction
                else if (offset == 1)
                    return ca::instruction_operand(ca::operand_type::literal,
                                                   "$3e"); // Pointer to next BASIC instruction
                else
                    throw std::runtime_error("Unexpected register offset.");
            default:
                break;
        }

        // TODO: i386_register enum to string
        throw std::runtime_error("Unhandled register: " + std::to_string(static_cast<int>(reg)));
    }

    template <typename... T>
    void emit(T... t)
    {
        instructions_.emplace_back(std::forward<T>(t)...);
        // TODO: This adds the original x86 assembly line as a comment. Find a better way to do this...
        instructions_.back().set_comment(current_text_);
    }

    void set_current_text(const std::string &text) override;
    void finalize() override;

    void translate_unknown(const std::string &line) override;
    void translate_label(const std::string &line) override;
    void translate_directive(const std::string &line) override;

    void translate_andb(const ca::instruction_operand &o1, const ca::instruction_operand &o2) override;
    void translate_andl(const ca::instruction_operand &o1, const ca::instruction_operand &o2) override;
    void translate_notb(const ca::instruction_operand &o1, const ca::instruction_operand &o2) override;
    void translate_orb(const ca::instruction_operand &o1, const ca::instruction_operand &o2) override;
    void translate_orl(const ca::instruction_operand &o1, const ca::instruction_operand &o2) override;
    void translate_shrb(const ca::instruction_operand &o1, const ca::instruction_operand &o2) override;
    void translate_shrl(const ca::instruction_operand &o1, const ca::instruction_operand &o2) override;
    void translate_xorl(const ca::instruction_operand &o1, const ca::instruction_operand &o2) override;

    void translate_calll(const ca::instruction_operand &o1, const ca::instruction_operand &o2) override;
    void translate_jne(const ca::instruction_operand &o1, const ca::instruction_operand &o2) override;
    void translate_je(const ca::instruction_operand &o1, const ca::instruction_operand &o2) override;
    void translate_js(const ca::instruction_operand &o1, const ca::instruction_operand &o2) override;
    void translate_jmp(const ca::instruction_operand &o1, const ca::instruction_operand &o2) override;
    void translate_ret(const ca::instruction_operand &o1, const ca::instruction_operand &o2) override;
    void translate_retl(const ca::instruction_operand &o1, const ca::instruction_operand &o2) override;

    void translate_addb(const ca::instruction_operand &o1, const ca::instruction_operand &o2) override;
    void translate_addl(const ca::instruction_operand &o1, const ca::instruction_operand &o2) override;
    void translate_cmpb(const ca::instruction_operand &o1, const ca::instruction_operand &o2) override;
    void translate_decb(const ca::instruction_operand &o1, const ca::instruction_operand &o2) override;
    void translate_decl(const ca::instruction_operand &o1, const ca::instruction_operand &o2) override;
    void translate_incb(const ca::instruction_operand &o1, const ca::instruction_operand &o2) override;
    void translate_incl(const ca::instruction_operand &o1, const ca::instruction_operand &o2) override;
    void translate_negb(const ca::instruction_operand &o1, const ca::instruction_operand &o2) override;
    void translate_sall(const ca::instruction_operand &o1, const ca::instruction_operand &o2) override;
    void translate_sarl(const ca::instruction_operand &o1, const ca::instruction_operand &o2) override;
    void translate_sbbb(const ca::instruction_operand &o1, const ca::instruction_operand &o2) override;
    void translate_subb(const ca::instruction_operand &o1, const ca::instruction_operand &o2) override;
    void translate_subl(const ca::instruction_operand &o1, const ca::instruction_operand &o2) override;
    void translate_testb(const ca::instruction_operand &o1, const ca::instruction_operand &o2) override;

    void translate_movb(const ca::instruction_operand &o1, const ca::instruction_operand &o2) override;
    void translate_movl(const ca::instruction_operand &o1, const ca::instruction_operand &o2) override;
    void translate_movzbl(const ca::instruction_operand &o1, const ca::instruction_operand &o2) override;
    void translate_movzwl(const ca::instruction_operand &o1, const ca::instruction_operand &o2) override;

    void translate_pushl(const ca::instruction_operand &o1, const ca::instruction_operand &o2) override;
    void translate_rep(const ca::instruction_operand &o1, const ca::instruction_operand &o2) override;

    std::vector<mos6502> instructions_;
    std::string current_text_;
};
