// IGI native AppContext_SetDebugtextState - 0x0048F1E0 - transcribed from retail igi.exe.
// Retail: movsx eax,byte [esp+4]; mov [g_5c8bf4],eax; ret
#include <cstdint>
#include <windows.h>
extern std::int32_t g_5c8bf4;

int __cdecl AppContext_SetDebugtextState(int context)
{
    int value = (signed char)context;
    g_5c8bf4 = value;
    return value;
}
