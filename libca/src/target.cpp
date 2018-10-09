#include <ca/target.h>
#include <map>
#include <iostream>
#include <functional>

namespace ca
{

using opcode_translate_func = void (target::*)(const instruction_operand &, const instruction_operand &);

// clang-format off
static std::map<intel_386_opcode, opcode_translate_func> opcode_translation_lookup = {
    {intel_386_opcode::movzbl, &target::translate_movzbl},
    {intel_386_opcode::movzwl, &target::translate_movzwl},
    {intel_386_opcode::shrb,   &target::translate_shrb},
    {intel_386_opcode::shrl,   &target::translate_shrl},
    {intel_386_opcode::xorl,   &target::translate_xorl},
    {intel_386_opcode::andl,   &target::translate_andl},
    {intel_386_opcode::andb,   &target::translate_andb},
    {intel_386_opcode::addb,   &target::translate_addb},
    {intel_386_opcode::ret,    &target::translate_ret},
    {intel_386_opcode::movb,   &target::translate_movb},
    {intel_386_opcode::cmpb,   &target::translate_cmpb},
    {intel_386_opcode::movl,   &target::translate_movl},
    {intel_386_opcode::jmp,    &target::translate_jmp},
    {intel_386_opcode::jne,    &target::translate_jne},
    {intel_386_opcode::je,     &target::translate_je},
    {intel_386_opcode::js,     &target::translate_js},
    {intel_386_opcode::testb,  &target::translate_testb},
    {intel_386_opcode::incl,   &target::translate_incl},
    {intel_386_opcode::incb,   &target::translate_incb},
    {intel_386_opcode::decl,   &target::translate_decl},
    {intel_386_opcode::decb,   &target::translate_decb},
    {intel_386_opcode::sarl,   &target::translate_sarl},
    {intel_386_opcode::addl,   &target::translate_addl},
    {intel_386_opcode::subl,   &target::translate_subl},
    {intel_386_opcode::subb,   &target::translate_subb},
    {intel_386_opcode::sall,   &target::translate_sall},
    {intel_386_opcode::orl,    &target::translate_orl},
    {intel_386_opcode::orb,    &target::translate_orb},
    {intel_386_opcode::rep,    &target::translate_rep},
    {intel_386_opcode::pushl,  &target::translate_pushl},
    {intel_386_opcode::sbbb,   &target::translate_sbbb},
    {intel_386_opcode::negb,   &target::translate_negb},
    {intel_386_opcode::notb,   &target::translate_notb},
    {intel_386_opcode::retl,   &target::translate_retl},
    {intel_386_opcode::calll,  &target::translate_calll}
};
// clang-format on

void target::translate(const intel_386 &opcode)
{
    if (opcode.opcode() == intel_386_opcode::unknown)
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
