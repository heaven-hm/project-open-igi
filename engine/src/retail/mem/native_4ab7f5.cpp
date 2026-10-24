// Native: Mem_FindBucketBlock_4AB7F5 (placeholder until VERIFIED); retail VA 0x004AB7F5-0x004AB84B (87 bytes, 7 blocks, 8 edges).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 complete listing (8b442404 bab0465400 56 8bca 3b4110 7605 3b4114 7208 8b09 3bca 7437 ebee a80f 7531 8bf0 ba00010000 81e6ff0f0000 3bf2 7220 ... 33c0 5e c3); the sentinel bytes at 0x005446B0 were read directly (pxw: 005446B0 005446B0 005446C8 005446C8 FFFFFFFF FFFFFFFF).
// Behavior/CFG: walks the bucket list whose head is the sentinel at 0x005446B0 looking for the node whose [node+0x10] < value < [node+0x14]; stops at the sentinel and returns 0. On a hit it returns 0 when the value is not 16-byte aligned or when (value & 0xFFF) < 0x100; otherwise it stores the node pointer through arg2, stores (value & 0xF000) through arg3 and returns (value - (value & 0xF000) - 0x100) / 16 + (value & 0xF000) + 8.
// Status/limits: TRANSCRIBED (static only); the 0x005446B0 sentinel is modelled as image data with the retail byte values (self links, payload pointers 0x005446C8, and -1/-1 bounds), matched by semantics rather than by hard-coded address; the 0x100 constant is retail's own immediate (r2 renders the 0x100 immediate as a PE symbol name, it is not a call); runtime UNOBSERVED.
#include <cstdint>

namespace {

struct Mode2Sentinel
{
    std::uint32_t next;         // 0x005446B0: links to the sentinel itself in the image
    std::uint32_t head;         // 0x005446B4: also the sentinel itself
    std::uint32_t payload_low;  // 0x005446B8: 0x005446C8 = sentinel + 0x18
    std::uint32_t payload_high; // 0x005446BC: same
    std::int32_t low;           // 0x005446C0: -1
    std::int32_t high;          // 0x005446C4: -1
};

Mode2Sentinel g_5446b0 = {
    static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(&g_5446b0)), // self link
    static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(&g_5446b0)), // head link
    0x005446C8U,                                                             // payload pointer, image constant
    0x005446C8U,                                                             // payload pointer, image constant
    -1,
    -1,
};

} // namespace

extern "C" {

void* Mem_FindBucketBlock_4AB7F5(std::uint32_t const value, std::uint32_t* const out_node,
                                 std::uint32_t* const out_base)
{
    const std::uint32_t root = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(&g_5446b0)); // 0x004AB7F9: mov edx, 0x5446b0
    std::uint32_t node = root;                                                                          // 0x004AB7FF: mov ecx, edx
    for (;;)
    {
        const auto* const entry = reinterpret_cast<const Mode2Sentinel*>(static_cast<std::uintptr_t>(node));
        if (value <= static_cast<std::uint32_t>(entry->low))                                            // 0x004AB801: cmp eax,[ecx+0x10]; jbe
        {
            node = entry->next;                                                                         // 0x004AB80B: mov ecx,[ecx]
            if (node == root)                                                                           // 0x004AB80D: cmp ecx,edx; je
            {
                return nullptr;                                                                         // 0x004AB848: xor eax,eax; ret
            }
            continue;                                                                                   // 0x004AB811: jmp
        }
        if (value < static_cast<std::uint32_t>(entry->high))                                            // 0x004AB806: cmp eax,[ecx+0x14]; jb
        {
            break;                                                                                      // 0x004AB813: found
        }
        node = entry->next;                                                                             // 0x004AB80B
        if (node == root)                                                                               // 0x004AB80D
        {
            return nullptr;                                                                             // 0x004AB848
        }
    }
    if ((value & 0x0FU) != 0U)                                                                          // 0x004AB813: test al,0xf; jne
    {
        return nullptr;                                                                                 // 0x004AB848
    }
    if ((value & 0x0FFFU) < 0x100U)                                                                     // 0x004AB819/0x004AB824: mov edx,0x100; and esi,0xfff; cmp esi,edx; jb
    {
        return nullptr;                                                                                 // 0x004AB848
    }
    *out_node = node;                                                                                   // 0x004AB82C: mov [esi], ecx
    const std::uint32_t base = value & 0xF000U;                                                         // 0x004AB834: and cx,0xf000
    *out_base = base;                                                                                   // 0x004AB83B: mov [esi], ecx
    const std::uint32_t offset = (value - base - 0x100U) >> 4;                                          // 0x004AB839/0x004AB83D/0x004AB840: sub; sub; sar eax,4
    return reinterpret_cast<void*>(static_cast<std::uintptr_t>(offset + base + 8U));                    // 0x004AB843: lea eax,[eax+ecx+8]
}

} // extern "C"
