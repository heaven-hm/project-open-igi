// IGI native Ilff_GetChunkPayload - 0x004B74D0 - transcribed from retail igi.exe.
// Retail: cdecl (chunk, container). Payload offset is (chunkOffset + chunkAlign + 0xf) & -chunkAlign inside the container; invalid alignment or out-of-range chunk sets g_a43e5c to 1 and returns null; zero chunk size returns null.
#include <cstdint>
#include <cstring>
static inline std::uint32_t rd32(const void* p, std::intptr_t off) { std::uint32_t v; std::memcpy(&v, reinterpret_cast<const std::uint8_t*>(p) + off, 4); return v; }

extern std::int32_t g_a43e5c;

void* __cdecl Ilff_GetChunkPayload(void* chunk, void* container)
{
    if (container == nullptr) return nullptr;
    if (chunk == nullptr) return nullptr;
    std::uint32_t contAlign = rd32(container, 8);
    std::uint32_t chunkOffset = static_cast<std::uint32_t>(reinterpret_cast<std::uint8_t*>(chunk) - reinterpret_cast<std::uint8_t*>(container));
    if (((~(contAlign - 1u)) & contAlign) == contAlign) {
        std::uint32_t contSize = rd32(container, 4);
        if (chunkOffset < contSize) {
            std::uint32_t chunkSize = rd32(chunk, 4);
            std::uint32_t chunkAlign = rd32(chunk, 8);
            if (((~(chunkAlign - 1u)) & chunkAlign) == chunkAlign && !(chunkAlign > contAlign)) {
                if (chunkSize == 0) return nullptr;
                std::uint32_t offset = (chunkAlign + chunkOffset + 0xfu) & ~(chunkAlign - 1u);
                return reinterpret_cast<std::uint8_t*>(container) + offset;
            }
        }
    }
    g_a43e5c = 1;
    return nullptr;
}
