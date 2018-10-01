#pragma once

#include <string>
#include <utility>
#include <cassert>

struct Operand
{
    enum class Type
    {
        empty,
        literal,
        reg /*ister*/
    };

    Type type = Type::empty;
    int reg_num = 0;
    std::string value;

    Operand() = default;

    bool operator==(const Operand &other) const
    {
        return type == other.type && reg_num == other.reg_num && value == other.value;
    }

    Operand(const Type t, std::string v)
        : type(t)
        , value(std::move(v))
    {
        assert(type == Type::literal);
    }

    Operand(const Type t, const int num)
        : type(t)
        , reg_num(num)
    {
        assert(type == Type::reg);
    }
};
