// Native: TaskEventTable_MarkSlot (placeholder until VERIFIED); retail VA 0x00401860-0x00401888 (41 bytes, 1 block, 0 edges).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 bytes/disassembly (8b442408 668b542404 25ff000000 b101 8d0440 d1e0 888832f95400 66899030f9 888833f95400 c3) + DINPUT/GLOBAL-548638 packet cross-check (stride 6, fields +0/+2/+3).
// Behavior/CFG: index = (arg2 & 0xFF) * 6 into QTask_EventTable (retail 0x0054F930); stores used flag 1 at +2, the 16-bit arg1 at +0, then used flag 1 at +3; returns void. Single block, no callees.
// Status/limits: TRANSCRIBED (static only); runtime UNOBSERVED.
#include "igi/retail/core/task_types.hpp"

#include <cstdint>

extern "C" {

void TaskEventTable_MarkSlot(std::uint16_t const value, std::uint32_t const index)
{
    std::uint32_t const offset = (index & 0xFFU) * 6U;                  // 0x00401869: and eax,0xFF; lea; shl
    QTask_EventTable[offset + 2U] = 1U;                                 // 0x00401875: mov byte [eax+0x54F932],cl
    *reinterpret_cast<std::uint16_t*>(QTask_EventTable + offset) = value; // 0x0040187B: mov word [eax+0x54F930],dx
    QTask_EventTable[offset + 3U] = 1U;                                 // 0x00401882: mov byte [eax+0x54F933],cl
}

} // extern "C"
