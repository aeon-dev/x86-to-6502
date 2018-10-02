#pragma once

#include <ca/asm_line.h>
#include <ca/instruction_operand.h>

enum class mos6502_opcode : int;

struct mos6502 : ca::asm_line
{
    explicit mos6502(const mos6502_opcode o);
    mos6502(const line_type t, std::string s);
    mos6502(const mos6502_opcode o, ca::instruction_operand t_o);

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
    mos6502_opcode opcode_;
    ca::instruction_operand operand_;
    std::string comment_;
    bool is_branch_ = false;
    bool is_comparison_ = false;
};
