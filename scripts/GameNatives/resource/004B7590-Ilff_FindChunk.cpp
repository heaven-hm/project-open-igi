// IGI native Ilff_FindChunk - 0x004B7590 - transcribed from retail igi.exe.
// Retail: cdecl (firstChunk, tag). Walks the chunk chain with Ilff_GetNextChunk until the tag at chunk+0 matches.
#include <cstdint>
#include <cstring>
static inline std::uint32_t rd32(const void* p, std::intptr_t off) { std::uint32_t v; std::memcpy(&v, reinterpret_cast<const std::uint8_t*>(p) + off, 4); return v; }

void* __cdecl Ilff_GetNextChunk(void* chunk); // 0x004B7470

void* __cdecl Ilff_FindChunk(void* chunk, std::uint32_t tag)
{
    if (chunk == nullptr) return nullptr;
    for (;;) {
        if (rd32(chunk, 0) == tag) return chunk;
        chunk = Ilff_GetNextChunk(chunk);
        if (chunk == nullptr) return nullptr;
    }
}
