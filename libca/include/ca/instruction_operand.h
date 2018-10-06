#pragma once

#include <ca/i386_register.h>
#include <string>
#include <utility>
#include <cassert>

namespace ca
{

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

    instruction_operand(const i386_register reg)
        : type_{operand_type::reg}
        , register_{reg}
    {
    }

    bool operator==(const instruction_operand &other) const
    {
        return type_ == other.type_ && register_ == other.register_ && value_ == other.value_;
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

    auto reg() const noexcept
    {
        assert(type_ == operand_type::reg);
        return register_;
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
    i386_register register_ = i386_register::unknown;
    std::string value_;
};

} // namespace ca
