// Native: InputPort_Init_4ED510 (placeholder until VERIFIED); retail VA 0x004ED510-0x004ED556 (71 bytes, 1 block, 0 edges).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; capture SHA256 73ecb764d01f316904b4b523f529c2df0a70e20fc986fd9f954bd68f2d7a93da.
// Tools/evidence: Binary Ninja 6.0.10601 decompile + radare2 6.2.0 disassembly (56 33 f6 56 56 e8 ...); calls 0x004018F0, 0x00401900, 0x00401530; string "InputPort" at 0x0054DBD0.
// Behavior/CFG: zeroes ESI, parent = QTask_GetCurrentType(), registers "InputPort" (0x20 bytes) -> observed id 46, stores the id at 0x00A5EFA0, links phase 1 (0x004ED560) through 0x00401530, then zeroes the four dwords at 0x00BC20A0/0x00BC2150/0x00BC2200/0x00BC22B0, and returns the link result. Single block.
// Status/limits: TRANSCRIBED (static only); handler 0x004ED560 passed as a retail-VA literal; runtime UNOBSERVED in the port.
#include "igi/retail/core/task_types.hpp"

#include <cstdint>

extern "C" {

std::uint16_t g_a5efa0;    // retail 0x00A5EFA0: InputPort class id, written here
std::uint32_t g_bc20a0;    // retail 0x00BC20A0: zeroed here
std::uint32_t g_bc2150;    // retail 0x00BC2150: zeroed here
std::uint32_t g_bc2200;    // retail 0x00BC2200: zeroed here
std::uint32_t g_bc22b0;    // retail 0x00BC22B0: zeroed here

std::int32_t InputPort_Init_4ED510()
{
    std::uint32_t const zero = 0U;                                        // 0x004ED511: xor esi,esi
    std::uint16_t const parent = QTask_GetCurrentType();                  // 0x004ED515: call 0x4018F0
    g_a5efa0 = TaskType_Register("InputPort", 0x20U, parent, 0U);         // 0x004ED522: call 0x401900
    std::int32_t const result = static_cast<std::int32_t>(QTask_LinkQTaskEventHandlerNoContextFn(g_a5efa0, 1U, reinterpret_cast<igi::retail::core::FrameHandler>(0x004ED560))); // 0x004ED535
    g_bc20a0 = zero;                                                      // 0x004ED53D: mov [0xBC20A0], esi
    g_bc2150 = zero;                                                      // 0x004ED543
    g_bc2200 = zero;                                                      // 0x004ED549
    g_bc22b0 = zero;                                                      // 0x004ED54F
    return result;                                                        // 0x004ED555: pop esi; ret
}

} // extern "C"
