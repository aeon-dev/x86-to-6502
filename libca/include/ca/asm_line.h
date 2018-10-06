#pragma once

#include <string>
#include <utility>

namespace ca
{

struct asm_line
{
    enum class line_type
    {
        empty,
        comment,
        label,
        instruction,
        directive,
        missing_opcode
    };

    asm_line()
        : type_{line_type::empty}
        , text_{}
    {
    }

    explicit asm_line(std::string line)
        : type_{line_type::comment}
        , text_{std::move(line)}
    {
    }

    asm_line(const line_type t, std::string te)
        : type_{t}
        , text_{std::move(te)}
    {
    }

    auto type() const noexcept
    {
        return type_;
    }

    auto is_empty() const noexcept
    {
        return type_ == line_type::empty;
    }

    auto is_comment() const noexcept
    {
        return type_ == line_type::comment;
    }

    auto is_label() const noexcept
    {
        return type_ == line_type::label;
    }

    auto is_instruction() const noexcept
    {
        return type_ == line_type::instruction;
    }

    auto is_directive() const noexcept
    {
        return type_ == line_type::directive;
    }

    auto is_missing_opcode() const noexcept
    {
        return type_ == line_type::missing_opcode;
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

} // namespace ca
