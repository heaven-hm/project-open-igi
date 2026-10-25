// Native: QObj_Init (placeholder until VERIFIED); retail VA 0x004D9730-0x004D9782 (83 bytes, 1 block, 0 edges).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 disassembly (6a04 6a00 e82781f2ff ... 66a338865400 c3); GLOBAL-548638 packet (QObj bytes 51 4F 62 6A 00 at 0x0054863C).
// Behavior/CFG: marks event-table slots for phases 4..7 with value 0 via 0x401860, allocates event 0 (0x4017C0) and stores its byte at 0x0054863A, then registers "QObj" (0x50) with parent QTask_GetCurrentType() and arg4 0, storing the returned id at 0x00548638, and returns it. Single block.
// Status/limits: STRUCTURALLY_VERIFIED (static only); retail pushes one stray extra cdecl dword at the 0x401900 site (5 pushes, add esp,0x14) that the callee never reads - documented benign stack-traffic divergence, the four meaningful arguments are reproduced. Runtime UNOBSERVED.
#include "igi/retail/core/task_types.hpp"

#include <cstdint>

extern "C" {

// Native: TaskEventTable_MarkSlot; retail VA 0x00401860.
void TaskEventTable_MarkSlot(std::uint16_t value, std::uint32_t index);

std::uint16_t g_548638;    // retail 0x00548638: QObj class id, written here
std::uint8_t g_54863a;     // retail 0x0054863A: byte result of the 0x4017C0 call

std::int32_t QObj_Init()
{
    TaskEventTable_MarkSlot(0x0000U, 4U);                          // 0x004D9730: push 4; push 0; call 0x401860
    TaskEventTable_MarkSlot(0x0000U, 5U);                          // 0x004D9739: push 5; push 0; call 0x401860
    TaskEventTable_MarkSlot(0x0000U, 6U);                          // 0x004D9742: push 6; push 0; call 0x401860
    TaskEventTable_MarkSlot(0x0000U, 7U);                          // 0x004D974B: push 7; push 0; call 0x401860
    g_54863a = QTask_AllocateEvent(0x0000U);                              // 0x004D9754: push 0; call 0x4017C0; add esp,0x24
    g_548638 = TaskType_Register("QObj", 0x50U, QTask_GetCurrentType(), 0U); // 0x004D9763..0x004D9774 (stray extra push)
    return g_548638;                                                      // 0x004D977C: mov [0x548638],ax; ret
}

} // extern "C"
