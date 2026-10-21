// IGI native Iff_GetChunkPayload - 0x005179C0 - transcribed from retail igi.exe.
// Retail: cdecl (chunk). Returns the 32-bit value at chunk+0x1c.
#include <cstdint>
#include <cstring>
static inline std::uint32_t rd32(const void* p, std::intptr_t off) { std::uint32_t v; std::memcpy(&v, reinterpret_cast<const std::uint8_t*>(p) + off, 4); return v; }
static inline void* ptr32(std::uint32_t v) { return reinterpret_cast<void*>(static_cast<std::uintptr_t>(v)); }

std::uint32_t __cdecl Iff_GetChunkPayload(void* chunk)
{
    return rd32(chunk, 0x1c);
}
