// Native: QTask_NewQTaskEventNoContextFn (retail BN name; keep until VERIFIED); retail VA 0x00401810-0x0040185A (74 bytes, 6 blocks).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP disassembly/decompile + radare2 6.2.0 bytes; single callee 0x004AF7B0 Debug_Error; event table image 0x0054F930 all zero.
// Behavior/CFG: scans the 6-byte event table from the first allocatable id while the in-use flag at record+2 is set; on a free slot stores the 16-bit argument at +0, sets +2=1 and +3=0, returns the event id; table exhaustion calls Debug_Error("Unable to allocate event") and spins.
// Status/limits: TRANSCRIBED; QTask_AllocateEvent (0x004017C0) has the same table scan and is the reference for the scan bounds; runtime UNOBSERVED.
#include "igi/retail/core/retail_error.hpp"
#include "igi/retail/core/task_types.hpp"

#include <cstdint>

extern "C" {

std::uint8_t QTask_NewQTaskEventNoContextFn(std::uint16_t value)
{
    // 0x00401810-0x00401825: scan from the first id while the flag byte at +2 is set.
    std::uint32_t event_id = igi::retail::core::first_event_id;
    while (event_id < igi::retail::core::event_table_entry_count &&
           QTask_EventTable[event_id * 6U + 2U] != 0U)
    {
        ++event_id;
    }
    if (event_id >= igi::retail::core::event_table_entry_count)
    {
        // 0x0040183A-0x0040184F: push "Unable to allocate event"; call Debug_Error; jmp $.
        Debug_Error("Unable to allocate event");
        for (;;)
        {
        }
    }
    // 0x00401826-0x00401839: word store at +0, in-use=1, pad=0.
    const std::size_t entry = static_cast<std::size_t>(event_id) * 6U;
    QTask_EventTable[entry] = static_cast<std::uint8_t>(value & 0xFFU);
    QTask_EventTable[entry + 1U] = static_cast<std::uint8_t>((value >> 8) & 0xFFU);
    QTask_EventTable[entry + 2U] = 1U;
    QTask_EventTable[entry + 3U] = 0U;
    return static_cast<std::uint8_t>(event_id);
}

} // extern "C"
