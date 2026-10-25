// Native: FreeCamera_Init (placeholder until VERIFIED); retail VA 0x005310E0-0x00531122 (67 bytes, 1 block, 0 edges).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; capture SHA256 099189b718a252a62b5cbe0a2d2664e7f55ddcf09f20bca452431bd2fb36f0e7.
// Tools/evidence: Binary Ninja 6.0.10601 decompile + radare2 6.2.0 disassembly (6a 00 6a 00 e8 07 08 ed ff ...); calls 0x004018F0, 0x00401900, 0x00401530 (x2); string "FreeCamera" at 0x0054F7D4.
// Behavior/CFG: parent = QTask_GetCurrentType(); registers "FreeCamera" (0x90 bytes) -> observed id 32, stores the id at 0x0054F7D0, links phase 1 (0x00531130) and phase 2 (0x004D3710) through 0x00401530, returns the last result. Single block.
// Status/limits: BUILD_VERIFIED (static only); handlers passed as retail-VA literals until transcribed; runtime UNOBSERVED in the port.
#include "igi/retail/core/task_types.hpp"

#include <cstdint>

extern "C" {
std::uint16_t g_54f7d0;  // retail 0x0054F7D0: FreeCamera class id, written here

std::int32_t FreeCamera_Init()
{
    std::uint16_t const parent = QTask_GetCurrentType();           // 0x005310E4: call 0x4018F0
    g_54f7d0 = TaskType_Register("FreeCamera", 0x90U, parent, 0U); // 0x005310F4: call 0x401900
    QTask_LinkQTaskEventHandlerNoContextFn(g_54f7d0, 1U, reinterpret_cast<igi::retail::core::FrameHandler>(0x00531130)); // 0x00531107
    return static_cast<std::int32_t>(QTask_LinkQTaskEventHandlerNoContextFn(g_54f7d0, 2U, reinterpret_cast<igi::retail::core::FrameHandler>(0x004D3710))); // 0x0053111A
}
} // extern "C"
