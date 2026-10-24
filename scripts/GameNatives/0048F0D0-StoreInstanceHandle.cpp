// IGI native FX_SmokeEmit_22 - 0x0048F0D0 - transcribed from retail igi.exe.
// Retail: mov eax,[esp+4]; mov [g_5c8bc0],eax; ret
#include <cstdint>
#include <windows.h>
extern std::int32_t g_5c8bc0;

int __cdecl FX_SmokeEmit_22(int context)
{
    g_5c8bc0 = context;
    return context;
}
