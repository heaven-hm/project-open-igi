// IGI native Iff_FindChunkBelowForm - 0x005178E0 - transcribed from retail igi.exe.
// Retail: cdecl (node, id). If node+0x14 equals id the search starts at the next sibling. FORM nodes are matched on +0x14 and then searched through their child pointer at +8 with Iff_FindFormChunk (0x00517890); other nodes are matched on +0x14.
#include <cstdint>
#include <cstring>
static inline std::uint32_t rd32(const void* p, std::intptr_t off) { std::uint32_t v; std::memcpy(&v, reinterpret_cast<const std::uint8_t*>(p) + off, 4); return v; }
static inline void* ptr32(std::uint32_t v) { return reinterpret_cast<void*>(static_cast<std::uintptr_t>(v)); }

void* __cdecl Iff_FindFormChunk(void* node, std::uint32_t id); // 0x00517890

void* __cdecl Iff_FindChunkBelowForm(void* node, std::uint32_t id)
{
    if (rd32(node, 0x14) == id) node = ptr32(rd32(node, 0));
    while (node != nullptr) {
        std::uint32_t type = rd32(node, 0x10);
        std::uint32_t key = rd32(node, 0x14);
        if (type == 0x464f524du) {
            if (key == id) return node;
            void* child = ptr32(rd32(node, 8));
            if (child != nullptr) {
                void* found = Iff_FindFormChunk(child, id);
                if (found != nullptr) return found;
            }
        } else if (key == id) {
            return node;
        }
        node = ptr32(rd32(node, 0));
    }
    return nullptr;
}
