// Native: TaskType_HandlerPhaseMask (placeholder name until VERIFIED; retail BN alias QTask_ChangeParent is wrong - it builds a phase mask).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004014E0-0x0040152A (75 bytes).
// Tools/evidence: Binary Ninja MCP disassembly/decompile + radare2 6.2.0 bytes; 0 callees; reads g_54ff24[16] and QTask_HandlerTable.
// Behavior/CFG: for i=0..15, phase=g_54ff24[i]; when phase != 0xFF set bit i if QTask_HandlerTable[phase*0x180 + (type_id&0xFFFF)] != null; then set bit 15 (or ah,0x80) and return the mask in eax.
// Status/limits: TRANSCRIBED; mask values are not retail-equitable until the 0x0054FF24 writers 0x00401400/0x00401530 are transcribed (zero placeholder makes every mask 0x8000).
#include "igi/retail/core/task_types.hpp"
#include <cstdint>

extern "C" {

std::int32_t TaskType_HandlerPhaseMask(std::int16_t type_id)
{
    std::int32_t mask = 0;
    for (std::int32_t i = 0; i < 0x10; ++i)
    {
        const std::uint8_t phase = g_54ff24[i];
        if (phase != 0xFFU)
        {
            const std::uint32_t index =
                static_cast<std::uint32_t>(phase) * igi::retail::core::task_phase_stride +
                (static_cast<std::uint32_t>(type_id) & 0xFFFFU);
            if (::QTask_HandlerTable[index] != nullptr)
            {
                mask |= 1 << i;
            }
        }
    }
    mask |= 0x8000;
    return mask;
}

} // extern "C"
