// IGI native Iff_FindChunk - 0x00517940 - transcribed from retail igi.exe.
// Retail: cdecl (form, id). Walks the children starting at form+8 and the sibling pointer at +0; FORM children are searched recursively, other children are matched on +0x10.
#include <cstdint>
#include <cstring>
static inline std::uint32_t rd32(const void* p, std::intptr_t off) { std::uint32_t v; std::memcpy(&v, reinterpret_cast<const std::uint8_t*>(p) + off, 4); return v; }
static inline void* ptr32(std::uint32_t v) { return reinterpret_cast<void*>(static_cast<std::uintptr_t>(v)); }

void* __cdecl Iff_FindChunk(void* form, std::uint32_t id)
{
    void* node = ptr32(rd32(form, 8));
    while (node != nullptr) {
        std::uint32_t type = rd32(node, 0x10);
        if (type == 0x464f524du) {
            void* found = Iff_FindChunk(node, id);
            if (found != nullptr) return found;
        } else if (type == id) {
            return node;
        }
        node = ptr32(rd32(node, 0));
    }
    return nullptr;
}
