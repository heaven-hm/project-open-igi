// Native: TaskEventTable_MarkSlotNoMask (placeholder until VERIFIED); retail VA 0x00401890-0x004018B3 (36 bytes, 1 block, 0 edges).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 bytes/disassembly (8b442408 668b4c2404 8d0440 d1e0 c68032f954000 1 66898830f9 5400 c68033f9540000 c3).
// Behavior/CFG: index = arg2 * 6 (NO 0xFF mask, unlike 0x401860); stores used byte 1 at +2, the 16-bit arg1 at +0, then byte 0 at +3; void return. Single block, no callees.
// Status/limits: BUILD_VERIFIED (static only); sibling of 0x401860 with two retail differences preserved deliberately: no index mask and the +3 byte written as 0 instead of 1; runtime UNOBSERVED.
#include "igi/retail/core/task_types.hpp"

#include <cstdint>

extern "C" {

void TaskEventTable_MarkSlotNoMask(std::uint16_t const value, std::uint32_t const index)
{
    std::uint32_t const offset = index * 6U;            // 0x00401899: lea; shl (no and 0xFF here)
    QTask_EventTable[offset + 2U] = 1U;                 // 0x0040189E: mov byte [eax+0x54F932],1
    *reinterpret_cast<std::uint16_t*>(QTask_EventTable + offset) = value; // 0x004018A5: mov word [eax+0x54F930],cx
    QTask_EventTable[offset + 3U] = 0U;                 // 0x004018AC: mov byte [eax+0x54F933],0
}

} // extern "C"
