// IGI native Ilff_GetFirstChunk - 0x004B7490 - transcribed from retail igi.exe.
// Retail: cdecl (container). Header size at +4, alignment at +8 (must be a power of two, else g_a43e5c is set to 1 and null returned). First chunk is container + ((align + 0x13) & -align), null when that offset reaches the size.
#include <cstdint>
#include <cstring>
static inline std::uint32_t rd32(const void* p, std::intptr_t off) { std::uint32_t v; std::memcpy(&v, reinterpret_cast<const std::uint8_t*>(p) + off, 4); return v; }

extern std::int32_t g_a43e5c;

void* __cdecl Ilff_GetFirstChunk(void* container)
{
    if (container == nullptr) return nullptr;
    std::uint32_t size = rd32(container, 4);
    if (size == 0) return nullptr;
    std::uint32_t align = rd32(container, 8);
    std::uint32_t mask = ~(align - 1u);
    if ((mask & align) != align) {
        g_a43e5c = 1;
        return nullptr;
    }
    std::uint32_t offset = (align + 0x13u) & mask;
    if (offset >= size) return nullptr;
    return reinterpret_cast<std::uint8_t*>(container) + offset;
}
