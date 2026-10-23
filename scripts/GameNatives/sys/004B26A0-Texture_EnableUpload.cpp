// IGI native Texture_EnableUpload - 0x004B26A0 - transcribed from retail igi.exe.
// Retail: cdecl, no arguments. Sets the byte g_943b3c to 1.
#include <cstdint>
#include <cstring>

extern std::uint8_t g_943b3c;

void __cdecl Texture_EnableUpload(void)
{
    g_943b3c = 1;
}
