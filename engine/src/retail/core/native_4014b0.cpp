// Native: TaskType_RefreshHandlerMask (placeholder name until VERIFIED; retail BN alias QTask_DeleteTask is wrong - nothing is deleted).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004014B0-0x004014DA (43 bytes).
// Tools/evidence: Binary Ninja MCP + radare2 6.2.0; 5 blocks, 1 callee (0x004014E0).
// Behavior/CFG: walks the 384 0x30-byte type slots at 0x00AF5EE0; when slot+0x25 is non-zero, stores the 16-bit mask from 0x004014E0 at slot+0x28. No other stores.
// Status/limits: TRANSCRIBED; returns the last callee eax (unused by retail callers); mask values depend on 0x0054FF24 (see native_4014e0.cpp).
#include "igi/retail/core/task_types.hpp"
#include <cstdint>

extern "C" {

std::int32_t TaskType_HandlerPhaseMask(std::int16_t type_id);

void TaskType_RefreshHandlerMask()
{
    for (std::uint32_t type_id = 0U; type_id < igi::retail::core::task_type_slot_count; ++type_id)
    {
        if (::QTask_TypeTable[type_id].in_use != 0U)
        {
            ::QTask_TypeTable[type_id].handler_mask =
                static_cast<std::uint16_t>(TaskType_HandlerPhaseMask(static_cast<std::int16_t>(type_id)));
        }
    }
}

} // extern "C"
