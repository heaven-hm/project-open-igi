// IGI native Game_GetContext - 0x00402880 - transcribed from retail igi.exe.
// Retail: cdecl, no arguments. Returns the game context pointer stored at 0x567C8C.
#include <cstdint>
#include <cstring>

extern void* g_567c8c;

void* __cdecl Game_GetContext(void)
{
    return g_567c8c;
}
