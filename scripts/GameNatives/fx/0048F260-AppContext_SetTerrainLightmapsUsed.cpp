// IGI native AppContext_SetTerrainLightmapsUsed - 0x0048F260 - transcribed from retail igi.exe.
// Retail: cdecl (byte). Sign-extends the byte argument and stores it as a dword at 0x5C8BE8.
#include <cstdint>
#include <cstring>

extern std::int32_t g_5c8be8;

void __cdecl AppContext_SetTerrainLightmapsUsed(std::int8_t value)
{
    g_5c8be8 = static_cast<std::int32_t>(value);
}
