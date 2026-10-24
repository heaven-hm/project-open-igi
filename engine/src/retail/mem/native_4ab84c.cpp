// Native: Mem_MarkBucketPage (placeholder until VERIFIED); retail VA 0x004AB84C-0x004AB890 (69 bytes, 7 blocks, 8 edges).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679914a01.
// Tools/evidence: radare2 6.2.0 complete listing (8b442404 8b4c2408 2b4810 c1f90c 8d44c818 8b4c240c 0fb611 0110 802100 8138f0000000 c74004f100000000 7517 ff0500629300 833d006293..20 7508 6a10 e8a4feffff 59 c3); storage owned by 0x4AB733/mode2_bucket_globals.
// Behavior/CFG: computes the page entry from the pointer (offset = value - [node+0x10], page = offset >> 12, entry at node+0x18+8*page), adds the byte flag through arg3 to the entry's first dword, clears the flag byte, always writes 0xF1 to the entry's second dword, and when the first dword reaches 0xF0 increments the committed-page count 0x00936200 and, once that count reaches 0x20, calls the purge 0x4AB733 with 0x10.
// Status/limits: TRANSCRIBED (static only); the 0xF1 store at entry+4 happens before the 0xF0 comparison branches, so it is unconditional and is preserved that way; runtime UNOBSERVED and unreachable while the heap mode is not 2.
#include "igi/retail/mem/mode2_bucket.hpp"

#include <cstdint>

using igi::retail::mem::g_936200;
using igi::retail::mem::Mode2BucketNode;

extern "C" {

void Mem_PurgeBucketPages(std::int32_t count); // 0x004AB88A: call 0x4ab733

void Mem_MarkBucketPage(Mode2BucketNode* const node, void* const value, std::uint8_t* const flag)
{
    const std::uint32_t page = static_cast<std::uint32_t>(
        (reinterpret_cast<std::uintptr_t>(value) - reinterpret_cast<std::uintptr_t>(node->base_10)) >> 12); // 0x004AB854/0x004AB857: sub ecx,[eax+0x10]; sar ecx,0xc
    std::uint32_t* const entry = &node->page_entries[static_cast<std::uintptr_t>(page) * 2U];              // 0x004AB85A: lea eax,[eax+ecx*8+0x18]
    entry[0] += static_cast<std::uint32_t>(*flag);                                                // 0x004AB862/0x004AB865: movzx edx,[ecx]; add [eax],edx
    *flag = 0;                                                                                    // 0x004AB867: and byte [ecx],0
    entry[1] = 0xF1U;                                                                             // 0x004AB870: mov dword [eax+4],0xf1
    if (entry[0] == 0xF0U)                                                                        // 0x004AB86A/0x004AB877: cmp dword [eax],0xf0; jne
    {
        ++g_936200;                                                                               // 0x004AB879: inc dword [0x936200]
        if (g_936200 == 0x20)                                                                     // 0x004AB87F/0x004AB886: cmp [0x936200],0x20; jne
        {
            Mem_PurgeBucketPages(0x10);                                                    // 0x004AB888/0x004AB88A: push 0x10; call 0x4ab733
        }
    }
}

} // extern "C"
