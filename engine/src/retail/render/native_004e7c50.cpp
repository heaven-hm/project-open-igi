// Native: CameraControlTask_Init (placeholder until VERIFIED); retail VA 0x004E7C50-0x004E7C8F (64 bytes, 1 block, 0 edges).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; capture SHA256 b3bae5933dbb03a42cb443672f3a8d85f65eca84fa790540003b7890a51eba74.
// Tools/evidence: Binary Ninja 6.0.10601 decompile + radare2 6.2.0 disassembly (6a 00 6a 00 e8 97 9c f1 ff ...); calls 0x004018F0, 0x00401900, 0x00401530 (x2); string "CameraControlTask" at 0x0054D978.
// Behavior/CFG: parent = QTask_GetCurrentType(); registers "CameraControlTask" (0x2C bytes) -> observed id 31, stores the id at 0x0054D974, links phase 1 (0x004E7C90) and phase 2 (0x004D3710) through 0x00401530, returns the last result. Single block.
// Status/limits: BUILD_VERIFIED (static only); the two handlers are passed as retail-VA literals until transcribed; runtime UNOBSERVED in the port.
#include "igi/retail/core/task_types.hpp"

#include <cstdint>

extern "C" {
std::uint16_t g_54d974= 0x181U;  // retail 0x0054D974: CameraControlTask class id, written here

std::int32_t CameraControlTask_Init()
{
    std::uint16_t const parent = QTask_GetCurrentType();                  // 0x004E7C54: call 0x4018F0
    g_54d974 = TaskType_Register("CameraControlTask", 0x2CU, parent, 0U); // 0x004E7C61: call 0x401900
    QTask_LinkQTaskEventHandlerNoContextFn(g_54d974, 1U, reinterpret_cast<igi::retail::core::FrameHandler>(0x004E7C90)); // 0x004E7C74
    return static_cast<std::int32_t>(QTask_LinkQTaskEventHandlerNoContextFn(g_54d974, 2U, reinterpret_cast<igi::retail::core::FrameHandler>(0x004D3710))); // 0x004E7C87
}
} // extern "C"
