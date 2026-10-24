// Native: SoundSysMicrophone_Init (placeholder until VERIFIED); retail VA 0x004EBC60-0x004EBCC6 (103 bytes, 1 block, 0 edges).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; capture SHA256 0b19afa8e3bc59f6740af55af2f060aa44a811ee2e679429f9adfc40e22b64e3.
// Tools/evidence: Binary Ninja 6.0.10601 decompile + radare2 6.2.0 disassembly (6a 00 6a 00 e8 ...); calls 0x004018F0, 0x00401900, 0x004017C0, 0x00401400, 0x00401530 (x2); string "SoundSysMicrophone" at 0x0054DB6C.
// Behavior/CFG: parent = QTask_GetCurrentType(); registers "SoundSysMicrophone" (0x68 bytes) -> observed id 23, stores the id at 0x0054DB68, stores QTask_AllocateEvent(0) at 0x0054DB6A, links phase 0 (0x004D3710) through 0x00401400 and phases 1/2 (0x004EBCD0, 0x004D3710) through 0x00401530, returning the last result. Single block.
// Status/limits: TRANSCRIBED (static only); handler natives 0x004D3710 and 0x004EBCD0 remain untranscribed and are passed as retail-VA literals; runtime UNOBSERVED in the port.
#include "igi/retail/core/task_types.hpp"

#include <cstdint>

extern "C" {
std::uint16_t g_54db68;  // retail 0x0054DB68: SoundSysMicrophone class id, written here
std::uint8_t  g_54db6a;  // retail 0x0054DB6A: QTask_AllocateEvent result, written here

std::int32_t SoundSysMicrophone_Init()
{
    std::uint16_t const parent = QTask_GetCurrentType();                   // 0x004EBC64: call 0x4018F0
    g_54db68 = TaskType_Register("SoundSysMicrophone", 0x68U, parent, 0U); // 0x004EBC71: call 0x401900
    g_54db6a = QTask_AllocateEvent(0U);                                    // 0x004EBC7E: call 0x4017C0
    QTask_LinkQTaskEventHandler(g_54db68, 0U, reinterpret_cast<igi::retail::core::FrameHandler>(0x004D3710)); // 0x004EBC96
    QTask_LinkQTaskEventHandlerNoContextFn(g_54db68, 1U, reinterpret_cast<igi::retail::core::FrameHandler>(0x004EBCD0)); // 0x004EBCAA
    return static_cast<std::int32_t>(QTask_LinkQTaskEventHandlerNoContextFn(g_54db68, 2U, reinterpret_cast<igi::retail::core::FrameHandler>(0x004D3710))); // 0x004EBCBE
}
} // extern "C"
