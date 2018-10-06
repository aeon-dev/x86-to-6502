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
                case asm_line::line_type::label:
                {
                    target.translate_label(i.text());
                    break;
                }
                case asm_line::line_type::directive:
                {
                    target.translate_directive(i.text());
                    break;
                }
                case asm_line::line_type::instruction:
                {
                    target.set_current_text(i.line_text_unindented());
                    target.translate(i);
                    break;
                }
                case asm_line::line_type::missing_opcode:
                {
                    std::cerr << "Missing opcode: " << i.text() << '\n';
                    break;
                }
                case asm_line::line_type::empty:
                case asm_line::line_type::comment:
                default:
                    break;
            }
        }
        catch (const std::exception &e)
        {
            log(log_level::error, i, e.what());
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
