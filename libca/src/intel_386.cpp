#include <ca/intel_386.h>
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
static std::map<std::string, intel_386_opcode> opcode_string_lookup{
    {"addb",   intel_386_opcode::addb},
    {"addl",   intel_386_opcode::addl},
    {"andb",   intel_386_opcode::andb},
    {"andl",   intel_386_opcode::andl},
    {"calll",  intel_386_opcode::calll},
    {"cmpb",   intel_386_opcode::cmpb},
    {"decb",   intel_386_opcode::decb},
    {"decl",   intel_386_opcode::decl},
    {"incb",   intel_386_opcode::incb},
    {"incl",   intel_386_opcode::incl},
    {"je",     intel_386_opcode::je},
    {"jmp",    intel_386_opcode::jmp},
    {"jne",    intel_386_opcode::jne},
    {"js",     intel_386_opcode::js},
    {"movb",   intel_386_opcode::movb},
    {"movl",   intel_386_opcode::movl},
    {"movzbl", intel_386_opcode::movzbl},
    {"movzwl", intel_386_opcode::movzwl},
    {"negb",   intel_386_opcode::negb},
    {"notb",   intel_386_opcode::notb},
    {"orb",    intel_386_opcode::orb},
    {"orl",    intel_386_opcode::orl},
    {"pushl",  intel_386_opcode::pushl},
    {"rep",    intel_386_opcode::rep},
    {"ret",    intel_386_opcode::ret},
    {"retl",   intel_386_opcode::retl},
    {"sall",   intel_386_opcode::sall},
    {"sarl",   intel_386_opcode::sarl},
    {"sbbb",   intel_386_opcode::sbbb},
    {"shrb",   intel_386_opcode::shrb},
    {"shrl",   intel_386_opcode::shrl},
    {"subb",   intel_386_opcode::subb},
    {"subl",   intel_386_opcode::subl},
    {"testb",  intel_386_opcode::testb},
    {"xorl",   intel_386_opcode::xorl}
};

static std::map<std::string, intel_386_register> register_name_lookup{
    {"%al",  intel_386_register::al },
    {"%ah",  intel_386_register::ah },
    {"%ax",  intel_386_register::ax },
    {"%eax", intel_386_register::eax},
    {"%bl",  intel_386_register::bl },
    {"%bh",  intel_386_register::bh },
    {"%bx",  intel_386_register::bx },
    {"%ebx", intel_386_register::ebx},
    {"%cl",  intel_386_register::cl },
    {"%ch",  intel_386_register::ch },
    {"%cx",  intel_386_register::cx },
    {"%ecx", intel_386_register::ecx},
    {"%dl",  intel_386_register::dl },
    {"%dh",  intel_386_register::dh },
    {"%dx",  intel_386_register::dx },
    {"%edx", intel_386_register::edx},
    {"%sil", intel_386_register::sil},
    {"%si",  intel_386_register::si },
    {"%esi", intel_386_register::esi},
    {"%dil", intel_386_register::dil},
    {"%di",  intel_386_register::di },
    {"%edi", intel_386_register::edi},
    {"%bpl", intel_386_register::bpl},
    {"%bp",  intel_386_register::bp },
    {"%ebp", intel_386_register::ebp},
    {"%spl", intel_386_register::spl},
    {"%sp",  intel_386_register::sp },
    {"%esp", intel_386_register::esp}
};
// clang-format on

static auto parse_opcode(asm_line::line_type t, const std::string &o) -> intel_386_opcode
{
    switch (t)
    {
        case asm_line::line_type::label:
            return intel_386_opcode::unknown;
        case asm_line::line_type::directive:
            return intel_386_opcode::unknown;
        case asm_line::line_type::instruction:
        {
            const auto result = opcode_string_lookup.find(o);
            if (result != std::end(opcode_string_lookup))
                return result->second;

            return intel_386_opcode::unknown;
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

intel_386::intel_386(const int line_number)
    : asm_line{}
    , line_number_{line_number}
    , line_text_{}
    , opcode_{}
    , operand1_{}
    , operand2_{}
{
}

intel_386::intel_386(const int line_number, std::string line_text)
    : asm_line{std::move(line_text)}
    , line_number_{line_number}
    , line_text_{}
    , opcode_{}
    , operand1_{}
    , operand2_{}
{
}

intel_386::intel_386(const int line_number, std::string line_text, line_type type, std::string opcode,
                     std::string operand1, std::string operand2)
    : asm_line{type, opcode}
    , line_number_{line_number}
    , line_text_{std::move(line_text)}
    , opcode_{internal::parse_opcode(type, opcode)}
    , operand1_{internal::parse_operand(operand1)}
    , operand2_{internal::parse_operand(operand2)}
{
}

// TODO: This should be refactored into smaller methods.
auto intel_386::parse() -> std::vector<intel_386>
{
    int lineno = 0;

    std::vector<intel_386> instructions;

    while (std::cin.good())
    {
        std::string line;
        getline(std::cin, line);

        try
        {
            auto instruction = parse(line, lineno);

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

auto intel_386::parse(const std::string &line, const int line_number) -> intel_386
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
        return intel_386{line_number};
    }
    else
    {
        throw std::runtime_error("Unparsed Input, Line: " + std::to_string(line_number));
    }
}

} // namespace ca
