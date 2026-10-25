// Native: QTask_InitPhaseHandlers (placeholder until VERIFIED); retail VA 0x004C5900-0x004C5A0E (271 bytes, 1 block, 0 edges).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 complete listing (73 instructions, single block); retail-only Frida range probe entered it as a real call from 0x531DB5 (return 0x531DBA) with 0 stack arguments, observed once during level-1 startup; both writers 0x4C5983/0x4C5992 fired inside it.
// Behavior/CFG: marks event slots 8..0xD with value 0, writes the phase/slot byte pairs (8,3) (9,4) (10,5) (11,6) (13,2) after (4,0) and (5,1), allocates two 0x181 events storing their ids at 0x548176 and 0x548177, links handler 0x004C5A10 to the phase at 0x54875C with the second event id as context, then zeroes the 17 dwords at 0xA44348.
// Status/limits: BUILD_VERIFIED (static only); this native OWNS the two state bytes 0x548176/0x548177 and the dword block at 0xA44348 that 0x4CEA10/0x4CEA20 and later readers reference extern; retail's cdecl cleanups (add esp,0x40 and add esp,0x3c) are the caller-side pops of the calls transcribed here; runtime UNOBSERVED.
#include "igi/retail/core/task_types.hpp"

#include <cstdint>

extern "C" {

// Native: TaskEventTable_MarkSlot; retail VA 0x00401860. Owned by native_4d9730.cpp.
void TaskEventTable_MarkSlot(std::uint16_t value, std::uint32_t index);

// Native: TaskType_SetPhaseHandlerByte; retail VA 0x00401D30. Owned by native_401d30.cpp.
void TaskType_SetPhaseHandlerByte(std::int32_t value, std::int32_t slot);

extern std::uint16_t g_54875c; // 0x0054875C, written outside this routine

std::uint8_t g_548176;         // 0x00548176, first allocated 0x181 event id
std::uint8_t g_548177;         // 0x00548177, second allocated 0x181 event id
std::uint32_t g_a44348[17];    // 0x00A44348..0x00A4438B, cleared here

void QTask_InitPhaseHandlers()
{
    TaskEventTable_MarkSlot(0x0000U, 8U);  // 0x004C5903: push 8; push esi(0); call 0x401860
    TaskEventTable_MarkSlot(0x0000U, 9U);  // 0x004C590B: push 9; push esi(0)
    TaskEventTable_MarkSlot(0x0000U, 10U); // 0x004C5913: push 0xa; push esi(0)
    TaskEventTable_MarkSlot(0x0000U, 11U); // 0x004C591B: push 0xb; push esi(0)
    TaskEventTable_MarkSlot(0x0000U, 12U); // 0x004C5923: push 0xc; push esi(0)
    TaskEventTable_MarkSlot(0x0000U, 13U); // 0x004C592B: push 0xd; push esi(0)

    TaskType_SetPhaseHandlerByte(4, 0);    // 0x004C5933: push esi(0); push 4
    TaskType_SetPhaseHandlerByte(5, 1);    // 0x004C593B: push 1; push 5
    // 0x004C5944: add esp,0x40 - cdecl cleanup of the eight calls above.

    TaskType_SetPhaseHandlerByte(8, 3);    // 0x004C5947: push 3; push 8
    TaskType_SetPhaseHandlerByte(9, 4);    // 0x004C5950: push 4; push 9
    TaskType_SetPhaseHandlerByte(10, 5);   // 0x004C5959: push 5; push 0xa
    TaskType_SetPhaseHandlerByte(11, 6);   // 0x004C5962: push 6; push 0xb
    TaskType_SetPhaseHandlerByte(13, 2);   // 0x004C596B: push 2; push 0xd

    g_548176 = QTask_AllocateEvent(0x0181U);      // 0x004C5974: push 0x181; call 0x4017C0; mov byte [0x548176], al
    g_548177 = QTask_AllocateEvent(0x0181U);      // 0x004C597E: push 0x181; call 0x4017C0; 0x4C5992: mov byte [0x548177], al
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(g_54875c),
                                static_cast<std::uint32_t>(g_548177),
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x004C5A10U))); // 0x004C598D/0x004C5997/0x004C599E: push handler; push event; push phase
    // 0x004C59A4: add esp,0x3c - cdecl cleanup of the stores/calls above.

    for (std::uint32_t index = 0U; index < 17U; ++index) // 0x004C59A7..0x004C5A07: 17 x mov dword [0xa44348+index*4], esi(0)
    {
        g_a44348[index] = 0U;
    }
}

} // extern "C"
