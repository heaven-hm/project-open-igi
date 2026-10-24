// Native: FlashTask_Init (placeholder until VERIFIED); retail VA 0x00530A80-0x00530AC9 (74 bytes, 1 block, 0 edges).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; capture SHA256 f786e2e9796327e4514f9d1ff77809332bcc898f9e09dc8064f084d27b032ade.
// Tools/evidence: Binary Ninja 6.0.10601 decompile + radare2 6.2.0 disassembly (6a 00 6a 02 e8 67 0e ed ff ...); calls 0x004018F0, 0x00401900, 0x00401530 (x2); string "FlashTask" at 0x0054F7A4; written byte 0x00A845EC.
// Behavior/CFG: parent = QTask_GetCurrentType(); registers "FlashTask" (0x88 bytes) with task-list argument 2 -> observed id 39, stores the id at 0x0054F7A0, links phase 2 (0x00530C40) then phase 1 (0x00530AD0), sets the byte at 0x00A845EC to 1, returns the last link result. Single block.
// Status/limits: TRANSCRIBED (static only); the non-zero 4th TaskType_Register argument selects the retail task-list build path at 0x004B0B00 that the port does not transcribe yet (recorded in task_types.hpp limits); handlers passed as retail-VA literals; runtime UNOBSERVED in the port.
#include "igi/retail/core/task_types.hpp"

#include <cstdint>

extern "C" {
std::uint16_t g_54f7a0;  // retail 0x0054F7A0: FlashTask class id, written here
std::uint8_t  g_a845ec;  // retail 0x00A845EC: flag byte set to 1 here

std::int32_t FlashTask_Init()
{
    std::uint16_t const parent = QTask_GetCurrentType();          // 0x00530A84: call 0x4018F0
    g_54f7a0 = TaskType_Register("FlashTask", 0x88U, parent, 2U); // 0x00530A94: call 0x401900 (task-list argument 2)
    QTask_LinkQTaskEventHandlerNoContextFn(g_54f7a0, 2U, reinterpret_cast<igi::retail::core::FrameHandler>(0x00530C40)); // 0x00530AA7
    std::int32_t const result = static_cast<std::int32_t>(QTask_LinkQTaskEventHandlerNoContextFn(g_54f7a0, 1U, reinterpret_cast<igi::retail::core::FrameHandler>(0x00530AD0))); // 0x00530ABA
    g_a845ec = 1U;                                                // 0x00530AC2: mov byte [0xA845EC], 1
    return result;                                                // 0x00530AC9: ret
}
} // extern "C"
