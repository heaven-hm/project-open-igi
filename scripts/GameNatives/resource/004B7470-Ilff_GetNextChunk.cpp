// IGI native Ilff_GetNextChunk - 0x004B7470 - transcribed from retail igi.exe.
// Retail: cdecl (chunk). Returns chunk + the 32-bit offset at chunk+0xc, or null when chunk or that offset is 0.
#include <cstdint>
#include <cstring>
static inline std::uint32_t rd32(const void* p, std::intptr_t off) { std::uint32_t v; std::memcpy(&v, reinterpret_cast<const std::uint8_t*>(p) + off, 4); return v; }

void* __cdecl Ilff_GetNextChunk(void* chunk)
{
    if (chunk == nullptr) return nullptr;
    std::uint32_t next = rd32(chunk, 0xc);
    if (next == 0) return nullptr;
    return reinterpret_cast<std::uint8_t*>(chunk) + next;
}
