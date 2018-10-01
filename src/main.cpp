#include "asm_line.h"
#include "operand.h"
#include "mos6502.h"
#include "i386.h"

#include <iostream>
#include <vector>
#include <set>
#include <string>
#include <regex>
#include <map>
#include <cctype>

int parse_8bit_literal(const std::string &s)
{
    return std::stoi(std::string(std::next(std::begin(s)), std::end(s)));
}

std::string fixup_8bit_literal(const std::string &s)
{
    if (s[0] == '$')
    {
        return "#" + std::to_string(static_cast<uint8_t>(parse_8bit_literal(s)));
    }

    return s;
}

operand get_register(const int reg_num, const int offset = 0)
{
    switch (reg_num)
    {
        //  http://sta.c64.org/cbm64mem.html
        case 0x00:
            return operand(operand_type::literal, "$03"); // unused, fp->int routine pointer
        case 0x01:
            return operand(operand_type::literal, "$04");
        case 0x02:
            return operand(operand_type::literal, "$05"); // unused, int->fp routine pointer
        case 0x03:
            return operand(operand_type::literal, "$06");
        case 0x04:
            return operand(operand_type::literal, "$fb"); // unused
        case 0x05:
            return operand(operand_type::literal, "$fc"); // unused
        case 0x06:
            return operand(operand_type::literal, "$fd"); // unused
        case 0x07:
            return operand(operand_type::literal, "$fe"); // unused
        case 0x08:
            return operand(operand_type::literal, "$22"); // unused
        case 0x09:
            return operand(operand_type::literal, "$23"); // unused
        case 0x0A:
            return operand(operand_type::literal, "$39"); // Current BASIC line number
        case 0x0B:
            return operand(operand_type::literal, "$3a"); // Current BASIC line number
        case 0x10:
            return get_register(0x00 + offset);
        case 0x11:
            return get_register(0x02 + offset);
        case 0x12:
            return get_register(0x04 + offset);
        case 0x13:
            return get_register(0x06 + offset);
        case 0x14:
            return get_register(0x08 + offset);
        case 0x15:
            return get_register(0x0A + offset);
        default:
            break;
    }

    throw std::runtime_error("Unhandled register number: " + std::to_string(reg_num));
}

void translate_instruction(std::vector<mos6502> &instructions, const std::string &line_text, const i386_opcode op,
                           const operand &o1, const operand &o2)
{
    switch (op)
    {
        case i386_opcode::unknown:
            instructions.emplace_back(asm_line::line_type::MissingOpcode, line_text);
            break;
        case i386_opcode::ret:
            instructions.emplace_back(mos6502_opcode::rts);
            break;
        case i386_opcode::retl:
            /// \todo I don't know if this is completely correct for retl translation
            instructions.emplace_back(mos6502_opcode::rts);
            break;
        case i386_opcode::movl:
            if (o1.type == operand_type::reg && o2.type == operand_type::reg)
            {
                instructions.emplace_back(mos6502_opcode::lda, get_register(o1.reg_num));
                instructions.emplace_back(mos6502_opcode::sta, get_register(o2.reg_num));
                instructions.emplace_back(mos6502_opcode::lda, get_register(o1.reg_num, 1));
                instructions.emplace_back(mos6502_opcode::sta, get_register(o2.reg_num, 1));
            }
            else if (o1.type == operand_type::literal && o2.type == operand_type::reg)
            {
                instructions.emplace_back(mos6502_opcode::lda, operand(o1.type, "#<" + o1.value));
                instructions.emplace_back(mos6502_opcode::sta, get_register(o2.reg_num));
                instructions.emplace_back(mos6502_opcode::lda, operand(o1.type, "#>" + o1.value));
                instructions.emplace_back(mos6502_opcode::sta, get_register(o2.reg_num, 1));
            }
            else
            {
                throw std::runtime_error("Cannot translate movl instruction");
            }
            break;
        case i386_opcode::xorl:
            if (o1.type == operand_type::reg && o2.type == operand_type::reg && o1.reg_num == o2.reg_num)
            {
                instructions.emplace_back(mos6502_opcode::lda, operand(operand_type::literal, "#$00"));
                instructions.emplace_back(mos6502_opcode::sta, get_register(o2.reg_num));
                instructions.emplace_back(mos6502_opcode::sta, get_register(o2.reg_num, 1));
            }
            else
            {
                throw std::runtime_error("Cannot translate xorl instruction");
            }
            break;
        case i386_opcode::movb:
            if (o1.type == operand_type::literal && o2.type == operand_type::literal)
            {
                instructions.emplace_back(mos6502_opcode::lda, operand(o1.type, fixup_8bit_literal(o1.value)));
                instructions.emplace_back(mos6502_opcode::sta, o2);
            }
            else if (o1.type == operand_type::literal && o2.type == operand_type::reg)
            {
                instructions.emplace_back(mos6502_opcode::lda, operand(o1.type, fixup_8bit_literal(o1.value)));
                instructions.emplace_back(mos6502_opcode::sta, get_register(o2.reg_num));
            }
            else if (o1.type == operand_type::reg && o2.type == operand_type::literal)
            {
                instructions.emplace_back(mos6502_opcode::lda, get_register(o1.reg_num));
                instructions.emplace_back(mos6502_opcode::sta, o2);
            }
            else if (o1.type == operand_type::reg && o2.type == operand_type::reg)
            {
                instructions.emplace_back(mos6502_opcode::lda, get_register(o1.reg_num));
                instructions.emplace_back(mos6502_opcode::sta, get_register(o2.reg_num));
            }
            else
            {
                throw std::runtime_error("Cannot translate movb instruction");
            }
            break;
        case i386_opcode::orb:
            if (o1.type == operand_type::literal && o2.type == operand_type::literal)
            {
                instructions.emplace_back(mos6502_opcode::lda, operand(o1.type, fixup_8bit_literal(o1.value)));
                instructions.emplace_back(mos6502_opcode::ORA, o2);
                instructions.emplace_back(mos6502_opcode::sta, o2);
            }
            else if (o1.type == operand_type::reg && o2.type == operand_type::reg)
            {
                instructions.emplace_back(mos6502_opcode::lda, get_register(o1.reg_num));
                instructions.emplace_back(mos6502_opcode::ORA, get_register(o2.reg_num));
                instructions.emplace_back(mos6502_opcode::sta, get_register(o2.reg_num));
            }
            else if (o1.type == operand_type::literal && o2.type == operand_type::reg)
            {
                instructions.emplace_back(mos6502_opcode::lda, operand(o1.type, fixup_8bit_literal(o1.value)));
                instructions.emplace_back(mos6502_opcode::ORA, get_register(o2.reg_num));
                instructions.emplace_back(mos6502_opcode::sta, get_register(o2.reg_num));
            }
            else
            {
                throw std::runtime_error("Cannot translate orb instruction");
            }
            break;

        case i386_opcode::movzbl:
            if (o1.type == operand_type::literal && o2.type == operand_type::reg)
            {
                instructions.emplace_back(mos6502_opcode::lda, o1);
                instructions.emplace_back(mos6502_opcode::sta, get_register(o2.reg_num));
            }
            else
            {
                throw std::runtime_error("Cannot translate movzbl instruction");
            }
            break;
        case i386_opcode::shrb:
            if (o1.type == operand_type::reg || o2.type == operand_type::reg)
            {
                const auto do_shift = [&instructions](const int reg_num) {
                    instructions.emplace_back(mos6502_opcode::lsr, get_register(reg_num));
                };

                if (o1.type == operand_type::literal)
                {
                    const auto count = parse_8bit_literal(o1.value);
                    for (int i = 0; i < count; ++i)
                    {
                        do_shift(o2.reg_num);
                    }
                }
                else
                {
                    do_shift(o1.reg_num);
                }
            }
            else
            {
                throw std::runtime_error("Cannot translate shrb instruction");
            }
            break;
        case i386_opcode::shrl:
            if (o1.type == operand_type::reg || o2.type == operand_type::reg)
            {
                const auto do_shift = [&instructions](const int reg_num) {
                    instructions.emplace_back(mos6502_opcode::lsr, get_register(reg_num, 1));
                    instructions.emplace_back(mos6502_opcode::ror, get_register(reg_num));
                };

                if (o1.type == operand_type::literal)
                {
                    const auto count = parse_8bit_literal(o1.value);
                    for (int i = 0; i < count; ++i)
                    {
                        do_shift(o2.reg_num);
                    }
                }
                else
                {
                    do_shift(o1.reg_num);
                }
            }
            else
            {
                throw std::runtime_error("Cannot translate shrl instruction");
            }
            break;
        case i386_opcode::testb:
            if (o1.type == operand_type::reg && o2.type == operand_type::reg && o1.reg_num == o2.reg_num)
            {
                // this just tests the register for 0
                instructions.emplace_back(mos6502_opcode::lda, get_register(o1.reg_num));
                //        instructions.emplace_back(mos6502_opcode::bit, Operand(Operand::Type::literal, "#$00"));
            }
            else if (o1.type == operand_type::reg && o2.type == operand_type::reg)
            {
                // ands the values
                instructions.emplace_back(mos6502_opcode::lda, get_register(o1.reg_num));
                instructions.emplace_back(mos6502_opcode::bit, get_register(o2.reg_num));
            }
            else if (o1.type == operand_type::literal && o2.type == operand_type::reg)
            {
                // ands the values
                instructions.emplace_back(mos6502_opcode::lda, operand(o1.type, fixup_8bit_literal(o1.value)));
                instructions.emplace_back(mos6502_opcode::bit, get_register(o2.reg_num));
            }
            else if (o1.type == operand_type::literal && o2.type == operand_type::literal)
            {
                // ands the values
                instructions.emplace_back(mos6502_opcode::lda, operand(o1.type, fixup_8bit_literal(o1.value)));
                instructions.emplace_back(mos6502_opcode::bit, o2);
            }
            else
            {
                throw std::runtime_error("Cannot translate testb instruction");
            }
            break;
        case i386_opcode::decb:
            if (o1.type == operand_type::reg)
            {
                instructions.emplace_back(mos6502_opcode::dec, get_register(o1.reg_num));
            }
            else
            {
                instructions.emplace_back(mos6502_opcode::dec, o1);
            }
            break;
        case i386_opcode::incb:
            if (o1.type == operand_type::reg)
            {
                instructions.emplace_back(mos6502_opcode::inc, get_register(o1.reg_num));
            }
            else
            {
                instructions.emplace_back(mos6502_opcode::inc, o1);
            }
            break;
        case i386_opcode::jne:
            instructions.emplace_back(mos6502_opcode::bne, o1);
            break;
        case i386_opcode::je:
            instructions.emplace_back(mos6502_opcode::beq, o1);
            break;
        case i386_opcode::js:
            instructions.emplace_back(mos6502_opcode::bmi, o1);
            break;
        case i386_opcode::jmp:
            instructions.emplace_back(mos6502_opcode::jmp, o1);
            break;
        case i386_opcode::addb:
            if (o1.type == operand_type::literal && o2.type == operand_type::reg)
            {
                instructions.emplace_back(mos6502_opcode::lda, get_register(o2.reg_num));
                instructions.emplace_back(mos6502_opcode::clc);
                instructions.emplace_back(mos6502_opcode::adc, operand(o1.type, fixup_8bit_literal(o1.value)));
                instructions.emplace_back(mos6502_opcode::sta, get_register(o2.reg_num));
            }
            else if (o1.type == operand_type::literal && o2.type == operand_type::literal)
            {
                instructions.emplace_back(mos6502_opcode::lda, operand(o1.type, fixup_8bit_literal(o1.value)));
                instructions.emplace_back(mos6502_opcode::clc);
                instructions.emplace_back(mos6502_opcode::adc, o2);
                instructions.emplace_back(mos6502_opcode::sta, o2);
            }
            else if (o1.type == operand_type::reg && o2.type == operand_type::literal)
            {
                instructions.emplace_back(mos6502_opcode::lda, get_register(o1.reg_num));
                instructions.emplace_back(mos6502_opcode::clc);
                instructions.emplace_back(mos6502_opcode::adc, o2);
                instructions.emplace_back(mos6502_opcode::sta, o2);
            }
            else
            {
                throw std::runtime_error("Cannot translate addb instruction");
            }
            break;
        case i386_opcode::cmpb:
            if (o1.type == operand_type::literal && o2.type == operand_type::literal)
            {
                instructions.emplace_back(mos6502_opcode::lda, o2);
                instructions.emplace_back(mos6502_opcode::cmp, operand(o1.type, fixup_8bit_literal(o1.value)));
            }
            else if (o1.type == operand_type::literal && o2.type == operand_type::reg)
            {
                instructions.emplace_back(mos6502_opcode::lda, get_register(o2.reg_num));
                instructions.emplace_back(mos6502_opcode::cmp, operand(o1.type, fixup_8bit_literal(o1.value)));
            }
            else
            {
                throw std::runtime_error("Cannot translate cmpb instruction");
            }
            break;
        case i386_opcode::andb:
            if (o1.type == operand_type::literal && o2.type == operand_type::reg)
            {
                const auto reg = get_register(o2.reg_num);
                instructions.emplace_back(mos6502_opcode::lda, reg);
                instructions.emplace_back(mos6502_opcode::AND, operand(o1.type, fixup_8bit_literal(o1.value)));
                instructions.emplace_back(mos6502_opcode::sta, reg);
            }
            else if (o1.type == operand_type::literal && o2.type == operand_type::literal)
            {
                const auto reg = get_register(o2.reg_num);
                instructions.emplace_back(mos6502_opcode::lda, o2);
                instructions.emplace_back(mos6502_opcode::AND, operand(o1.type, fixup_8bit_literal(o1.value)));
                instructions.emplace_back(mos6502_opcode::sta, o2);
            }
            else
            {
                throw std::runtime_error("Cannot translate andb instruction");
            }
            break;
        case i386_opcode::negb:
            if (o1.type == operand_type::reg)
            {
                // perform a two's complement of the register location
                instructions.emplace_back(mos6502_opcode::lda, get_register(o1.reg_num));
                instructions.emplace_back(mos6502_opcode::eor, operand(operand_type::literal, "#$ff"));
                instructions.emplace_back(mos6502_opcode::sta, get_register(o1.reg_num));
                instructions.emplace_back(mos6502_opcode::inc, get_register(o1.reg_num));
            }
            else
            {
                throw std::runtime_error("Cannot translate negb instruction");
            }
            break;
        case i386_opcode::notb:
            if (o1.type == operand_type::reg)
            {
                // exclusive or against 0xff to perform a logical not
                instructions.emplace_back(mos6502_opcode::lda, get_register(o1.reg_num));
                instructions.emplace_back(mos6502_opcode::eor, operand(operand_type::literal, "#$ff"));
                instructions.emplace_back(mos6502_opcode::sta, get_register(o1.reg_num));
            }
            else
            {
                throw std::runtime_error("Cannot translate notb instruction");
            }
            break;
        case i386_opcode::subb:
            // DEST <- DEST - SRC
            // o2 = o2 - o1
            // Ensure that we set the carry flag before performing the subtraction
            if (o1.type == operand_type::reg && o2.type == operand_type::literal)
            {
                instructions.emplace_back(mos6502_opcode::lda, o2);
                instructions.emplace_back(mos6502_opcode::sec);
                instructions.emplace_back(mos6502_opcode::sbc, get_register(o1.reg_num));
                instructions.emplace_back(mos6502_opcode::sta, o2);
            }
            else
            {
                throw std::runtime_error("Cannot translate subb instruction");
            }
            break;
        case i386_opcode::pushl:
            if (o1.type == operand_type::reg)
            {
                instructions.emplace_back(mos6502_opcode::lda, get_register(o1.reg_num));
                instructions.emplace_back(mos6502_opcode::pha);
                instructions.emplace_back(mos6502_opcode::lda, get_register(o1.reg_num, 1));
                instructions.emplace_back(mos6502_opcode::pha);
            }
            else if (o1.type == operand_type::literal)
            {
                instructions.emplace_back(mos6502_opcode::lda, o1);
                instructions.emplace_back(mos6502_opcode::pha);
            }
            else
            {
                throw std::runtime_error("Cannot translate pushl instruction");
            }
            break;

        case i386_opcode::sbbb:
            // DEST <- (DEST – (SRC + CF))
            // o2 <- (o2 - (o1 + cf))
            // if o1 and o2 are the same we get
            // o2 <- (o2 - (o2 + cf))
            // o2 <- -cf
            if (o1.type == operand_type::reg && o2.type == operand_type::reg && o1.reg_num == o2.reg_num)
            {
                instructions.emplace_back(mos6502_opcode::lda, operand(operand_type::literal, "#$00")); // reset a
                instructions.emplace_back(mos6502_opcode::sbc,
                                          operand(operand_type::literal, "#$00")); // subtract out the carry flag
                instructions.emplace_back(mos6502_opcode::eor,
                                          operand(operand_type::literal, "#$ff"));        // invert the bits
                instructions.emplace_back(mos6502_opcode::sta, get_register(o2.reg_num)); // place the value
            }
            else
            {
                throw std::runtime_error("Cannot translate sbbb instruction");
            }
            break;
        case i386_opcode::calll:
            if (o1.type == operand_type::literal)
            {
                instructions.emplace_back(mos6502_opcode::jsr, o1);
            }
            else
            {
                throw std::runtime_error("Cannot translate calll instruction");
            }
            break;

        default:
            throw std::runtime_error("Cannot translate unhandled instruction");
    }
}

enum class LogLevel
{
    Warning,
    Error
};

std::string to_string(const LogLevel ll)
{
    switch (ll)
    {
        case LogLevel::Warning:
            return "warning";
        case LogLevel::Error:
            return "error";
    }
    return "unknown";
}

void log(LogLevel ll, const i386 &i, const std::string &message)
{
    std::cerr << to_string(ll) << ": " << i.line_num << ": " << message << ": `" << i.line_text << "`\n";
}

void log(LogLevel ll, const int line_no, const std::string &line, const std::string &message)
{
    std::cerr << to_string(ll) << ": " << line_no << ": " << message << ": `" << line << "`\n";
}

void to_mos6502(const i386 &i, std::vector<mos6502> &instructions)
{
    try
    {
        switch (i.type)
        {
            case asm_line::line_type::Label:
                instructions.emplace_back(i.type, i.text);
                return;
            case asm_line::line_type::Directive:
                instructions.emplace_back(i.type, i.text);
                return;
            case asm_line::line_type::Instruction:
                //        instructions.emplace_back(ASMLine::Type::Directive, "; " + i.line_text);

                const auto head = instructions.size();

                auto text = i.line_text;
                if (text[0] == '\t')
                {
                    text.erase(0, 1);
                }

                translate_instruction(instructions, text, i.opcode, i.operand1, i.operand2);

                for_each(std::next(instructions.begin(), head), instructions.end(),
                         [text](auto &ins) { ins.comment = text; });
                return;
        }
    }
    catch (const std::exception &e)
    {
        log(LogLevel::Error, i, e.what());
    }
}

bool optimize(std::vector<mos6502> &instructions)
{
    if (instructions.size() < 2)
    {
        return false;
    }

    const auto next_instruction = [&instructions](auto i) {
        do
        {
            ++i;
        } while (i < instructions.size() && instructions[i].type == asm_line::line_type::Directive);
        return i;
    };

    for (size_t op = 0; op < instructions.size() - 1; ++op)
    {
        // look for a transfer of Y -> A immediately followed by A -> Y
        if (instructions[op].opcode == mos6502_opcode::tya)
        {
            next_instruction(op);
            if (instructions[op].opcode == mos6502_opcode::tay)
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
        if (instructions[op].opcode == mos6502_opcode::sta)
        {
            const auto next = next_instruction(op);
            if (instructions[next].opcode == mos6502_opcode::lda && instructions[next].op == instructions[op].op)
            {
                instructions.erase(std::next(std::begin(instructions), next),
                                   std::next(std::begin(instructions), next + 1));
                return true;
            }
        }
    }

    for (size_t op = 0; op < instructions.size() - 1; ++op)
    {
        if (instructions[op].opcode == mos6502_opcode::lda && instructions[op].op.type == operand_type::literal)
        {
            const auto operand = instructions[op].op;
            auto op2 = op + 1;
            // look for multiple stores of the same value
            while (op2 < instructions.size() && (instructions[op2].opcode == mos6502_opcode::sta ||
                                                 instructions[op2].type == asm_line::line_type::Directive))
            {
                ++op2;
            }
            if (instructions[op2].opcode == mos6502_opcode::lda && operand == instructions[op2].op)
            {
                instructions.erase(std::next(std::begin(instructions), op2),
                                   std::next(std::begin(instructions), op2 + 1));
                return true;
            }
        }
    }

    return false;
}

bool fix_long_branches(std::vector<mos6502> &instructions, int &branch_patch_count)
{
    std::map<std::string, size_t> labels;
    for (size_t op = 0; op < instructions.size(); ++op)
    {
        if (instructions[op].type == asm_line::line_type::Label)
        {
            labels[instructions[op].text] = op;
        }
    }

    for (size_t op = 0; op < instructions.size(); ++op)
    {
        if (instructions[op].is_branch &&
            std::abs(static_cast<int>(labels[instructions[op].op.value]) - static_cast<int>(op)) * 3 > 255)
        {
            ++branch_patch_count;
            const auto going_to = instructions[op].op.value;
            const auto new_pos = "patch_" + std::to_string(branch_patch_count);
            // uh-oh too long of a branch, have to convert this to a jump...
            if (instructions[op].opcode == mos6502_opcode::bne)
            {
                const auto comment = instructions[op].comment;
                instructions[op] = mos6502(mos6502_opcode::beq, operand(operand_type::literal, new_pos));
                instructions.insert(std::next(std::begin(instructions), op + 1),
                                    mos6502(mos6502_opcode::jmp, operand(operand_type::literal, going_to)));
                instructions.insert(std::next(std::begin(instructions), op + 2),
                                    mos6502(asm_line::line_type::Label, new_pos));
                instructions[op].comment = instructions[op + 1].comment = instructions[op + 2].comment = comment;
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

bool fix_overwritten_flags(std::vector<mos6502> &instructions)
{
    if (instructions.size() < 3)
    {
        return false;
    }

    for (size_t op = 0; op < instructions.size(); ++op)
    {
        if (instructions[op].is_comparison)
        {
            auto op2 = op + 1;
            while (op2 < instructions.size() && !instructions[op2].is_comparison && !instructions[op2].is_branch)
            {
                ++op2;
            }

            if (op2 < instructions.size() && (op2 - op) > 1 && instructions[op2 - 1].opcode != mos6502_opcode::plp)
            {
                if (instructions[op2].is_comparison)
                {
                    continue;
                }

                if (instructions[op2].is_branch)
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

int main()
{
    std::regex Comment(R"(\s*\#.*)");
    std::regex Label(R"(^(\S+):.*)");
    std::regex Directive(R"(^\t(\..+))");
    std::regex UnaryInstruction(R"(^\t(\S+)\s+(\S+))");
    std::regex BinaryInstruction(R"(^\t(\S+)\s+(\S+),\s+(\S+))");
    std::regex Instruction(R"(^\t(\S+))");

    int lineno = 0;

    std::vector<i386> instructions;

    while (std::cin.good())
    {
        std::string line;
        getline(std::cin, line);

        try
        {
            std::smatch match;
            if (std::regex_match(line, match, Label))
            {
                instructions.emplace_back(lineno, line, asm_line::line_type::Label, match[1]);
            }
            else if (std::regex_match(line, match, Comment))
            {
                // don't care about comments
            }
            else if (std::regex_match(line, match, Directive))
            {
                instructions.emplace_back(lineno, line, asm_line::line_type::Directive, match[1]);
            }
            else if (std::regex_match(line, match, UnaryInstruction))
            {
                instructions.emplace_back(lineno, line, asm_line::line_type::Instruction, match[1], match[2]);
            }
            else if (std::regex_match(line, match, BinaryInstruction))
            {
                instructions.emplace_back(lineno, line, asm_line::line_type::Instruction, match[1], match[2], match[3]);
            }
            else if (std::regex_match(line, match, Instruction))
            {
                instructions.emplace_back(lineno, line, asm_line::line_type::Instruction, match[1]);
            }
            else if (line.empty())
            {
                // std::cout << "EmptyLine\n";
            }
            else
            {
                throw std::runtime_error("Unparsed Input, Line: " + std::to_string(lineno));
            }
        }
        catch (const std::exception &e)
        {
            log(LogLevel::Error, lineno, line, e.what());
        }

        ++lineno;
    }

    std::set<std::string> labels;

    for (const auto &i : instructions)
    {
        if (i.type == asm_line::line_type::Label)
        {
            labels.insert(i.text);
        }
    }

    std::set<std::string> used_labels{"main"};

    for (const auto &i : instructions)
    {
        if (i.type == asm_line::line_type::Instruction)
        {
            if (labels.count(i.operand1.value) != 0)
            {
                used_labels.insert(i.operand1.value);
                used_labels.insert(i.operand2.value);
            }
        }
    }

    // remove all labels and directives that we don't need
    instructions.erase(std::remove_if(std::begin(instructions), std::end(instructions),
                                      [&used_labels](const auto &i) {
                                          if (i.type == asm_line::line_type::Label)
                                          {
                                              if (used_labels.count(i.text) == 0)
                                              {
                                                  // remove all unused labels that aren't 'main'
                                                  return true;
                                              }
                                          }
                                          return false;
                                      }),
                       std::end(instructions));

    const auto new_labels = [&used_labels]() {
        std::map<std::string, std::string> result;
        for (const auto &l : used_labels)
        {
            auto newl = l;
            std::transform(newl.begin(), newl.end(), newl.begin(), [](const auto c) { return std::tolower(c); });
            newl.erase(std::remove_if(newl.begin(), newl.end(), [](const auto c) { return !std::isalnum(c); }),
                       std::end(newl));
            result.emplace(std::make_pair(l, newl));
        }
        return result;
    }();

    for (auto &i : instructions)
    {
        if (i.type == asm_line::line_type::Label)
        {
            i.text = new_labels.at(i.text);
        }

        const auto itr1 = new_labels.find(i.operand1.value);
        if (itr1 != new_labels.end())
        {
            i.operand1.value = itr1->second;
        }

        const auto itr2 = new_labels.find(i.operand2.value);
        if (itr2 != new_labels.end())
        {
            i.operand2.value = itr2->second;
        }
    }

    std::vector<mos6502> new_instructions;

    for (const auto &i : instructions)
    {
        to_mos6502(i, new_instructions);
    }

    while (fix_overwritten_flags(new_instructions))
    {
        // do it however many times it takes
    }

    while (optimize(new_instructions))
    {
        // do it however many times it takes
    }

    int branch_patch_count = 0;
    while (fix_long_branches(new_instructions, branch_patch_count))
    {
        // do it however many times it takes
    }

    for (const auto &i : new_instructions)
    {
        std::cout << i.to_string() << '\n';
    }
}
