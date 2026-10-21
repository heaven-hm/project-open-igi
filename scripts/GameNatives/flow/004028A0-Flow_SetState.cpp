// IGI native Flow_SetState - 0x004028A0 - transcribed from retail igi.exe.
// Retail: cdecl (state). Stores the pending flow state dword into game context + 0x2C.
#include <cstdint>
#include <cstring>

extern std::uint8_t* g_567c8c;

void __cdecl Flow_SetState(int state)
{
    *reinterpret_cast<int*>(g_567c8c + 0x2C) = state;
}
