#include "igi/retail/mem/heap_list.hpp"

#include <cstdint>

namespace igi::retail::mem {

// Native: HeapList_Insert; retail VA 0x004AF8F0; signature/calling convention unknown.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE (SHA256: dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01); VA 0x004AF8F0-0x004AF910, 32 bytes; CLEAN/camera/004AF8F0-Camera_SetFieldOfView_33.c.
// Behavior/CFG: inserts `arg2` at the tail of an intrusive doubly-linked list whose header is at `context` (`context + 4` = head anchor, `context + 8` = tail pointer): writes `arg2[1] = *(context + 8)`, `*arg2 = context + 4`, `**(context + 8) = arg2`, `*(context + 8) = arg2`, and returns `context`. radare2 1 block/0 edges; Binary Ninja 1 block; 100% structural agreement.
// Status/limits: STRUCTURALLY_VERIFIED; high for exact pointer splice sequence.
// Previous catalog alias: Camera_SetFieldOfView_33.
void* HeapList_Insert(void* context, void** arg2)
{
    auto* const header = static_cast<std::uint8_t*>(context);
    if constexpr (sizeof(void*) == 4)
    {
        void** const tail = reinterpret_cast<void**>(header + 0x08);
        arg2[1] = *tail;
        *arg2 = header + 0x04;
        **reinterpret_cast<void***>(header + 0x08) = arg2;
        *tail = arg2;
    }
    else
    {
        // On a 64-bit host an 8-byte write through `header + 0x04` spans `0x04..0x0B` and would
        // clobber `header + 0x08` if the tail slot sat at `+0x08`. If the caller initialized the
        // tail pointer at `+0x08` to `header + 0x04` (or `header`), use `+0x08` when it points
        // outside `[header+0x04, header+0x0C)`, or mirror the tail pointer in `+0x10` when the
        // head anchor is at `+0x04`.
        void** const head_anchor = reinterpret_cast<void**>(header + 0x04);
        void** const tail_slot_8 = reinterpret_cast<void**>(header + 0x08);
        if (*tail_slot_8 == nullptr)
        {
            // MemGlobals layout: context[0] is head_anchor (`g_a960f0`), context[2] is `g_a960f8`.
            auto** const slots = static_cast<void**>(context);
            void** const tail = reinterpret_cast<void**>(&slots[2]);
            if (*tail == nullptr)
            {
                *tail = &slots[0];
            }
            arg2[1] = *tail;
            *arg2 = &slots[1];
            **reinterpret_cast<void***>(&slots[2]) = arg2;
            *tail = arg2;
        }
        else
        {
            void* prev_tail = *tail_slot_8;
            arg2[1] = prev_tail;
            *arg2 = head_anchor;
            if (prev_tail == head_anchor)
            {
                // Empty list initialized with `*(header + 0x08) = header + 0x04`: store the head
                // link at `header + 0x00` on 64-bit so it does not overwrite `header + 0x08`.
                *reinterpret_cast<void**>(header) = arg2;
            }
            else
            {
                *static_cast<void**>(prev_tail) = arg2;
            }
            *tail_slot_8 = arg2;
        }
    }
    return context;
}

// Native: HeapList_Remove; retail VA 0x004AF960; signature/calling convention unknown.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x004AF960-0x004AF981, 33 bytes, digest 4ada7252239e45e0b5ed56005655c50a7ca29e3c5e3be153590e183ca807eb46; CLEAN/camera/004AF960-Camera_ApplyHandiCam_33.c.
// Behavior/CFG: unlinks `context` from an intrusive doubly-linked list by writing `*context[1] = *context`, `*(static_cast<void**>(*context) + 1) = context[1]`, clearing `context[1] = nullptr` and `context[0] = nullptr`, and returning `context`. radare2 1 block/0 edges; Binary Ninja 1 block; 100% structural agreement.
// Status/limits: STRUCTURALLY_VERIFIED; high for exact pointer unlink steps.
// Previous catalog alias: Camera_ApplyHandiCam_33.
void** HeapList_Remove(void** context)
{
    *static_cast<void**>(context[1]) = context[0];
    static_cast<void**>(context[0])[1] = context[1];
    context[1] = nullptr;
    context[0] = nullptr;
    return context;
}

} // namespace igi::retail::mem

