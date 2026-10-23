// IGI native App_ContextGetLightmapsUsed - 0x0048F230 - transcribed from retail igi.exe.
// Retail: cdecl, no arguments. Returns the lightmaps-used flag dword at 0x5C8BE4.
#include <cstdint>
#include <cstring>

extern std::int32_t g_5c8be4;

int __cdecl App_ContextGetLightmapsUsed(void)
{
    return g_5c8be4;
}
