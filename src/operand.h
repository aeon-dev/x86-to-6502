#pragma once

#include <string>
#include <utility>
#include <cassert>

enum class operand_type
{
    empty,
    literal,
    reg /*ister*/
};

struct operand
{
    operand_type type = operand_type::empty;
    int reg_num = 0;
    std::string value;

    operand() = default;

    bool operator==(const operand &other) const
    {
        return type == other.type && reg_num == other.reg_num && value == other.value;
    }

    operand(const operand_type t, std::string v)
        : type(t)
        , value(std::move(v))
    {
        assert(type == operand_type::literal);
    }

    operand(const operand_type t, const int num)
        : type(t)
        , reg_num(num)
    {
        assert(type == operand_type::reg);
    }
};
