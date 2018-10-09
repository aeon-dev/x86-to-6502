#include <mos6502/mos6502_target.h>
#include "opcodes.h"
#include <map>
#include <iostream>

namespace internal
{

static auto optimize(std::vector<mos6502> &instructions) -> bool
{
    if (instructions.size() < 2)
        return false;

    const auto next_instruction = [&instructions](auto i) {
        do
        {
            ++i;
        } while (i < instructions.size() && instructions[i].is_directive());
        return i;
    };

    for (size_t op = 0; op < instructions.size() - 1; ++op)
    {
        // look for a transfer of Y -> A immediately followed by A -> Y
        if (instructions[op].opcode() == mos6502_opcode::tya)
        {
            next_instruction(op);
            if (instructions[op].opcode() == mos6502_opcode::tay)
            {
                instructions.erase(std::next(std::begin(instructions), op),
                                   std::next(std::begin(instructions), op + 1));
                return true;
            }
        }
    }

    for (size_t op = 0; op < instructions.size() - 1; ++op)
    {
        // look for a store A -> loc immediately followed by loc -> A
        if (instructions[op].opcode() == mos6502_opcode::sta)
        {
            const auto next = next_instruction(op);
            if (instructions[next].opcode() == mos6502_opcode::lda &&
                instructions[next].operand() == instructions[op].operand())
            {
                instructions.erase(std::next(std::begin(instructions), next),
                                   std::next(std::begin(instructions), next + 1));
                return true;
            }
        }
    }

    for (size_t op = 0; op < instructions.size() - 1; ++op)
    {
        if (instructions[op].opcode() == mos6502_opcode::lda && instructions[op].operand().is_literal())
        {
            const auto operand = instructions[op].operand();
            auto op2 = op + 1;
            // look for multiple stores of the same value
            while (op2 < instructions.size() &&
                   (instructions[op2].opcode() == mos6502_opcode::sta || instructions[op2].is_directive()))
            {
                ++op2;
            }
            if (instructions[op2].opcode() == mos6502_opcode::lda && operand == instructions[op2].operand())
            {
                instructions.erase(std::next(std::begin(instructions), op2),
                                   std::next(std::begin(instructions), op2 + 1));
                return true;
            }
        }
    }

    return false;
}

static auto fix_long_branches(std::vector<mos6502> &instructions, int &branch_patch_count) -> bool
{
    std::map<std::string, size_t> labels;
    for (size_t op = 0; op < instructions.size(); ++op)
    {
        if (instructions[op].is_label())
        {
            labels[instructions[op].text()] = op;
        }
    }

    for (size_t op = 0; op < instructions.size(); ++op)
    {
        if (instructions[op].is_branch() &&
            std::abs(static_cast<int>(labels[instructions[op].operand().value()]) - static_cast<int>(op)) * 3 > 255)
        {
            ++branch_patch_count;
            const auto going_to = instructions[op].operand().value();
            const auto new_pos = "patch_" + std::to_string(branch_patch_count);
            // uh-oh too long of a branch, have to convert this to a jump...
            if (instructions[op].opcode() == mos6502_opcode::bne)
            {
                const auto comment = instructions[op].comment();
                instructions[op] =
                    mos6502(mos6502_opcode::beq, ca::instruction_operand(ca::operand_type::literal, new_pos));
                instructions.insert(
                    std::next(std::begin(instructions), op + 1),
                    mos6502(mos6502_opcode::jmp, ca::instruction_operand(ca::operand_type::literal, going_to)));
                instructions.insert(std::next(std::begin(instructions), op + 2),
                                    mos6502(ca::asm_line::line_type::label, new_pos));

                instructions[op].set_comment(comment);
                instructions[op + 1].set_comment(comment);
                instructions[op + 2].set_comment(comment);
                return true;
            }
            else
            {
                throw std::runtime_error("Don't know how to reorg this branch");
            }
        }
    }
    return false;
}

static auto fix_overwritten_flags(std::vector<mos6502> &instructions) -> bool
{
    if (instructions.size() < 3)
    {
        return false;
    }

    for (size_t op = 0; op < instructions.size(); ++op)
    {
        if (instructions[op].is_comparison())
        {
            auto op2 = op + 1;
            while (op2 < instructions.size() && !instructions[op2].is_comparison() && !instructions[op2].is_branch())
            {
                ++op2;
            }

            if (op2 < instructions.size() && (op2 - op) > 1 && instructions[op2 - 1].opcode() != mos6502_opcode::plp)
            {
                if (instructions[op2].is_comparison())
                {
                    continue;
                }

                if (instructions[op2].is_branch())
                {
                    // insert a pull of processor status before the branch
                    instructions.insert(std::next(std::begin(instructions), op2), mos6502(mos6502_opcode::plp));
                    // insert a push of processor status after the comparison
                    instructions.insert(std::next(std::begin(instructions), op + 1), mos6502(mos6502_opcode::php));

                    return true;
                }
            }
        }
    }

    return false;
}

} // namespace internal

void mos6502_target::set_current_text(const std::string &text)
{
    current_text_ = text;
}

void mos6502_target::finalize()
{
    while (internal::fix_overwritten_flags(instructions_))
    {
        // do it however many times it takes
    }

    while (internal::optimize(instructions_))
    {
        // do it however many times it takes
    }

    int branch_patch_count = 0;
    while (internal::fix_long_branches(instructions_, branch_patch_count))
    {
        // do it however many times it takes
    }

    for (const auto &i : instructions_)
    {
        std::cout << i.to_string() << '\n';
    }
}

void mos6502_target::translate_unknown(const std::string &line)
{
    emit(ca::asm_line::line_type::missing_opcode, line);
}

void mos6502_target::translate_label(const std::string &line)
{
    emit(ca::asm_line::line_type::label, line);

    // Update the current label only if it's one of the known functions.
    // We don't care about the intermediate labels. This way we can emit
    // rti and rts correctly.
    if (line == "nmi" || line == "irq" || line == "main")
        current_label_ = line;
}

void mos6502_target::translate_directive(const std::string &line)
{
    emit(ca::asm_line::line_type::directive, line);
}
