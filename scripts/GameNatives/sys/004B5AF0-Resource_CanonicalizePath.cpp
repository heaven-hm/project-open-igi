// IGI native Resource_CanonicalizePath - 0x004B5AF0 - transcribed from retail igi.exe.
// Retail: cdecl (dst, src). Copies src into dst converting each byte through String_ToUpperChar (0x004AD73D), then terminates dst.
#include <cstdint>

int __cdecl String_ToUpperChar(int ch); // 0x004AD73D

void __cdecl Resource_CanonicalizePath(char* dst, const char* src)
{
    char* esi;
    const char* edi = src;
    char al = *edi;
    if (al == 0) {
        *dst = 0;
        return;
    }
    esi = dst;
    do {
        int r = String_ToUpperChar(static_cast<int>(static_cast<signed char>(al)));
        *esi = static_cast<char>(r);
        al = edi[1];
        esi++;
        edi++;
    } while (al != 0);
    *esi = al;
}
