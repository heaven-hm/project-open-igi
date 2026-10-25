// Native: Hazinglayers_Init (placeholder until VERIFIED); retail VA 0x004DA1D0-0x004DA23B (108 bytes, 1 block, 0 edges).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; capture SHA256 82a328c5d6e36ab15015e1b44e3fb02f7277dbde4343885d257b6b739132a1fc.
// Tools/evidence: Binary Ninja 6.0.10601 decompile + radare2 6.2.0 disassembly (66 a1 38 86 54 00 ...); calls 0x00401900 and 0x00401400 (x4); string "Hazinglayers" at 0x0054866C.
// Behavior/CFG: registers "Hazinglayers" (0x90 bytes) with the 16-bit parent at 0x00548638 -> observed id 3, stores the id at 0x005AE304, then links four phase handlers (0x11/0x12/4/5 -> 0x004DA240/0x004DA2A0/0x004DA270/0x004DA2C0) through 0x00401400 and returns the last link result. Single block.
// Status/limits: BUILD_VERIFIED (static only); the four handlers are not transcribed yet, so their addresses are passed as retail-VA literals (the exact retail push immediates); runtime UNOBSERVED in the port.
#include "igi/retail/core/task_types.hpp"

#include <cstdint>

extern "C" {
extern std::uint16_t g_548638;  // retail 0x00548638: parent class id, written by QObj_Init 0x004D9730 (native_4d9730.cpp); read-only here
std::uint16_t g_a5e304;  // retail 0x005AE304: Hazinglayers class id, written here

std::int32_t Hazinglayers_Init()
{
    g_a5e304 = TaskType_Register("Hazinglayers", 0x90U, g_548638, 0U);  // 0x004DA1E5: call 0x401900
    QTask_LinkQTaskEventHandler(g_a5e304, 0x11U, reinterpret_cast<igi::retail::core::FrameHandler>(0x004DA240)); // 0x004DA1F8: push 0x4DA240; push 0x11; push eax
    QTask_LinkQTaskEventHandler(g_a5e304, 0x12U, reinterpret_cast<igi::retail::core::FrameHandler>(0x004DA2A0)); // 0x004DA20C
    QTask_LinkQTaskEventHandler(g_a5e304, 4U, reinterpret_cast<igi::retail::core::FrameHandler>(0x004DA270));    // 0x004DA220
    return static_cast<std::int32_t>(QTask_LinkQTaskEventHandler(g_a5e304, 5U, reinterpret_cast<igi::retail::core::FrameHandler>(0x004DA2C0))); // 0x004DA233
}
} // extern "C"
