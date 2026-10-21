// IGI native FX_FireBallUpdate_24 - 0x0048F1C0 - transcribed from retail igi.exe.
// Retail: movsx eax,byte [esp+4]; mov [g_5c8bec],eax; ret
#include <cstdint>
#include <windows.h>
extern std::int32_t g_5c8bec;

int __cdecl FX_FireBallUpdate_24(int context)
{
    int value = (signed char)context;
    g_5c8bec = value;
    return value;
}
