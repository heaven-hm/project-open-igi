// IGI native Human_IsCrouched - 0x00463180 - transcribed from retail igi.exe.
// Retail: cdecl, one argument (human). Returns the byte at human+0x33d.
#include <cstdint>
#include <cstring>

std::uint8_t __cdecl Human_IsCrouched(void* human)
{
    return *(reinterpret_cast<std::uint8_t*>(human) + 0x33d);
}
