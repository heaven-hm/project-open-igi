// Native: AI_Detection_Init; retail VA 0x00489560. previous note: Native: AI_Detection_Init (placeholder name until VERIFIED; BN alias FX_SmokeEmit_12 is unreliable for a registrar).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x00489560-0x004895C8 (104 bytes, 1 block).
// Tools/evidence: Binary Ninja MCP disassembly/decompile + radare2 6.2.0 bytes; 4 calls, no branches, no strings; retail registration capture has no TaskType_Register in this position.
// Behavior/CFG: g_5c8ac0 = QTaskList_New(0xC0); clears 0x005C8B0C/10/14; rep stosd fills 16 dwords 0x01810181 at 0x005C8AC4; then three QTask_AllocateEvent(0) results are stored as bytes at 0x0054127C/7D/7E; returns the last event result.
// Status/limits: BUILD_VERIFIED; QTaskList_New (0x004C1800) and QTask_AllocateEvent (0x004017C0) are transcribed natives; runtime UNOBSERVED (registration runs after the QTask-entry boundary).
#include "igi/retail/core/task_types.hpp"
#include "igi/retail/core/qtask_list.hpp"

#include <cstdint>

std::uint32_t g_5c8ac0 = 0U;      // retail 0x005C8AC0
std::uint32_t g_5c8ac4[16] = {};  // retail 0x005C8AC4
std::uint32_t g_5c8b0c = 0U;      // retail 0x005C8B0C
std::uint32_t g_5c8b10 = 0U;      // retail 0x005C8B10
std::uint32_t g_5c8b14 = 0U;      // retail 0x005C8B14
std::uint8_t g_54127c = 0U;       // retail 0x0054127C
std::uint8_t g_54127d = 0U;       // retail 0x0054127D
std::uint8_t g_54127e = 0U;       // retail 0x0054127E

extern "C" {

std::int32_t AI_Detection_Init()
{
    g_5c8b0c = 0U;
    g_5c8b10 = 0U;
    g_5c8b14 = 0U;
    g_5c8ac0 = static_cast<std::uint32_t>(
        reinterpret_cast<std::uintptr_t>(QTaskList_New(0xC0)));
    // 0x004895A6-0x004895B4: rep stosd, ECX=16, EAX=0x01810181, EDI=0x005C8AC4.
    for (std::uint32_t i = 0U; i < 16U; ++i)
    {
        g_5c8ac4[i] = 0x01810181U;
    }
    g_54127c = QTask_AllocateEvent(0);
    g_54127d = QTask_AllocateEvent(0);
    const std::int32_t result = static_cast<std::int32_t>(QTask_AllocateEvent(0));
    g_54127e = static_cast<std::uint8_t>(result);
    return result;
}

} // extern "C"
