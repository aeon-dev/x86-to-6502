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

    auto get_register(const int reg_num, const int offset = 0) -> ca::instruction_operand
    {
        switch (reg_num)
        {
            //  http://sta.c64.org/cbm64mem.html
            case 0x00:
                return ca::instruction_operand(ca::operand_type::literal, "$03"); // unused, fp->int routine pointer
            case 0x01:
                return ca::instruction_operand(ca::operand_type::literal, "$04");
            case 0x02:
                return ca::instruction_operand(ca::operand_type::literal, "$05"); // unused, int->fp routine pointer
            case 0x03:
                return ca::instruction_operand(ca::operand_type::literal, "$06");
            case 0x04:
                return ca::instruction_operand(ca::operand_type::literal, "$fb"); // unused
            case 0x05:
                return ca::instruction_operand(ca::operand_type::literal, "$fc"); // unused
            case 0x06:
                return ca::instruction_operand(ca::operand_type::literal, "$fd"); // unused
            case 0x07:
                return ca::instruction_operand(ca::operand_type::literal, "$fe"); // unused
            case 0x08:
                return ca::instruction_operand(ca::operand_type::literal, "$22"); // unused
            case 0x09:
                return ca::instruction_operand(ca::operand_type::literal, "$23"); // unused
            case 0x0A:
                return ca::instruction_operand(ca::operand_type::literal, "$39"); // Current BASIC line number
            case 0x0B:
                return ca::instruction_operand(ca::operand_type::literal, "$3a"); // Current BASIC line number
            case 0x0C:
                return ca::instruction_operand(ca::operand_type::literal, "$3d"); // Pointer to next BASIC instruction
            case 0x0D:
                return ca::instruction_operand(ca::operand_type::literal, "$3e"); // Pointer to next BASIC instruction
            case 0x10:
                return get_register(0x00 + offset);
            case 0x11:
                return get_register(0x02 + offset);
            case 0x12:
                return get_register(0x04 + offset);
            case 0x13:
                return get_register(0x06 + offset);
            case 0x14:
                return get_register(0x08 + offset);
            case 0x15:
                return get_register(0x0A + offset);
            case 0x16:
                return get_register(0x0C + offset);
            default:
                break;
        }

        throw std::runtime_error("Unhandled register number: " + std::to_string(reg_num));
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
