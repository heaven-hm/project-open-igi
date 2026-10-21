// IGI native Ilff_ReadContainerHeader - 0x004B7540 - transcribed from retail igi.exe.
// Retail: cdecl (outSize, outAlign, container). Writes container+4 to *outSize and container+8 to *outAlign when each pointer is non-null.
#include <cstdint>
#include <cstring>
static inline std::uint32_t rd32(const void* p, std::intptr_t off) { std::uint32_t v; std::memcpy(&v, reinterpret_cast<const std::uint8_t*>(p) + off, 4); return v; }

void __cdecl Ilff_ReadContainerHeader(std::uint32_t* outSize, std::uint32_t* outAlign, void* container)
{
    if (container == nullptr) return;
    if (outSize != nullptr) *outSize = rd32(container, 4);
    if (outAlign != nullptr) *outAlign = rd32(container, 8);
}
