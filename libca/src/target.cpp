#include <ca/target.h>
#include <map>
#include <iostream>
#include <functional>

namespace ca
{

using opcode_translate_func = void (target::*)(const instruction_operand &, const instruction_operand &);

// clang-format off
static std::map<i386_opcode, opcode_translate_func> opcode_translation_lookup = {
    {i386_opcode::movzbl, &target::translate_movzbl},
    {i386_opcode::movzwl, &target::translate_movzwl},
    {i386_opcode::shrb,   &target::translate_shrb},
    {i386_opcode::shrl,   &target::translate_shrl},
    {i386_opcode::xorl,   &target::translate_xorl},
    {i386_opcode::andl,   &target::translate_andl},
    {i386_opcode::andb,   &target::translate_andb},
    {i386_opcode::addb,   &target::translate_addb},
    {i386_opcode::ret,    &target::translate_ret},
    {i386_opcode::movb,   &target::translate_movb},
    {i386_opcode::cmpb,   &target::translate_cmpb},
    {i386_opcode::movl,   &target::translate_movl},
    {i386_opcode::jmp,    &target::translate_jmp},
    {i386_opcode::jne,    &target::translate_jne},
    {i386_opcode::je,     &target::translate_je},
    {i386_opcode::js,     &target::translate_js},
    {i386_opcode::testb,  &target::translate_testb},
    {i386_opcode::incl,   &target::translate_incl},
    {i386_opcode::incb,   &target::translate_incb},
    {i386_opcode::decl,   &target::translate_decl},
    {i386_opcode::decb,   &target::translate_decb},
    {i386_opcode::sarl,   &target::translate_sarl},
    {i386_opcode::addl,   &target::translate_addl},
    {i386_opcode::subl,   &target::translate_subl},
    {i386_opcode::subb,   &target::translate_subb},
    {i386_opcode::sall,   &target::translate_sall},
    {i386_opcode::orl,    &target::translate_orl},
    {i386_opcode::orb,    &target::translate_orb},
    {i386_opcode::rep,    &target::translate_rep},
    {i386_opcode::pushl,  &target::translate_pushl},
    {i386_opcode::sbbb,   &target::translate_sbbb},
    {i386_opcode::negb,   &target::translate_negb},
    {i386_opcode::notb,   &target::translate_notb},
    {i386_opcode::retl,   &target::translate_retl},
    {i386_opcode::calll,  &target::translate_calll}
};
// clang-format on

void target::translate(const i386 &opcode)
{
    if (opcode.opcode() == i386_opcode::unknown)
    {
        translate_unknown(opcode.text());
        return;
    }

    auto result = opcode_translation_lookup.find(opcode.opcode());

    if (result == std::end(opcode_translation_lookup))
    {
        std::cerr << "Missing opcode: " << opcode.text() << '\n';
        return;
    }

    std::invoke(result->second, *this, opcode.operand1(), opcode.operand2());
}

} // namespace ca
