// IGI native Display_ClearSurfaces_16 - 0x00498460 - transcribed from retail igi.exe.
// Retail: cdecl, no arguments. Saves the x87 control word at 0x6E5BC4, sets rounding control bits 0xC00 (truncate) and reloads it.
#include <cstdint>
#include <cstring>

extern std::uint16_t g_6e5bc4;

void __cdecl Display_ClearSurfaces_16(void)
{
    __asm__ __volatile__("fwait\n\tfnstcw %0" : "=m"(g_6e5bc4));
    g_6e5bc4 = static_cast<std::uint16_t>(g_6e5bc4 | 0xC00);
    __asm__ __volatile__("fldcw %0" : : "m"(g_6e5bc4));
}
