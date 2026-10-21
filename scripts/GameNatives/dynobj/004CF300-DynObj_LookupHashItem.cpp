// IGI native DynObj_LookupHashItem - 0x004CF300 - transcribed from retail igi.exe.
// Retail: cdecl (table, index). Starts at the node pointed to by *table and follows the next pointer at +0 index times; null when the chain ends before the node is reached.
#include <cstdint>
#include <cstring>
static inline std::uint32_t rd32(const void* p, std::intptr_t off) { std::uint32_t v; std::memcpy(&v, reinterpret_cast<const std::uint8_t*>(p) + off, 4); return v; }
static inline void* ptr32(std::uint32_t v) { return reinterpret_cast<void*>(static_cast<std::uintptr_t>(v)); }

void* __cdecl DynObj_LookupHashItem(void* table, std::int32_t index)
{
    void* node = ptr32(rd32(table, 0));
    std::uint32_t next = rd32(node, 0);
    std::uint32_t i = 0;
    if (next == 0) return nullptr;
    for (;;) {
        if (i == static_cast<std::uint32_t>(index)) return node;
        node = ptr32(next);
        next = rd32(node, 0);
        ++i;
        if (next == 0) return nullptr;
    }
}
