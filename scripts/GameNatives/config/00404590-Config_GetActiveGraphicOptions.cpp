// IGI native Config_GetActiveGraphicOptions - 0x00404590 - transcribed from retail igi.exe.
// Retail: cdecl, no arguments. Returns 0xBC2394 + g_bc2384 * 1380.
#include <cstdint>
#include <cstring>

extern std::int32_t g_bc2384;
extern std::uint8_t g_bc2394[];

void* __cdecl Config_GetActiveGraphicOptions(void)
{
    return g_bc2394 + g_bc2384 * 1380;
}
