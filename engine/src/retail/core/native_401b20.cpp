// Native: QHashSlotTable_Release (placeholder until VERIFIED); retail VA 0x00401B20-0x00401B81 (98 bytes, 4 blocks, 4 edges).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 complete listing (8b4c2404 8b01 85c0 7403 ff4818 a1407c5600 56 8b354c7c5600 48 8b0c0e a3407c5600 3bc8 743a 8b153c7c5600 53 57 8b3d387c5600 8b0482 0faf05487c56.. 03c7 8b3c8a 8b3406 8b1cb2 891c8a 8b153c7c5600 893cb2 8b154c7c5600 5f 5b 890c02 5e c3).
// Behavior/CFG: releases a slot in the QHash slot table. The argument is the slot address returned by the insert counterpart 0x00401AE0 (its callers store it in the object and hand it back here), not the object pointer. It decrements the count word at [slot] + 0x18 when that pointer is non-null, decrements the live count 0x00567C40, reads this slot's link from the link table 0x00567C4C indexed by the slot address itself, and returns when that link already equals the new count. Otherwise it swaps the index entries of the released slot and the moved link (index table 0x00567C3C, stride 0x00567C48, base 0x00567C38) and rewrites the link table so the moved slot points at the released one.
// Status/limits: TRANSCRIBED (static only); the link table is indexed by the slot address exactly as retail does ([esi+ecx] with scale 1), which is preserved rather than normalised: the insert side writes [links + slot_address] with the same scale, so normalising to [esi+ecx*4] would be wrong; the globals are zero-initialised storage while the insert counterpart at 0x00401Axx remains untranscribed; runtime UNOBSERVED.
#include "igi/retail/core/qhash_slot_table.hpp"

#include <cstdint>

using igi::retail::core::g_567c38;
using igi::retail::core::g_567c3c;
using igi::retail::core::g_567c40;
using igi::retail::core::g_567c48;
using igi::retail::core::g_567c4c;

extern "C" {

void QHashSlotTable_Release(void* const slot_address)
{
    void* const slot = *reinterpret_cast<void**>(slot_address);                                  // 0x00401B24: mov eax,[ecx]
    if (slot != nullptr)                                                                        // 0x00401B26/0x00401B28: test eax,eax; je
    {
        --*reinterpret_cast<std::int32_t*>(static_cast<std::uint8_t*>(slot) + 0x18);            // 0x00401B2A: dec dword [eax+0x18]
    }
    std::uint32_t count = g_567c40;                                                             // 0x00401B2D: mov eax,[0x567c40]
    std::uint8_t* const links = reinterpret_cast<std::uint8_t*>(static_cast<std::uintptr_t>(g_567c4c)); // 0x00401B33: mov esi,[0x567c4c]
    --count;                                                                                    // 0x00401B39: dec eax
    const std::uint32_t next = *reinterpret_cast<std::uint32_t*>(links + reinterpret_cast<std::uintptr_t>(slot_address)); // 0x00401B3A: mov ecx,[esi+ecx]
    g_567c40 = count;                                                                           // 0x00401B3D: mov [0x567c40],eax
    if (next == count)                                                                          // 0x00401B42/0x00401B44: cmp ecx,eax; je
    {
        return;                                                                                 // 0x00401B80: pop esi; ret
    }
    std::uint32_t* const index = reinterpret_cast<std::uint32_t*>(static_cast<std::uintptr_t>(g_567c3c)); // 0x00401B46: mov edx,[0x567c3c]
    const std::uint32_t element_base = g_567c38;                                                // 0x00401B4E: mov edi,[0x567c38]
    std::uint32_t offset = index[count];                                                        // 0x00401B54: mov eax,[edx+eax*4]
    offset *= g_567c48;                                                                         // 0x00401B57: imul eax,[0x567c48]
    offset += element_base;                                                                     // 0x00401B5E: add eax,edi
    const std::uint32_t saved = index[next];                                                    // 0x00401B60: mov edi,[edx+ecx*4]
    const std::uint32_t link = *reinterpret_cast<std::uint32_t*>(links + offset);               // 0x00401B63: mov esi,[esi+eax]
    const std::uint32_t moved = index[link];                                                    // 0x00401B66: mov ebx,[edx+esi*4]
    index[next] = moved;                                                                        // 0x00401B69: mov [edx+ecx*4],ebx
    index[link] = saved;                                                                        // 0x00401B72: mov [edx+esi*4],edi
    *reinterpret_cast<std::uint32_t*>(links + offset) = next;                                   // 0x00401B7D: mov [edx+eax],ecx (edx reloaded from 0x00567C4C)
}

} // extern "C"
