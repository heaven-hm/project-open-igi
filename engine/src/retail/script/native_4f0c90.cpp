// Native: QHashNode_Release_4F0C90 (placeholder until VERIFIED); retail VA 0x004F0C90-0x004F0CAE (31 bytes, 1 block, 3 edges).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 complete listing (56 8b742408 8b4608 50 e8820ef1ff 56 e8bcecfbff 56 e86600fcff 83c40c 5e c3); calls 0x401B20, 0x4AF960 and 0x4B0D10.
// Behavior/CFG: releases one QHash node: withdraws its slot from the slot table 0x401B20 using the node word at +0x08, unlinks it from the heap list with 0x4AF960, and returns its storage through the deallocation entry 0x4B0D10.
// Status/limits: TRANSCRIBED (static only); the three calls are in retail's order and the last two receive the node itself; runtime UNOBSERVED.
#include "igi/retail/mem/heap_list.hpp"

#include <cstdint>

using igi::retail::mem::HeapList_Remove;

extern "C" {

void QHashSlotTable_Release_401B20(void* context); // 0x004F0C99: call 0x401b20
void Mem_DeAllocFn_4B0D10(void* pointer);          // 0x004F0CA5: call 0x4b0d10

void QHashNode_Release_4F0C90(void* const node)
{
    QHashSlotTable_Release_401B20(*reinterpret_cast<void**>(static_cast<std::uint8_t*>(node) + 0x08)); // 0x004F0C95/0x004F0C98/0x004F0C99: mov eax,[esi+8]; push eax; call
    HeapList_Remove(static_cast<void**>(node));                                      // 0x004F0C9E/0x004F0C9F: push esi; call 0x4af960
    Mem_DeAllocFn_4B0D10(node);                                                                        // 0x004F0CA4/0x004F0CA5: push esi; call 0x4b0d10
}

} // extern "C"
