// IGI native Ilff_GetFormType - 0x004B7570 - transcribed from retail igi.exe.
// Retail: cdecl (container). Returns the 32-bit value at container+0x10, 0 for null.
#include <cstdint>
#include <cstring>
static inline std::uint32_t rd32(const void* p, std::intptr_t off) { std::uint32_t v; std::memcpy(&v, reinterpret_cast<const std::uint8_t*>(p) + off, 4); return v; }

std::uint32_t __cdecl Ilff_GetFormType(void* container)
{
    if (container == nullptr) return 0;
    return rd32(container, 0x10);
}
