#pragma once

#include <string>
#include <utility>

struct ASMLine
{
    enum class Type
    {
        Label,
        Instruction,
        Directive,
        MissingOpcode
    };

    ASMLine(Type t, std::string te)
        : type{t}
        , text{std::move(te)}
    {
    }

    Type type;
    std::string text;
};
