// IGI native Matrix_InvalidateSlot - 0x004B7450 - transcribed from retail igi.exe.
// Retail: cdecl (context, slot). Stores 0 at context+0x14+slot*4.
#include <cstdint>
#include <cstring>

void __cdecl Matrix_InvalidateSlot(void* context, std::int32_t slot)
{
    std::uint8_t* base = reinterpret_cast<std::uint8_t*>(context);
    std::uint32_t zero = 0;
    std::memcpy(base + 0x14 + static_cast<std::intptr_t>(slot) * 4, &zero, 4);
}
