// IGI native Iff_FindFormChunk - 0x00517890 - transcribed from retail igi.exe.
// Retail: cdecl (node, id). Walks the sibling list through the pointer at node+0; FORM nodes (0x464F524D at +0x10) are matched on +0x14 and then searched recursively through the child pointer at +8, other nodes are matched on +0x14.
#include <cstdint>
#include <cstring>
static inline std::uint32_t rd32(const void* p, std::intptr_t off) { std::uint32_t v; std::memcpy(&v, reinterpret_cast<const std::uint8_t*>(p) + off, 4); return v; }

void* __cdecl Iff_FindFormChunk(void* node, std::uint32_t id)
{
    while (node != nullptr) {
        std::uint32_t type = rd32(node, 0x10);
        std::uint32_t key = rd32(node, 0x14);
        if (type == 0x464f524du) {
            if (key == id) return node;
            void* child = reinterpret_cast<void*>(static_cast<std::uintptr_t>(rd32(node, 8)));
            if (child != nullptr) {
                void* found = Iff_FindFormChunk(child, id);
                if (found != nullptr) return found;
            }
        } else if (key == id) {
            return node;
        }
        node = reinterpret_cast<void*>(static_cast<std::uintptr_t>(rd32(node, 0)));
    }
    return nullptr;
}
