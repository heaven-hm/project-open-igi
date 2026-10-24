// IGI native HeapList_Insert - 0x004AF8F0 - transcribed from retail igi.exe.
// Retail: cdecl (node, head). Links head in after the tail pointer stored at node+8: head->prev = node->tail, head->next = node+4, node->tail->next = head, node->tail = head.
#include <cstdint>
#include <cstring>
static inline std::uint32_t rd32(const void* p, std::intptr_t off) { std::uint32_t v; std::memcpy(&v, reinterpret_cast<const std::uint8_t*>(p) + off, 4); return v; }
static inline void wr32(void* p, std::intptr_t off, std::uint32_t v) { std::memcpy(reinterpret_cast<std::uint8_t*>(p) + off, &v, 4); }
static inline void* ptr32(std::uint32_t v) { return reinterpret_cast<void*>(static_cast<std::uintptr_t>(v)); }
static inline std::uint32_t u32(const void* p) { return static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(p)); }


void __cdecl HeapList_Insert(void* node, void* head)
{
    std::uint32_t tail = rd32(node, 8);
    wr32(head, 4, tail);
    wr32(head, 0, u32(node) + 4u);
    tail = rd32(node, 8);
    wr32(ptr32(tail), 0, u32(head));
    wr32(node, 8, u32(head));
}
