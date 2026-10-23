// IGI native Loop_SetStatusByte - 0x0048F480 - transcribed from retail igi.exe.
// Retail: cdecl (byte). Stores the byte argument at 0x5C8DE8.
#include <cstdint>
#include <cstring>

extern std::int8_t g_5c8de8;

void __cdecl Loop_SetStatusByte(std::int8_t value)
{
    g_5c8de8 = value;
}
