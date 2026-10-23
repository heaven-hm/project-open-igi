// IGI native Ilff_GetChunkTag - 0x004B7580 - transcribed from retail igi.exe.
// Retail: cdecl (chunk). Returns the 32-bit tag at chunk+0, 0 for null.
#include <cstdint>
#include <cstring>
static inline std::uint32_t rd32(const void* p, std::intptr_t off) { std::uint32_t v; std::memcpy(&v, reinterpret_cast<const std::uint8_t*>(p) + off, 4); return v; }

std::uint32_t __cdecl Ilff_GetChunkTag(void* chunk)
{
    if (chunk == nullptr) return 0;
    return rd32(chunk, 0);
}
