// Native: Mem_FreeBucketNode (placeholder until VERIFIED); retail VA 0x004AB6DD-0x004AB732 (86 bytes, 5 blocks, 5 edges).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 complete listing (56 8b742408 6800800000 6a00 ff7610 ff15bc305300 3935d0665400 7508 8b4604 a3d0665400 81feb0465400 7420 8b4604 8b0e 56 6a00 8908 8b06 8b4e04 894804 ff352824c300 ff15a4305300 5e c3 830dc04654..ff 5e); imports KERNEL32 VirtualFree and HeapFree.
// Behavior/CFG: VirtualFree([node+0x10], 0, MEM_RELEASE) releases the node's reservation; if the remembered node 0x005466D0 is this node it remembers [node+0x04] instead; for a real node it unlinks it from the circular list ([next]=prev and [prev+4]=next) and HeapFree([0xC32428], 0, node); for the 0x005446B0 sentinel it only writes -1 to the sentinel's +0x10 bound word.
// Status/limits: TRANSCRIBED (static only); the sentinel branch writes 0x005446C0, which is sentinel + 0x10 (the bound word), not a separate global; the KERNEL32 calls are retail's own imports reached through ::VirtualFree/::HeapFree; runtime UNOBSERVED and unreachable while the heap mode is not 2.
#include "igi/retail/mem/mem_alloc.hpp"
#include "igi/retail/mem/mode2_bucket.hpp"

#include <cstdint>
#include <windows.h>

using igi::retail::mem::g_5446b0;
using igi::retail::mem::g_5466d0;
using igi::retail::mem::Mode2BucketNode;

extern "C" {

void Mem_FreeBucketNode(Mode2BucketNode* const node)
{
    ::VirtualFree(node->base_10, 0, MEM_RELEASE);                       // 0x004AB6E2..0x004AB6EC: push 0x8000; push 0; push [esi+0x10]; call [KERNEL32.VirtualFree]
    if (g_5466d0 == node)                                               // 0x004AB6F2: cmp [0x5466d0], esi; jne
    {
        g_5466d0 = node->next;                                          // 0x004AB6FA/0x004AB6FD: mov eax,[esi+4]; mov [0x5466d0],eax
    }
    if (node != &g_5446b0)                                              // 0x004AB702: cmp esi,0x5446b0; je
    {
        node->next->prev = node->prev;                                  // 0x004AB70A/0x004AB70D/0x004AB712: eax=next; ecx=prev; mov [eax],ecx
        node->prev->next = node->next;                                  // 0x004AB714/0x004AB716/0x004AB719: eax=prev; ecx=next; mov [eax+4],ecx
        ::HeapFree(static_cast<HANDLE>(igi::retail::mem::g_c32428), 0, node); // 0x004AB71C/0x004AB722: push esi; push 0; push [0xc32428]; call [KERNEL32.HeapFree]
    }
    else
    {
        g_5446b0.base_10 = reinterpret_cast<void*>(static_cast<std::uintptr_t>(0xFFFFFFFFU)); // 0x004AB72A: or dword [0x5446c0], 0xffffffff
    }
}

} // extern "C"
