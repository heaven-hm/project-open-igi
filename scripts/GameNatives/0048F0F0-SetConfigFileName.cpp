// IGI native FX_TrailUpdate_22 - 0x0048F0F0 - transcribed from retail igi.exe.
// Retail: mov eax,[esp+4]; mov [g_5c8bcc],eax; ret
#include <cstdint>
#include <windows.h>
extern std::int32_t g_5c8bcc;

int __cdecl FX_TrailUpdate_22(int context)
{
    g_5c8bcc = context;
    return context;
}
