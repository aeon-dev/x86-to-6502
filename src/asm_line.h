#pragma once

#include <string>
#include <utility>

struct asm_line
{
    enum class line_type
    {
        Label,
        Instruction,
        Directive,
        MissingOpcode
    };

    asm_line(const line_type t, std::string te)
        : type_{t}
        , text_{std::move(te)}
    {
    }

    auto type() const noexcept
    {
        return type_;
    }

    auto is_label() const noexcept
    {
        return type_ == line_type::Label;
    }

    auto is_instruction() const noexcept
    {
        return type_ == line_type::Instruction;
    }

    auto is_directive() const noexcept
    {
        return type_ == line_type::Directive;
    }

    auto is_missing_opcode() const noexcept
    {
        return type_ == line_type::MissingOpcode;
    }

    const auto &text() const noexcept
    {
        return text_;
    }

    void set_text(std::string text) noexcept
    {
        text_ = std::move(text);
    }

private:
    line_type type_;
    std::string text_;
};
