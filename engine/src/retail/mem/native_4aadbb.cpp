// Native: Mem_FreeSmallBlock (placeholder until VERIFIED); retail VA 0x004AADBB-0x004AB0E3 (809 bytes, 265 instructions).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 exact-range listing committed at docs/fidelity/evidence/0x004AADBB-full-listing.txt (pD 809 @ 0x4AADBB); the 0x80000000 constants below are retail's own immediates that radare2 renders as the unrelated DirectSoundCreate import symbol.
// Behavior/CFG: small-block free with forward and backward coalescing into the 0x204-byte-per-segment bucket structures at [[arena+0x10] + segment*0x204 + 0x144]. It takes the block header at pointer-4, returns without doing anything when ((header-1) & 1) is set, coalesces with the following block when its size is even (removing it from its doubly-linked free list through [block+4]/[block+8] and clearing its size-class bit in the 0x44 or 0xC4 bitmap when it was the last element), then coalesces with the preceding block when the preceding size word is even, resizing between size classes, inserting the merged block at the head of its class list, setting that class's bitmap bit and per-class count, then writes the new size into the header and footer, decrements the segment's block count, and - only when the segment becomes empty - decommits and releases its 0x8000-byte arena page, compacts the 0x140-byte pool entries at 0xC321F8 by 0x14 through 0x4ACD70, which is the statically linked MSVC CRT memmove (jump-table-driven overlapping copy, not a game routine), and updates 0xC321E8/0xC321F0/0xC321EC.
// Status/limits: STRUCTURALLY_VERIFIED (static only); block-order and register reuse are preserved, including that arg2 is overwritten with the next block pointer and then with the back-merged block; the final arena-release path is retail's own VirtualFree/HeapFree sequence and its call to the CRT memmove at 0x4ACD70, which is reproduced with std::memmove rather than reimplemented as a game native; 0x4ACD70 is 821 bytes of jump-table-driven overlapping copy, so its retail CFG is not comparable to a standard-library call and it is deliberately not claimed as a CFG-verified native; runtime UNOBSERVED and unreachable while the heap mode is not 3.
#include "igi/retail/mem/mem_alloc.hpp"
#include "igi/retail/mem/mode2_bucket.hpp"

#include <cstdint>
#include <cstring>
#include <windows.h>

using igi::retail::mem::g_c321e8;
using igi::retail::mem::g_c321ec;
using igi::retail::mem::g_c321f0;
using igi::retail::mem::g_c321f4;
using igi::retail::mem::g_c321f8;
using igi::retail::mem::g_c32428;

extern "C" {

void Mem_FreeSmallBlock(void* const arena_argument, void* const pointer)
{
    void* arena = arena_argument;                                                                                   // [ebp+8]: adjusted by 0x14 when the pool entry before it is removed
    std::uint32_t* const bitmap = *reinterpret_cast<std::uint32_t**>(static_cast<std::uint8_t*>(arena) + 0x10);      // 0x004AADC9: mov eax,[ecx+0x10]
    const std::uintptr_t region_base = *reinterpret_cast<std::uint32_t*>(static_cast<std::uint8_t*>(arena) + 0x0C);   // 0x004AADD2: sub edi,[ecx+0xc]
    const std::uint32_t segment = static_cast<std::uint32_t>((reinterpret_cast<std::uintptr_t>(pointer) - region_base) >> 15); // 0x004AADD5: shr edi,0xf
    std::uint8_t* const bucket = reinterpret_cast<std::uint8_t*>(bitmap) +
                                 static_cast<std::uintptr_t>(segment) * 0x204U + 0x144U;                              // 0x004AADDA/0x004AADE0/0x004AADE7
    std::uint8_t* block = static_cast<std::uint8_t*>(pointer) - 4;                                                    // 0x004AADCF: add esi,0xfffffffc
    std::uint32_t size = *reinterpret_cast<std::uint32_t*>(block);                                                    // 0x004AADEA: mov ecx,[esi]
    --size;                                                                                                          // 0x004AADEC: dec ecx
    if ((size & 1U) != 0U)                                                                                           // 0x004AADED/0x004AADF3: test cl,1; jne 0x4AB0DF
    {
        return;                                                                                                      // 0x004AB0DF..0x004AB0E3
    }
    std::uint32_t next_size = *reinterpret_cast<std::uint32_t*>(block + size);                                       // 0x004AADF9: mov edx,[ecx+esi]
    std::uint8_t* const next_block = block + size;                                                                   // 0x004AADFC: lea ebx,[ecx+esi]
    const std::uint32_t prev_size = *reinterpret_cast<std::uint32_t*>(block - 4);                                    // 0x004AAE02: mov edx,[esi-4]
    if ((next_size & 1U) == 0U)                                                                                      // 0x004AAE0B: test dl,1; jne 0x4AAE91
    {
        std::uint32_t next_index = next_size >> 4;                                                                   // 0x004AAE13: sar edx,4
        --next_index;                                                                                                // 0x004AAE16: dec edx
        if (next_index > 0x3FU)                                                                                      // 0x004AAE17/0x004AAE1A: cmp edx,0x3f; jbe
        {
            next_index = 0x3FU;                                                                                      // 0x004AAE1C/0x004AAE1E: push 0x3f; pop edx
        }
        if (*reinterpret_cast<std::uint32_t*>(next_block + 4) == *reinterpret_cast<std::uint32_t*>(next_block + 8))  // 0x004AAE1F/0x004AAE22/0x004AAE25: cmp; jne 0x4AAE73
        {
            if (next_index < 0x20U)                                                                                  // 0x004AAE27/0x004AAE2A: cmp edx,0x20; jae
            {
                std::uint32_t mask = 0x80000000U >> next_index;                                                      // 0x004AAE2C/0x004AAE33: mov ebx,0x80000000; shr ebx,cl
                mask = ~mask;                                                                                        // 0x004AAE39: not ebx
                bitmap[segment + 0x44U / 4U] &= mask;                                                                // 0x004AAE3B: and dword [eax+edi*4+0x44],ebx
                if (--*reinterpret_cast<std::uint8_t*>(reinterpret_cast<std::uint8_t*>(bitmap) + next_index + 4) == 0) // 0x004AAE35/0x004AAE3F/0x004AAE41: lea ecx,[edx+eax+4]; dec byte [ecx]; jne
                {
                    reinterpret_cast<std::uint32_t*>(arena)[0] &= mask;                                              // 0x004AAE43/0x004AAE46: mov ecx,[ebp+8]; and dword [ecx],ebx
                }
            }
            else
            {
                std::uint32_t mask = 0x80000000U >> (next_index - 0x20U);                                            // 0x004AAE4A/0x004AAE4D/0x004AAE52: lea ecx,[edx-0x20]; mov ebx,0x80000000; shr ebx,cl
                mask = ~mask;                                                                                        // 0x004AAE58: not ebx
                bitmap[segment + 0xC4U / 4U] &= mask;                                                                // 0x004AAE5A: and dword [eax+edi*4+0xc4],ebx
                if (--*reinterpret_cast<std::uint8_t*>(reinterpret_cast<std::uint8_t*>(bitmap) + next_index + 4) == 0) // 0x004AAE54/0x004AAE61/0x004AAE63: lea ecx,[edx+eax+4]; dec byte [ecx]; jne
                {
                    reinterpret_cast<std::uint32_t*>(arena)[1] &= mask;                                              // 0x004AAE65/0x004AAE68: mov ecx,[ebp+8]; and dword [ecx+4],ebx
                }
            }
        }
        {
            std::uint32_t* const next_prev = reinterpret_cast<std::uint32_t*>(*reinterpret_cast<std::uint32_t*>(next_block + 8)); // 0x004AAE76: mov edx,[ebx+8]
            std::uint32_t* const next_next = reinterpret_cast<std::uint32_t*>(*reinterpret_cast<std::uint32_t*>(next_block + 4)); // 0x004AAE79: mov ebx,[ebx+4]
            size += next_size;                                                                                       // 0x004AAE7C: add ecx,[ebp-0xc]
            next_prev[1] = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(next_next));                  // 0x004AAE7F: mov [edx+4],ebx
            next_next[2] = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(next_prev));                  // 0x004AAE8E: mov [ebx+8],edx
        }
    }
    std::uint32_t index = size >> 4;                                                                                 // 0x004AAE91/0x004AAE93: mov edx,ecx; sar edx,4
    --index;                                                                                                         // 0x004AAE96: dec edx
    if (index > 0x3FU)                                                                                               // 0x004AAE97/0x004AAE9A
    {
        index = 0x3FU;                                                                                               // 0x004AAE9C/0x004AAE9E
    }
    const std::uint32_t prev_in_use = prev_size & 1U;                                                                // 0x004AAE9F/0x004AAEA2: mov ebx,[ebp-8]; and ebx,1
    std::uint32_t old_index = 0;
    if (prev_in_use == 0U)                                                                                           // 0x004AAEA5/0x004AAEA8: mov [ebp-0xc],ebx; jne 0x4AAF42
    {
        std::uint8_t* const merged = block - prev_size;                                                              // 0x004AAEAE: sub esi,[ebp-8]
        old_index = (prev_size >> 4);                                                                                // 0x004AAEB1/0x004AAEB4: sar ebx,4
        --old_index;                                                                                                 // 0x004AAEBC: dec ebx
        if (old_index > 0x3FU)                                                                                       // 0x004AAEBE/0x004AAEC0: cmp ebx,esi(0x3f); jbe
        {
            old_index = 0x3FU;                                                                                       // 0x004AAEC2: mov ebx,esi
        }
        size += prev_size;                                                                                           // 0x004AAEC4: add ecx,[ebp-8]
        {
            std::uint32_t merged_index = size >> 4;                                                                  // 0x004AAEC7/0x004AAECC: mov edx,ecx; sar edx,4
            --merged_index;                                                                                          // 0x004AAECF: dec edx
            if (merged_index > 0x3FU)                                                                                // 0x004AAED0/0x004AAED2
            {
                merged_index = 0x3FU;                                                                                // 0x004AAED4: mov edx,esi
            }
            index = merged_index;
        }
        if (old_index != index)                                                                                      // 0x004AAED6/0x004AAED8: cmp ebx,edx; je 0x4AAF3D
        {
            if (*reinterpret_cast<std::uint32_t*>(merged + 4) == *reinterpret_cast<std::uint32_t*>(merged + 8))      // 0x004AAEDD/0x004AAEE0/0x004AAEE3: cmp esi,[ecx+8]; jne 0x4AAF25
            {
                if (old_index < 0x20U)                                                                               // 0x004AAEE5/0x004AAEE8: cmp ebx,0x20; jae
                {
                    std::uint32_t mask = ~(0x80000000U >> old_index);                                                // 0x004AAEEA/0x004AAEF1/0x004AAEF3
                    bitmap[segment + 0x44U / 4U] &= mask;                                                            // 0x004AAEF5
                    if (--*reinterpret_cast<std::uint8_t*>(reinterpret_cast<std::uint8_t*>(bitmap) + old_index + 4) == 0) // 0x004AAEF9/0x004AAEFD
                    {
                        reinterpret_cast<std::uint32_t*>(arena)[0] &= mask;                                          // 0x004AAEFF/0x004AAF02
                    }
                }
                else
                {
                    std::uint32_t mask = ~(0x80000000U >> (old_index - 0x20U));                                      // 0x004AAF06/0x004AAF09/0x004AAF0E
                    bitmap[segment + 0xC4U / 4U] &= mask;                                                            // 0x004AAF12
                    if (--*reinterpret_cast<std::uint8_t*>(reinterpret_cast<std::uint8_t*>(bitmap) + old_index + 4) == 0) // 0x004AAF19/0x004AAF1D
                    {
                        reinterpret_cast<std::uint32_t*>(arena)[1] &= mask;                                          // 0x004AAF1F/0x004AAF22
                    }
                }
            }
            {
                std::uint32_t* const merged_prev = reinterpret_cast<std::uint32_t*>(*reinterpret_cast<std::uint32_t*>(merged + 8)); // 0x004AAF28
                std::uint32_t* const merged_next = reinterpret_cast<std::uint32_t*>(*reinterpret_cast<std::uint32_t*>(merged + 4)); // 0x004AAF2B
                merged_prev[1] = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(merged_next));          // 0x004AAF2E
                merged_next[2] = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(merged_prev));          // 0x004AAF3A
            }
        }
        block = merged;                                                                                              // 0x004AAF3D: mov esi,[ebp+0xc]
    }
    else
    {
        old_index = 0;                                                                                               // 0x004AAF42: mov ebx,[ebp+8] (arena reused as the old class word)
    }
    if (prev_in_use != 0U || old_index != index)                                                                     // 0x004AAF45/0x004AAF49/0x004AAF4B/0x004AAF4D: cmp [ebp-0xc],0; jne; cmp ebx,edx; je 0x4AAFD4
    {
        const std::uintptr_t list = reinterpret_cast<std::uintptr_t>(bucket) + static_cast<std::uintptr_t>(index) * 8U; // 0x004AAF53/0x004AAF56/0x004AAF5A
        *reinterpret_cast<std::uint32_t*>(block + 4) = *reinterpret_cast<std::uint32_t*>(list + 4);                  // 0x004AAF5D: mov [esi+4],ebx
        *reinterpret_cast<std::uint32_t*>(block + 8) = *reinterpret_cast<std::uint32_t*>(list);                      // 0x004AAF60: mov [esi+8],ecx
        *reinterpret_cast<std::uint32_t*>(list + 4) = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(block)); // 0x004AAF63: mov [ecx+4],esi
        *reinterpret_cast<std::uint32_t*>(*reinterpret_cast<std::uint32_t*>(block + 4) + 8U) = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(block)); // 0x004AAF66/0x004AAF69: mov ecx,[esi+4]; mov [ecx+8],esi
        if (*reinterpret_cast<std::uint32_t*>(block + 4) == *reinterpret_cast<std::uint32_t*>(block + 8))            // 0x004AAF6C/0x004AAF6F/0x004AAF72: cmp ecx,[esi+8]; jne 0x4AAFD4
        {
            const std::uint8_t class_count = *reinterpret_cast<std::uint8_t*>(reinterpret_cast<std::uint8_t*>(bitmap) + index + 4); // 0x004AAF74: mov cl,[edx+eax+4]
            *reinterpret_cast<std::uint8_t*>(reinterpret_cast<std::uint8_t*>(bitmap) + index + 4) = static_cast<std::uint8_t>(class_count + 1); // 0x004AAF7E/0x004AAF80: inc cl; mov [edx+eax+4],cl
            if (index < 0x20U)                                                                                       // 0x004AAF78/0x004AAF84: cmp edx,0x20; jae
            {
                if (class_count == 0U)                                                                               // 0x004AAF86/0x004AAF8A: cmp byte [ebp+0xf],0; jne
                {
                    reinterpret_cast<std::uint32_t*>(arena)[0] |= 0x80000000U >> index;                              // 0x004AAF8C..0x004AAF98
                }
                bitmap[segment + 0x44U / 4U] |= 0x80000000U >> index;                                                // 0x004AAF9A..0x004AAFA7
            }
            else
            {
                if (class_count == 0U)                                                                               // 0x004AAFAB/0x004AAFAF
                {
                    reinterpret_cast<std::uint32_t*>(arena)[1] |= 0x80000000U >> (index - 0x20U);                    // 0x004AAFB1..0x004AAFBE
                }
                bitmap[segment + 0xC4U / 4U] |= 0x80000000U >> (index - 0x20U);                                      // 0x004AAFC1..0x004AAFD2
            }
        }
    }
    *reinterpret_cast<std::uint32_t*>(block) = size;                                                                 // 0x004AAFD4/0x004AAFD7: mov eax,[ebp-4]; mov [esi],eax
    *reinterpret_cast<std::uint32_t*>(block + size - 4) = size;                                                      // 0x004AAFD9: mov [eax+esi-4],eax
    if (--*reinterpret_cast<std::uint32_t*>(bucket) != 0U)                                                           // 0x004AAFDD/0x004AAFE0/0x004AAFE2: dec dword [eax]; jne 0x4AB0DF
    {
        return;
    }
    if (g_c321f0 != 0U)                                                                                              // 0x004AAFE8/0x004AAFED/0x004AAFEF: mov eax,[0xc321f0]; test eax,eax; je 0x4AB0D1
    {
        void* const pool = reinterpret_cast<void*>(static_cast<std::uintptr_t>(g_c321f0));
        ::VirtualFree(static_cast<std::uint8_t*>(*reinterpret_cast<void**>(static_cast<std::uint8_t*>(pool) + 0x0C)) +
                          (static_cast<std::uintptr_t>(g_c321e8) << 15),                                                     // 0x004AAFF5/0x004AB001/0x004AB004: shl ecx,0xf; add ecx,[eax+0xc]
                      0x8000, MEM_DECOMMIT);                                                                     // 0x004AB007/0x004AB00C/0x004AB011/0x004AB013: push 0x4000; push ebx(0x8000); call [KERNEL32.VirtualFree]
        (*reinterpret_cast<std::uint32_t*>(static_cast<std::uint8_t*>(pool) + 8)) |= 0x80000000U >> g_c321e8;       // 0x004AB01B/0x004AB025/0x004AB027: shr edx,cl; or [eax+8],edx
        std::uint32_t* const tables = *reinterpret_cast<std::uint32_t**>(static_cast<std::uint8_t*>(pool) + 0x10); // 0x004AB035: mov eax,[eax+0x10]
        tables[static_cast<std::uintptr_t>(g_c321e8) + 0xC4U / 4U] = 0U;                                             // 0x004AB038: and dword [eax+ecx*4+0xc4],0
        --*reinterpret_cast<std::uint8_t*>(reinterpret_cast<std::uint8_t*>(tables) + 0x43);                         // 0x004AB048: dec byte [eax+0x43]
        if (*reinterpret_cast<std::uint8_t*>(reinterpret_cast<std::uint8_t*>(tables) + 0x43) == 0U)                 // 0x004AB053/0x004AB057
        {
            *reinterpret_cast<std::uint32_t*>(static_cast<std::uint8_t*>(pool) + 4) &= 0xFFFFFFFEU;                 // 0x004AB059: and dword [eax+4],0xfffffffe
        }
        if (*reinterpret_cast<std::uint32_t*>(static_cast<std::uint8_t*>(pool) + 8) == 0xFFFFFFFFU)                // 0x004AB062/0x004AB066
        {
            ::VirtualFree(*reinterpret_cast<void**>(static_cast<std::uint8_t*>(pool) + 0x0C), 0, MEM_RELEASE);      // 0x004AB068/0x004AB069/0x004AB06B/0x004AB06E
            ::HeapFree(static_cast<HANDLE>(g_c32428), 0, *reinterpret_cast<void**>(static_cast<std::uint8_t*>(pool) + 0x10)); // 0x004AB075/0x004AB078/0x004AB07A/0x004AB080
            const std::uintptr_t tail = static_cast<std::uintptr_t>(g_c321f0) + 0x14U;
            const std::uintptr_t limit = reinterpret_cast<std::uintptr_t>(g_c321f8) +
                                         static_cast<std::uintptr_t>(g_c321f4) * 0x14U - 0x14U;                     // 0x004AB086..0x004AB0A0
            std::memmove(pool, reinterpret_cast<void*>(tail), limit - tail);                                    // 0x004AB0A4/0x004AB0A8/0x004AB0A9/0x004AB0AA: call 0x4acd70
            --g_c321f4;                                                                                              // 0x004AB0B5
            if (reinterpret_cast<std::uintptr_t>(g_c321f0) < reinterpret_cast<std::uintptr_t>(arena))               // 0x004AB0BB/0x004AB0C1/0x004AB0C3: cmp eax,[0xc321f0]; jbe
            {
                arena = static_cast<std::uint8_t*>(arena) - 0x14;                                                    // 0x004AB0C3: sub dword [ebp+8],0x14
            }
            g_c321ec = g_c321f8;                         // 0x004AB0C7/0x004AB0CC
        }
    }
    g_c321e8 = static_cast<std::int32_t>(segment);                                                                   // 0x004AB0D4: mov [0xc321e8],edi
    g_c321f0 = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(arena));                                  // 0x004AB0DA: mov [0xc321f0],eax
}

} // extern "C"
