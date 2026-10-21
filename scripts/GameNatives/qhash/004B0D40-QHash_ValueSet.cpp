// IGI native QHash_ValueSet - 0x004B0D40 - transcribed from retail igi.exe.
// Retail: cdecl, no arguments. Sets the byte at 0x009422FD to 1.
#include <cstdint>

extern std::uint8_t g_009422fd;   // 0x009422FD

void __cdecl QHash_ValueSet(void)
{
    g_009422fd = 1;
}
