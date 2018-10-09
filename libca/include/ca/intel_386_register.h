#pragma once

namespace ca
{

/*
 32 bits | Lower 16 bits | Higher 8 bits | Lower 8 bits
========================================================
 eax     | ax            | ah            | al
 ebx     | bx            | bh            | bl
 ecx     | cx            | ch            | cl
 edx     | dx            | dh            | dl
 esi     | si            |               | sil
 edi     | di            |               | dil
 ebp     | bp            |               | bpl
 esp     | sp            |               | spl
*/

enum class intel_386_register
{
    unknown,
    al,
    ah,
    ax,
    eax,
    bl,
    bh,
    bx,
    ebx,
    cl,
    ch,
    cx,
    ecx,
    dl,
    dh,
    dx,
    edx,
    sil,
    si,
    esi,
    dil,
    di,
    edi,
    bpl,
    bp,
    ebp,
    spl,
    sp,
    esp
};

} // namespace ca
