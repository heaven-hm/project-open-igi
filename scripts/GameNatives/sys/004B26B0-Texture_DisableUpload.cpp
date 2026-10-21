// IGI native Texture_DisableUpload - 0x004B26B0 - transcribed from retail igi.exe.
// Retail: cdecl, no arguments. Sets the byte g_943b3c to 0.
#include <cstdint>
#include <cstring>

extern std::uint8_t g_943b3c;

void __cdecl Texture_DisableUpload(void)
{
    g_943b3c = 0;
}
