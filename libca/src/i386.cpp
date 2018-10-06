#include <ca/i386.h>
#include <ca/instruction_operand.h>
#include "logger.h"
#include <regex>
#include <iostream>
#include <set>
#include <map>
#include <cctype>

namespace ca
{

namespace internal
{

// clang-format off
static std::map<std::string, i386_opcode> opcode_string_lookup{
    {"addb",   i386_opcode::addb},
    {"addl",   i386_opcode::addl},
    {"andb",   i386_opcode::andb},
    {"andl",   i386_opcode::andl},
    {"calll",  i386_opcode::calll},
    {"cmpb",   i386_opcode::cmpb},
    {"decb",   i386_opcode::decb},
    {"decl",   i386_opcode::decl},
    {"incb",   i386_opcode::incb},
    {"incl",   i386_opcode::incl},
    {"je",     i386_opcode::je},
    {"jmp",    i386_opcode::jmp},
    {"jne",    i386_opcode::jne},
    {"js",     i386_opcode::js},
    {"movb",   i386_opcode::movb},
    {"movl",   i386_opcode::movl},
    {"movzbl", i386_opcode::movzbl},
    {"movzwl", i386_opcode::movzwl},
    {"negb",   i386_opcode::negb},
    {"notb",   i386_opcode::notb},
    {"orb",    i386_opcode::orb},
    {"orl",    i386_opcode::orl},
    {"pushl",  i386_opcode::pushl},
    {"rep",    i386_opcode::rep},
    {"ret",    i386_opcode::ret},
    {"retl",   i386_opcode::retl},
    {"sall",   i386_opcode::sall},
    {"sarl",   i386_opcode::sarl},
    {"sbbb",   i386_opcode::sbbb},
    {"shrb",   i386_opcode::shrb},
    {"shrl",   i386_opcode::shrl},
    {"subb",   i386_opcode::subb},
    {"subl",   i386_opcode::subl},
    {"testb",  i386_opcode::testb},
    {"xorl",   i386_opcode::xorl}
};

static std::map<std::string, i386_register> register_name_lookup{
    {"%al",  i386_register::al },
    {"%ah",  i386_register::ah },
    {"%ax",  i386_register::ax },
    {"%eax", i386_register::eax},
    {"%bl",  i386_register::bl },
    {"%bh",  i386_register::bh },
    {"%bx",  i386_register::bx },
    {"%ebx", i386_register::ebx},
    {"%cl",  i386_register::cl },
    {"%ch",  i386_register::ch },
    {"%cx",  i386_register::cx },
    {"%ecx", i386_register::ecx},
    {"%dl",  i386_register::dl },
    {"%dh",  i386_register::dh },
    {"%dx",  i386_register::dx },
    {"%edx", i386_register::edx},
    {"%sil", i386_register::sil},
    {"%si",  i386_register::si },
    {"%esi", i386_register::esi},
    {"%dil", i386_register::dil},
    {"%di",  i386_register::di },
    {"%edi", i386_register::edi},
    {"%bpl", i386_register::bpl},
    {"%bp",  i386_register::bp },
    {"%ebp", i386_register::ebp},
    {"%spl", i386_register::spl},
    {"%sp",  i386_register::sp },
    {"%esp", i386_register::esp}
};
// clang-format on

static auto parse_opcode(asm_line::line_type t, const std::string &o) -> i386_opcode
{
    switch (t)
    {
        case asm_line::line_type::label:
            return i386_opcode::unknown;
        case asm_line::line_type::directive:
            return i386_opcode::unknown;
        case asm_line::line_type::instruction:
        {
            const auto result = opcode_string_lookup.find(o);
            if (result != std::end(opcode_string_lookup))
                return result->second;

            return i386_opcode::unknown;
        }
        default:
            break;
    }

    throw std::runtime_error("Unknown opcode: " + o);
}

static auto parse_operand(std::string o) -> instruction_operand
{
    if (o.empty())
        return {};

    if (o[0] != '%')
        return {operand_type::literal, std::move(o)};

    const auto result = register_name_lookup.find(o);
    if (result != std::end(register_name_lookup))
        return result->second;

    throw std::runtime_error("Unknown register operand: '" + o + "'");
}

} // namespace internal

i386::i386(const int line_number)
    : asm_line{}
    , line_number_{line_number}
    , line_text_{}
    , opcode_{}
    , operand1_{}
    , operand2_{}
{
}

i386::i386(const int line_number, std::string line_text)
    : asm_line{std::move(line_text)}
    , line_number_{line_number}
    , line_text_{}
    , opcode_{}
    , operand1_{}
    , operand2_{}
{
}

i386::i386(const int line_number, std::string line_text, line_type type, std::string opcode, std::string operand1,
           std::string operand2)
    : asm_line{type, opcode}
    , line_number_{line_number}
    , line_text_{std::move(line_text)}
    , opcode_{internal::parse_opcode(type, opcode)}
    , operand1_{internal::parse_operand(operand1)}
    , operand2_{internal::parse_operand(operand2)}
{
}

// TODO: This should be refactored into smaller methods.
auto i386::parse() -> std::vector<i386>
{
    int lineno = 0;

    std::vector<ca::i386> instructions;

    while (std::cin.good())
    {
        std::string line;
        getline(std::cin, line);

        try
        {
            auto instruction = ca::i386::parse(line, lineno);

            if (!instruction.is_empty() && !instruction.is_comment())
                instructions.emplace_back(std::move(instruction));
        }
        catch (const std::exception &e)
        {
            log(log_level::error, lineno, line, e.what());
        }

        ++lineno;
    }

    std::set<std::string> labels;

    for (const auto &i : instructions)
    {
        if (i.is_label())
        {
            labels.insert(i.text());
        }
    }

    std::set<std::string> used_labels{"main"};

    for (const auto &i : instructions)
    {
        if (i.is_instruction())
        {
            if (labels.count(i.operand1().value()) != 0)
            {
                used_labels.insert(i.operand1().value());
                used_labels.insert(i.operand2().value());
            }
        }
    }

    // remove all labels and directives that we don't need
    instructions.erase(std::remove_if(std::begin(instructions), std::end(instructions),
                                      [&used_labels](const auto &i) {
                                          if (i.is_label())
                                          {
                                              if (used_labels.count(i.text()) == 0)
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
        if (i.is_label())
        {
            i.set_text(new_labels.at(i.text()));
        }

        const auto itr1 = new_labels.find(i.operand1().value());
        if (itr1 != new_labels.end())
        {
            i.operand1().set_value(itr1->second);
        }

        const auto itr2 = new_labels.find(i.operand2().value());
        if (itr2 != new_labels.end())
        {
            i.operand2().set_value(itr2->second);
        }
    }

    return instructions;
}

auto i386::parse(const std::string &line, const int line_number) -> i386
{
    std::regex Comment(R"(\s*\#.*)");
    std::regex Label(R"(^(\S+):.*)");
    std::regex Directive(R"(^\t?(\..+))");
    std::regex UnaryInstruction(R"(^\t(\S+)\s+(\S+)(\s+#.*)?)");
    std::regex BinaryInstruction(R"(^\t(\S+)\s+(\S+),\s+(\S+))");
    std::regex Instruction(R"(^\t(\S+))");

    std::smatch match;
    if (std::regex_match(line, match, Label))
    {
        return {line_number, line, asm_line::line_type::label, match[1]};
    }
    else if (std::regex_match(line, match, Comment))
    {
        return {line_number, line};
    }
    else if (std::regex_match(line, match, Directive))
    {
        return {line_number, line, asm_line::line_type::directive, match[1]};
    }
    else if (std::regex_match(line, match, UnaryInstruction))
    {
        return {line_number, line, asm_line::line_type::instruction, match[1], match[2]};
    }
    else if (std::regex_match(line, match, BinaryInstruction))
    {
        return {line_number, line, asm_line::line_type::instruction, match[1], match[2], match[3]};
    }
    else if (std::regex_match(line, match, Instruction))
    {
        return {line_number, line, asm_line::line_type::instruction, match[1]};
    }
    else if (line.empty())
    {
        return i386{line_number};
    }
    else
    {
        throw std::runtime_error("Unparsed Input, Line: " + std::to_string(line_number));
    }
}

} // namespace ca
