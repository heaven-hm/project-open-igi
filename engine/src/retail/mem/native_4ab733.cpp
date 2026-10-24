// Native: Mem_PurgeBucketPages_4AB733 (placeholder until VERIFIED); retail VA 0x004AB733-0x004AB7F4 (194 bytes, 20 blocks, 22 edges).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 complete listing (55 8bec 51 53 56 8b35b4465400 57 837e10ff 0f8494000000 8365fc00 8dbe10200000 bb00f03f00 813ff0000000 ... 0f8f50ffffff 5f 5e 5b c9 c3); imports KERNEL32 VirtualFree.
// Behavior/CFG: walks the circular node list from 0x005446B4, decommitting committed pages. For each real node it scans walk entries from node+0x2010 downwards by 8 while the page offset falls from 0x3FF000 by 0x1000; a 0xF0 entry means a committed page, so it VirtualFree(base + offset, 0x1000, MEM_DECOMMIT), writes -1 over the entry, decrements the committed-page count 0x00936200, records the lowest decommitted slot in [node+0x0C], and stops the whole walk once `count` pages have been decommitted. After a node that purged anything, if its first entry is -1 and every one of the 0x400 entries is -1, it frees the node through 0x4AB6DD. It stops when it returns to the list anchor or when `count` is exhausted.
// Status/limits: TRANSCRIBED (static only); slot bookkeeping is walked backwards exactly as retail does (first entry node+0x2010, last node+0x18); the 0xFFFFFFFF bound marks the sentinel node and skips the scan; VirtualFree is retail's own KERNEL32 import reached through ::VirtualFree; runtime UNOBSERVED and unreachable while the heap mode is not 2.
#include "igi/retail/mem/mode2_bucket.hpp"

#include <cstdint>
#include <windows.h>

using igi::retail::mem::g_5446b0;
using igi::retail::mem::g_936200;
using igi::retail::mem::Mode2BucketNode;

extern "C" {

void Mem_FreeBucketNode_4AB6DD(Mode2BucketNode* node); // 0x004AB7D8: call 0x4ab6dd

void Mem_PurgeBucketPages_4AB733(std::int32_t count)
{
    Mode2BucketNode* node = g_5446b0.next;                                              // 0x004AB739: mov esi,[0x5446b4]
    for (;;)                                                                            // 0x004AB740
    {
        if (reinterpret_cast<std::uintptr_t>(node->base_10) != 0xFFFFFFFFU)             // 0x004AB740: cmp [esi+0x10],-1; je 0x4AB7DE
        {
            std::int32_t purged = 0;                                                    // 0x004AB74A: and dword [ebp-4],0
            std::uint32_t* slot = &node->page_entries[0x7FE];                            // 0x004AB74E: lea edi,[esi+0x2010]
            std::int32_t offset = 0x3FF000;                                             // 0x004AB754: mov ebx,0x3ff000
            for (;;)                                                                    // 0x004AB759
            {
                if (*slot == 0xF0U)                                                     // 0x004AB759: cmp dword [edi],0xf0; jne
                {
                    void* const page = static_cast<std::uint8_t*>(node->base_10) + offset; // 0x004AB768: add eax,[esi+0x10]
                    if (::VirtualFree(page, 0x1000, MEM_DECOMMIT) != 0)                 // 0x004AB763/6B/70/71: push 0x4000; push 0x1000; push eax; call [KERNEL32.VirtualFree]
                    {
                        *slot = 0xFFFFFFFFU;                                            // 0x004AB77B: or dword [edi],0xffffffff
                        --g_936200;                                                     // 0x004AB77E: dec dword [0x936200]
                        const auto slot_address = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(slot));
                        if (node->slot_marker_0c == 0U || node->slot_marker_0c > slot_address) // 0x004AB784..0x004AB78D: test eax,eax; je; cmp eax,edi; jbe
                        {
                            node->slot_marker_0c = slot_address;                        // 0x004AB78F: mov [esi+0xc],edi
                        }
                        ++purged;                                                       // 0x004AB792: inc dword [ebp-4]
                        if (--count == 0)                                               // 0x004AB795/0x004AB798: dec [ebp+8]; je 0x4AB7A7
                        {
                            break;
                        }
                    }
                }
                offset -= 0x1000;                                                       // 0x004AB79A: sub ebx,0x1000
                slot -= 2;                                                              // 0x004AB7A0: sub edi,8
                if (offset < 0)                                                         // 0x004AB7A3/0x004AB7A5: test ebx,ebx; jge
                {
                    break;
                }
            }
            Mode2BucketNode* const previous = node;                                     // 0x004AB7AB: mov ecx,esi
            node = node->next;                                                          // 0x004AB7AD: mov esi,[esi+4]
            if (purged != 0)                                                            // 0x004AB7A7/0x004AB7B0: cmp [ebp-4],0; je 0x4AB7DE
            {
                if (previous->page_entries[0] == 0xFFFFFFFFU)                           // 0x004AB7B2: cmp [ecx+0x18],-1; jne 0x4AB7DE
                {
                    std::int32_t filled = 1;                                            // 0x004AB7B8/0x004AB7BD: push 1; pop edx
                    const std::uint32_t* scan = &previous->page_entries[2];             // 0x004AB7BA: lea eax,[ecx+0x20]
                    for (;;)
                    {
                        if (*scan != 0xFFFFFFFFU)                                       // 0x004AB7BE: cmp dword [eax],-1; jne 0x4AB7CF
                        {
                            break;
                        }
                        ++filled;                                                       // 0x004AB7C3: inc edx
                        scan += 2;                                                      // 0x004AB7C4: add eax,8
                        if (filled >= 0x400)                                            // 0x004AB7C7/0x004AB7CD: cmp edx,0x400; jl
                        {
                            break;
                        }
                    }
                    if (filled == 0x400)                                                // 0x004AB7CF/0x004AB7D5: cmp edx,0x400; jne
                    {
                        Mem_FreeBucketNode_4AB6DD(previous);                            // 0x004AB7D7/0x004AB7D8: push ecx; call 0x4ab6dd
                    }
                }
            }
        }
        if (node == g_5446b0.next)                                                      // 0x004AB7DE: cmp esi,[0x5446b4]; je 0x4AB7F0
        {
            return;
        }
        if (count <= 0)                                                                 // 0x004AB7E6/0x004AB7EA: cmp [ebp+8],0; jg 0x4AB740
        {
            return;
        }
    }
}

} // extern "C"
