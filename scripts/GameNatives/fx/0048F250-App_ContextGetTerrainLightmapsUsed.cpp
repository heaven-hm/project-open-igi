// IGI native App_ContextGetTerrainLightmapsUsed - 0x0048F250 - transcribed from retail igi.exe.
// Retail: cdecl, no arguments. Returns the terrain-lightmaps-used flag dword at 0x5C8BE8.
#include <cstdint>
#include <cstring>

extern std::int32_t g_5c8be8;

int __cdecl App_ContextGetTerrainLightmapsUsed(void)
{
    return g_5c8be8;
}
