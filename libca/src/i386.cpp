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
            if (o == "movzwl")
                return i386_opcode::movzwl;
            if (o == "movzbl")
                return i386_opcode::movzbl;
            if (o == "shrb")
                return i386_opcode::shrb;
            if (o == "shrl")
                return i386_opcode::shrl;
            if (o == "xorl")
                return i386_opcode::xorl;
            if (o == "andl")
                return i386_opcode::andl;
            if (o == "ret")
                return i386_opcode::ret;
            if (o == "movb")
                return i386_opcode::movb;
            if (o == "cmpb")
                return i386_opcode::cmpb;
            if (o == "movl")
                return i386_opcode::movl;
            if (o == "jmp")
                return i386_opcode::jmp;
            if (o == "testb")
                return i386_opcode::testb;
            if (o == "incl")
                return i386_opcode::incl;
            if (o == "sarl")
                return i386_opcode::sarl;
            if (o == "decl")
                return i386_opcode::decl;
            if (o == "jne")
                return i386_opcode::jne;
            if (o == "je")
                return i386_opcode::je;
            if (o == "js")
                return i386_opcode::js;
            if (o == "subl")
                return i386_opcode::subl;
            if (o == "subb")
                return i386_opcode::subb;
            if (o == "addl")
                return i386_opcode::addl;
            if (o == "addb")
                return i386_opcode::addb;
            if (o == "sall")
                return i386_opcode::sall;
            if (o == "orl")
                return i386_opcode::orl;
            if (o == "andb")
                return i386_opcode::andb;
            if (o == "orb")
                return i386_opcode::orb;
            if (o == "decb")
                return i386_opcode::decb;
            if (o == "incb")
                return i386_opcode::incb;
            if (o == "rep")
                return i386_opcode::rep;
            if (o == "notb")
                return i386_opcode::notb;
            if (o == "negb")
                return i386_opcode::negb;
            if (o == "sbbb")
                return i386_opcode::sbbb;
            if (o == "pushl")
                return i386_opcode::pushl;
            if (o == "retl")
                return i386_opcode::retl;
            if (o == "calll")
                return i386_opcode::calll;

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

    if (o == "%al")
        return {i386_register::al};
    else if (o == "%ah")
        return {i386_register::ah};
    else if (o == "%ax")
        return {i386_register::ax};
    else if (o == "%eax")
        return {i386_register::eax};
    else if (o == "%bl")
        return {i386_register::bl};
    else if (o == "%bh")
        return {i386_register::bh};
    else if (o == "%bx")
        return {i386_register::bx};
    else if (o == "%ebx")
        return {i386_register::ebx};
    else if (o == "%cl")
        return {i386_register::cl};
    else if (o == "%ch")
        return {i386_register::ch};
    else if (o == "%cx")
        return {i386_register::cx};
    else if (o == "%ecx")
        return {i386_register::ecx};
    else if (o == "%dl")
        return {i386_register::dl};
    else if (o == "%dh")
        return {i386_register::dh};
    else if (o == "%dx")
        return {i386_register::dx};
    else if (o == "%edx")
        return {i386_register::edx};
    else if (o == "%sil")
        return {i386_register::sil};
    else if (o == "%dil")
        return {i386_register::dil};
    else if (o == "%si")
        return {i386_register::si};
    else if (o == "%esi")
        return {i386_register::esi};
    else if (o == "%di")
        return {i386_register::di};
    else if (o == "%edi")
        return {i386_register::edi};
    else if (o == "%sp")
        return {i386_register::sp};
    else if (o == "%esp")
        return {i386_register::esp};

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
