// Native: WinProc_RegisterHandler; retail VA 0x0048F490. previous note: Native: WinProc_RegisterHandler (placeholder name until VERIFIED; retail BN alias FX_FailedToRegisterWinp)
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x0048F490-0x0048F4EA.
// Tools/evidence: Binary Ninja MCP disassembly/decompile + radare2 bytes; 6 basic blocks; spec build/audit/main-0x48F530/level1-closure/20261024T022900Z/TRANSCRIPTION-SPECS.md §1.5.
// Behavior/CFG: scans the 16-node pool for a node whose handler (+0x08) is null, fills handler/prev_link/next and appends it to the WinProc chain via Sys_InsertListTail_4AF8F0(container=0x005C8DF0, node); pool-full path calls Debug_Error("Failed to register WinProc callback.") and jumps forever (0x48F4B4).
// Status/limits: BUILD_VERIFIED; depends on HeapList_Insert 0x004AF8F0; pool scan counts 16 iterations instead of retail's address bound (pool not adjacent to the container in the candidate).
#include "igi/retail/core/native_0048f970.hpp"
#include "igi/retail/core/retail_error.hpp"
#include <cstdint>

extern "C" void* Sys_InsertListTail_4AF8F0(void* head, void* node);   // 0x004AF8F0, engine/src/retail/sys/native_4af8f0.cpp

using igi::retail::core::WindowHandlerEntry;

extern "C" {

std::int32_t* WinProc_RegisterHandler(std::int32_t* handler)
{
    // 0x0048F490-0x0048F4A5: scan node[0..15] by +0x08 (0x005C8D30, stride 0xC, end = 0x005C8DF0).
    WindowHandlerEntry* free_entry = nullptr;
    for (std::int32_t index = 0; index < 16; ++index)
    {
        if (WindowHandlerPool[index].handler == nullptr)
        {
            free_entry = &WindowHandlerPool[index];
            break;
        }
    }
    if (free_entry == nullptr)
    {
        // 0x0048F4A7: push 0x00541D70; call Debug_Error; 0x0048F4B4: jmp $.
        Debug_Error("Failed to register WinProc callback.");
        for (;;)
        {
        }
    }
    // 0x0048F4C0: [node+0x08]=arg1; 0x0048F4C6: [node+0x04]=0; 0x0048F4DC: [node+0x00]=0.
    free_entry->handler = reinterpret_cast<std::intptr_t (*)(void*, std::uint32_t, std::uintptr_t, std::intptr_t)>(handler);
    free_entry->prev_link = nullptr;
    free_entry->next = nullptr;
    // 0x0048F4E2: Sys_InsertListTail_4AF8F0(0x005C8DF0, node); returns its eax.
    return static_cast<std::int32_t*>(Sys_InsertListTail_4AF8F0(&WindowHandlerChain, reinterpret_cast<void**>(free_entry)));
}

} // extern "C"
