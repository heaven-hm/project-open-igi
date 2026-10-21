// IGI native AppContext_SetDebugged - 0x0048F1A0 - transcribed from retail igi.exe.
// Retail: movsx eax,byte [esp+4]; mov [g_5c8bf8],eax; ret
#include <cstdint>
#include <windows.h>
extern std::int32_t g_5c8bf8;

int __cdecl AppContext_SetDebugged(int context)
{
    int value = (signed char)context;
    g_5c8bf8 = value;
    return value;
}
