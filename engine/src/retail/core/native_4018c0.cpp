// Native: TaskEventTable_ClearSlot (placeholder until VERIFIED); retail VA 0x004018C0-0x004018D4 (21 bytes, 1 block, 0 edges).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 bytes/disassembly (8b442404 25ff000000 8d0440 c6044532f9 540000 c3).
// Behavior/CFG: index = (arg & 0xFF) * 6 into QTask_EventTable (retail 0x0054F930) and clears the used byte at +2; void return. Single block, no callees.
// Status/limits: TRANSCRIBED (static only); the counterpart of 0x401860's used-flag write (there +2 := 1, here +2 := 0); runtime UNOBSERVED.
#include "igi/retail/core/task_types.hpp"

#include <cstdint>

extern "C" {

void TaskEventTable_ClearSlot(std::uint32_t const index)
{
    std::uint32_t const offset = (index & 0xFFU) * 6U;   // 0x004018C4: and eax,0xFF; lea; then *2 in the store
    QTask_EventTable[offset + 2U] = 0U;                  // 0x004018CC: mov byte [eax*2+0x54F932],0
}

} // extern "C"
