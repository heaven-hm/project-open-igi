// Native: WinProc_RemoveHandler; retail VA 0x0048F4F0. previous note: Native: WinProc_RemoveHandler (placeholder name until VERIFIED; retail BN alias FX_UnableToRemoveWinpro)
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x0048F4F0-0x0048F52E.
// Tools/evidence: Binary Ninja MCP disassembly/decompile + radare2 bytes; 7 basic blocks; spec build/audit/main-0x48F530/level1-closure/20261024T022900Z/TRANSCRIPTION-SPECS.md §1.6.
// Behavior/CFG: walks from the chain head while the current node has a non-null next word, compares handler (+0x08) with the argument and on a match unlinks with HeapList_Remove(0x004AF960), clears the handler and returns the remove result; not-found path calls Debug_Error("Unable to remove WinProc callback") and jumps forever (0x48F51B).
// Status/limits: BUILD_VERIFIED; depends on Sys_UnlinkListNode_4AF960 0x004AF960; not yet called by any transcribed caller (the 5 retail callers are untranscribed).
#include "igi/retail/core/native_0048f970.hpp"
#include "igi/retail/core/retail_error.hpp"
#include <cstdint>

using igi::retail::core::WindowHandlerEntry;

extern "C" {

// 0x004AF960: the list unlink half, transcribed in engine/src/retail/sys/native_4af960.cpp; C linkage since 2026-10-25.
void* Sys_UnlinkListNode_4AF960(void* node);

std::int32_t* WinProc_RemoveHandler(std::int32_t* handler)
{
    // 0x0048F4F1: node = head; 0x0048F4F7: next = [node].
    WindowHandlerEntry* node = WindowHandlerChain;
    WindowHandlerEntry* next = node->next;
    if (next == nullptr)
    {
        Debug_Error("Unable to remove WinProc callback");
        for (;;)
        {
        }
    }
    for (;;)
    {
        // 0x0048F501: compare [node+0x08] with the argument.
        if (node->handler == reinterpret_cast<std::intptr_t (*)(void*, std::uint32_t, std::uintptr_t, std::intptr_t)>(handler))
        {
            // 0x0048F51E: HeapList_Remove(node); 0x0048F526: [node+0x08]=0; returns eax.
            void* const removed = Sys_UnlinkListNode_4AF960(static_cast<void*>(node));
            node->handler = nullptr;
            return static_cast<std::int32_t*>(removed);
        }
        // 0x0048F506: node = next; 0x0048F508: next = [next]; 0x0048F50A: repeat.
        node = next;
        next = node->next;
        if (next == nullptr)
        {
            Debug_Error("Unable to remove WinProc callback");
            for (;;)
            {
            }
        }
    }
}

} // extern "C"
