// IGI native FX_FragmentSimulate_22 - 0x0048F0C0 - transcribed from retail igi.exe.
// Retail: mov eax,[esp+4]; mov [g_5c8bc8],eax; ret
#include <cstdint>
#include <windows.h>
extern std::int32_t g_5c8bc8;

int __cdecl FX_FragmentSimulate_22(int context)
{
    g_5c8bc8 = context;
    return context;
}
