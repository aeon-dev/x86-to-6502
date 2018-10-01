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
        : type{t}
        , text{std::move(te)}
    {
    }

    line_type type;
    std::string text;
};
