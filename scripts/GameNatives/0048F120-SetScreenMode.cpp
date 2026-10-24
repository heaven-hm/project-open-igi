// IGI native FX_FireBallUpdate_23 - 0x0048F120 - transcribed from retail igi.exe.
// Retail: movsx eax,byte [esp+4]; mov [g_5c8c00],eax; ret
#include <cstdint>
#include <windows.h>
extern std::int32_t g_5c8c00;

int __cdecl FX_FireBallUpdate_23(int context)
{
    int value = (signed char)context;
    g_5c8c00 = value;
    return value;
}
