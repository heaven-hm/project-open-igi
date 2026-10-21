// IGI native Human_TaskViewReset - 0x004659E0 - transcribed from retail igi.exe.
// Retail: cdecl, one argument (human). Stores tan(g_5335e8) as a float at human+0x1e4, zero at human+0x1ec and tan(g_5339c0) as a float at human+0x1e8.
#include <cstdint>
#include <cstring>
#include <cmath>

extern double g_5335e8;
extern double g_5339c0;

void __cdecl Human_TaskViewReset(void* human)
{
    std::uint8_t* base = reinterpret_cast<std::uint8_t*>(human);
    float viewX = static_cast<float>(std::tan(g_5335e8));
    *reinterpret_cast<std::int32_t*>(base + 0x1ec) = 0;
    std::memcpy(base + 0x1e4, &viewX, 4);
    float viewY = static_cast<float>(std::tan(g_5339c0));
    std::memcpy(base + 0x1e8, &viewY, 4);
}
