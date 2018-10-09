#pragma once

#include <ca/target.h>

namespace ca
{

class intel_386;

class cross_assembler
{
public:
    cross_assembler(target &t);
    ~cross_assembler() = default;

    cross_assembler(cross_assembler &&) noexcept = delete;
    auto operator=(cross_assembler &&) noexcept -> cross_assembler & = delete;

    cross_assembler(const cross_assembler &) noexcept = delete;
    auto operator=(const cross_assembler &) noexcept -> cross_assembler & = delete;

    void assemble();

private:
    target &target_;
};

} // namespace ca
