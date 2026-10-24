// IGI native HeapList_Remove - 0x004AF960 - transcribed from retail igi.exe.
// Retail: cdecl (node). Unlinks the node from its neighbours (prev at +4, next at +0) and clears both pointers.
#include <cstdint>
#include <cstring>
static inline std::uint32_t rd32(const void* p, std::intptr_t off) { std::uint32_t v; std::memcpy(&v, reinterpret_cast<const std::uint8_t*>(p) + off, 4); return v; }
static inline void wr32(void* p, std::intptr_t off, std::uint32_t v) { std::memcpy(reinterpret_cast<std::uint8_t*>(p) + off, &v, 4); }
static inline void* ptr32(std::uint32_t v) { return reinterpret_cast<void*>(static_cast<std::uintptr_t>(v)); }
static inline std::uint32_t u32(const void* p) { return static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(p)); }


void __cdecl HeapList_Remove(void* node)
{
    std::uint32_t prev = rd32(node, 4);
    std::uint32_t next = rd32(node, 0);
    wr32(ptr32(prev), 0, next);
    next = rd32(node, 0);
    prev = rd32(node, 4);
    wr32(ptr32(next), 4, prev);
    wr32(node, 4, 0);
    wr32(node, 0, 0);
}
