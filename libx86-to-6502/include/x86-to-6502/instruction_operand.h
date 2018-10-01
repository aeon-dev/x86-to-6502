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

class instruction_operand
{
public:
    instruction_operand() = default;

    instruction_operand(const operand_type t, std::string v)
        : type_{t}
        , value_{std::move(v)}
    {
        assert(type_ == operand_type::literal);
    }

    instruction_operand(const operand_type t, const int num)
        : type_{t}
        , reg_num_{num}
    {
        assert(type_ == operand_type::reg);
    }

    bool operator==(const instruction_operand &other) const
    {
        return type_ == other.type_ && reg_num_ == other.reg_num_ && value_ == other.value_;
    }

    auto is_empty() const noexcept
    {
        return type_ == operand_type::empty;
    }

    auto is_literal() const noexcept
    {
        return type_ == operand_type::literal;
    }

    auto is_register() const noexcept
    {
        return type_ == operand_type::reg;
    }

    auto type() const noexcept
    {
        return type_;
    }

    auto register_number() const noexcept
    {
        assert(type_ == operand_type::reg);
        return reg_num_;
    }

    const auto &value() const noexcept
    {
        return value_;
    }

    void set_value(std::string value) noexcept
    {
        value_ = std::move(value);
    }

private:
    operand_type type_ = operand_type::empty;
    int reg_num_ = 0;
    std::string value_;
};
