#include <ca/cross_assembler.h>
#include <mos6502/mos6502_target.h>

int main(int argc, char *argv[])
{
    mos6502_target target;
    ca::cross_assembler assembler(target);
    assembler.assemble();

    return 0;
}
