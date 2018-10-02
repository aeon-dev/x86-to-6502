#include <ca/cross_assembler.h>
#include <ca/i386.h>
#include "logger.h"
#include <iostream>

namespace ca
{

namespace internal
{

static void translate_instructions(const std::vector<i386> &instructions, target &target)
{
    for (const auto &i : instructions)
    {
        try
        {
            switch (i.type())
            {
                case asm_line::line_type::Label:
                {
                    target.translate_label(i.text());
                    break;
                }
                case asm_line::line_type::Directive:
                {
                    target.translate_directive(i.text());
                    break;
                }
                case asm_line::line_type::Instruction:
                {
                    target.set_current_text(i.line_text_unindented());
                    target.translate(i);
                    break;
                }
                case asm_line::line_type::MissingOpcode:
                {
                    std::cerr << "Missing opcode: " << i.text() << '\n';
                    break;
                }
                case asm_line::line_type::Empty:
                case asm_line::line_type::Comment:
                default:
                    break;
            }
        }
        catch (const std::exception &e)
        {
            log(LogLevel::Error, i, e.what());
        }
    }
}

} // namespace internal

cross_assembler::cross_assembler(target &t)
    : target_{t}
{
}

void cross_assembler::assemble()
{
    auto instructions = i386::parse();
    internal::translate_instructions(instructions, target_);
    target_.finalize();
}

} // namespace ca
