// Native: DynCubeObj_Init (placeholder until VERIFIED); retail VA 0x004E5370-0x004E539F (48 bytes, 1 block, 0 edges).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; capture SHA256 a037d19f3e9618e40d1da0fbd9226e3ee048d6ad2becee76eef358303dcaf2d1.
// Tools/evidence: Binary Ninja 6.0.10601 decompile + radare2 6.2.0 disassembly (66 a1 38 86 54 00 ...); calls 0x00401900, 0x004017C0; string "DynCubeObj" at 0x00548760.
// Behavior/CFG: reads the 16-bit parent at 0x00548638, registers "DynCubeObj" (0x68 bytes) -> observed class id 2, stores the id word at 0x0054875C, runs QTask_AllocateEvent(0x181), stores its AL result at 0x0054875E and returns that result in EAX. Single block, no branches.
// Status/limits: BUILD_VERIFIED (static only); runtime UNOBSERVED in the port; observed id 2 in the position-1 capture. ABI: retail loads the parent with `mov ax,[0x548638]`, so the pushed EAX upper bits are caller residue; the port passes a 16-bit value, which is all TaskType_Register stores.
#include "igi/retail/core/task_types.hpp"

#include <cstdint>

extern "C" {
extern std::uint16_t g_548638;  // retail 0x00548638: QObj class id, owned by 0x4D9730 (native_4d9730.cpp)
std::uint16_t g_54875c;  // retail 0x0054875C: DynCubeObj class id, written here
std::uint8_t g_54875e;   // retail 0x0054875E: QTask_AllocateEvent result, written here

std::int32_t DynCubeObj_Init()
{
    g_54875c = TaskType_Register("DynCubeObj", 0x68U, g_548638, 0U); // 0x004E537D: push 0x548760; push 0x68; push eax; push 0; push 0
    std::uint8_t const event = QTask_AllocateEvent(0x181U);          // 0x004E5387: push 0x181; call 0x4017C0
    g_54875e = event;                                                // 0x004E539A: mov byte [0x54875E], al
    return static_cast<std::int32_t>(event);                         // 0x004E539F: ret (EAX = allocate result)
}
} // extern "C"
