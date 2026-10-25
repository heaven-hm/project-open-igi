// Native: Mem_FreeBlock (placeholder until VERIFIED); retail VA 0x004A6A50-0x004A6AB8 (105 bytes, 10 blocks).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 complete listing (55 8bec 51 56 8b7508 85f6 745a a12c24c300 83f803 7516 56 e824430000 59 85c0 56 7436 50 e843430000 59 59 eb3a 83f802 7526 ... 56 6a00 ff352824c300 ff15a4305300 5e c9 c3); imports KERNEL32 HeapFree.
// Behavior/CFG: the allocator-mode dispatcher for free. A null pointer returns immediately. Mode 3 (0x00C3242C == 3) looks the pointer up in the small-block pool with 0x4AAD90 and frees it through 0x4AADBB when found. Mode 2 looks it up in the bucket heap with 0x4AB7F5 and marks the page through 0x4AB84C when found, passing the node, the masked base and the returned byte address. Every path that does not find the block - including any other mode value - falls through to HeapFree([0x00C32428], 0, pointer).
// Status/limits: STRUCTURALLY_VERIFIED (static only); both "not found" branches are the same shared HeapFree tail in retail (0x4A6AA7), preserved here; runtime UNOBSERVED.
#include "igi/retail/mem/mem_alloc.hpp"
#include "igi/retail/mem/mode2_bucket.hpp"

#include <cstdint>
#include <windows.h>

using igi::retail::mem::g_c32428;
using igi::retail::mem::g_c3242c;
using igi::retail::mem::Mode2BucketNode;

extern "C" {

void* Pool_FindBlock(void* pointer);                                                    // 0x004A6A67: call 0x4aad90
void Mem_FreeSmallBlock(void* arena, void* pointer);                                    // 0x004A6A73: call 0x4aadbb
void* Mem_FindBucketBlock(std::uint32_t value, std::uint32_t* node, std::uint32_t* base); // 0x004A6A8A: call 0x4ab7f5
void Mem_MarkBucketPage(Mode2BucketNode* node, void* value, std::uint8_t* flag);         // 0x004A6A9D: call 0x4ab84c

void Mem_FreeBlock(void* const pointer)
{
    if (pointer == nullptr)                                                                    // 0x004A6A58/0x004A6A5A: test esi,esi; je
    {
        return;                                                                                // 0x004A6AB6..0x004A6AB8
    }
    if (g_c3242c == 3U)                                                                        // 0x004A6A5C/0x004A6A61/0x004A6A64: cmp eax,3; jne
    {
        void* const entry = Pool_FindBlock(pointer);                                    // 0x004A6A67
        if (entry != nullptr)                                                                  // 0x004A6A6D/0x004A6A70: test eax,eax; je 0x4A6AA8
        {
            Mem_FreeSmallBlock(entry, pointer);                                         // 0x004A6A73
            return;                                                                            // 0x004A6A7A: jmp 0x4A6AB6
        }
    }
    else if (g_c3242c == 2U)                                                                   // 0x004A6A7C/0x004A6A7F: cmp eax,2; jne 0x4A6AA7
    {
        std::uint32_t node = 0;
        std::uint32_t base = 0;
        void* const address = Mem_FindBucketBlock(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(pointer)),
                                                         &node, &base);                        // 0x004A6A81..0x004A6A8A
        if (address != nullptr)                                                                // 0x004A6A92/0x004A6A94: test eax,eax; je 0x4A6AA7
        {
            Mem_MarkBucketPage(reinterpret_cast<Mode2BucketNode*>(static_cast<std::uintptr_t>(node)),
                                      reinterpret_cast<void*>(static_cast<std::uintptr_t>(base)),
                                      static_cast<std::uint8_t*>(address));                    // 0x004A6A96..0x004A6A9D
            return;                                                                            // 0x004A6AA5: jmp 0x4A6AB6
        }
    }
    ::HeapFree(static_cast<HANDLE>(g_c32428), 0, pointer);                                     // 0x004A6AA7/0x004A6AA8/0x004A6AAA/0x004A6AB0: push esi; push 0; push [0xc32428]; call [KERNEL32.HeapFree]
}

} // extern "C"
