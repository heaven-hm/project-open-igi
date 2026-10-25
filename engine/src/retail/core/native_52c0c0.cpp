// Native: DiscardTerrain_Init (name from the manifest record for 0x0052C0C0); retail VA 0x0052C0C0-0x0052C285.
// Artifact: retail igi.exe SHA-256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; image base 0x00400000; .text VA 0x00401000 raw 0x1000.
// Tools/evidence: capstone 5.0.7 decode from entry 0x0052C0C0: 125 instructions, 453 bytes, ONE `ret` at 0x0052C284, ZERO conditional branches, ZERO jumps, 28 calls. With no branch anywhere the extent cannot be ambiguous, and a linear sweep from the entry to that `ret` returns the same 125 instructions - the two methods agree exactly, so this body has none of the tail-jump ambiguity the auditor flags elsewhere. ENTRY BOUNDARY: the prologue `sub esp,0x158` at 0x0052C0C0 pairs with the epilogue `add esp,0x170` at 0x0052C27E (0x158 frame + 6 still-pushed argument dwords), and the caller is core/native_5183d0.cpp:117 (`call 0x0052C0C0` at 0x00518434).
// Behavior/CFG: straight-line registration, no branches. It reads a 16-bit global through 0x005034D0(0,0), registers the task type with the byte-exact name "DiscardTerrain" (0x0054F6D4) and size 0x158, stores the returned type id in the 16-bit global 0x00A8454C, then links SEVENTEEN handlers against that id - sixteen through 0x00401400 and one (0x00426860) through the no-context variant 0x00401530. Five of the event ids are not literals: they are the results of 0x004F1A60(0x0052C500), 0x004F1A70(0x0052C290), 0x004E5360(0x0052BFE0), 0x004CEA20(0x0052C580) and 0x004CEA10(0x0052C5A0); a sixth is 0x004D14D0(0) with a literal 0. Three further links pass a literal event (9, 8, 0xA) with a ZERO handler. It then allocates a task container with 0x004F1950(2), stores it in the 32-bit global 0x00A84550, and appends two blocks to it with 0x004F1A80(container, block, block, value): the values come from 0x004F1010(7,0) and 0x004F0FA0(7,0), and the blocks are the stack frame itself - the first at E-344 with its companion at E-104, the second again at E-344 with its companion at E-8. THE HANDLER ADDRESSES ARE PUSHED AS IMMEDIATES: there is no `E8` to 0x0052C000, 0x0052BFF0, 0x0052C0B0, 0x0052C510, 0x00426860, 0x0052BF00 or any other handler, so they are passed here as integer literals cast to the handler type - never as declared prototypes and never as address-of-symbol. The same applies to the six descriptor pointers pushed as arguments to the phase-id getters.
// ARITY DEVIATION, RECORDED RATHER THAN HIDDEN: task_types.hpp documents that "every retail registration call site pushes FIVE dwords" for TaskType_Register, and its four-parameter signature reads [esp+0xC], [esp+8], [esp+0x18] and [esp+0x1C]. THIS call site pushes THREE (0x0052C0CF, 0x0052C0D0, 0x0052C0D5) and the cleanup `add esp,0x44` at 0x0052C133 accounts for exactly 2+3+3+3+3+3 = 17 dwords, so no fourth dword was ever pushed here. The callee's fourth parameter therefore reads a stale stack slot in retail, which this transcription cannot reproduce from C++ - it passes 0 for task_list_argument, so no class task list is built, and this one-slot difference is a recorded limitation rather than a claim. Everything else at this site is byte-exact.
// THE PHASE-ID GETTERS READ NO ARGUMENT, MEASURED: 0x004F1A60 is `mov al,[0x54e0e4]; ret` and 0x004F1A70 is `mov al,[0x54e0e5]; ret` - two instructions each, no [esp+N] read at all, so the zero-parameter declarations in script_state.hpp are CORRECT and the descriptor pointer this call site pushes (0x0052C500, 0x0052C290) is ignored by the callee. The local declarations below carry one parameter because that is what this call site pushes, matching retail's stack; the two forms are behaviourally identical and this is a stack-shape difference, not a defect in either file.
// Status/limits: TRANSCRIBED; NOT LINKABLE - six of its callees have no definition in the tree (listed in the report and declared below as measured-arity gaps). STRUCTURALLY_VERIFIED against the 125-instruction listing (0 branches, 1 ret, 28 call sites, both sizing methods agreeing); BUILD_VERIFIED (i686-w64-mingw32-g++ -std=c++20 -O2 -Wall -Wextra single-file compile); runtime UNOBSERVED. LIMITS: (a) the manifest record for this VA carries the signature `int* DiscardTerrain_Init(void)` and the rationale "Address-bound leaf instructions map the retail chunk walk onto the port's C++ helpers" - BOTH ARE WRONG for these bytes: the body returns nothing that this listing sets deliberately (the caller declares void) and it is not a leaf, it makes 28 calls. The name is kept because the manifest is the naming authority for the VA; the signature and rationale are not adopted. (b) script_state.hpp declares 0x004F1A60 and 0x004F1A70 with ZERO parameters while this body pushes one descriptor pointer to each; the local declarations below carry the observed arity and that header is deliberately not included, so the mismatch is reported rather than inherited. (c) what the two appended blocks mean is not established by this body. Domain: core/, because the caller is core/native_5183d0.cpp and it registers a core task type.

#include "igi/retail/core/task_types.hpp"

#include <cstdint>

// task_types.hpp defines NATIVE_DECL for its own declarations and #undefs it again, so it is re-established here for
// this file's own definition (the same guarded block the other retail transcriptions carry).
#ifndef NATIVE_DECL
#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif
#endif

extern "C" {

// Globals this body owns. Both are zero-fill in the image (their VAs have no file-backed bytes), so their initial
// value is 0; the type id is written before every read and the container pointer before every use.
std::uint16_t g_a8454c = 0U; // 0x0052C0E7: mov word [0xa8454c],ax - the registered task type id
std::uint32_t g_a84550 = 0U; // 0x0052C230: mov dword [0xa84550],eax - the task container

// NATIVE GAPS. Each arity is measured from the push sequence at this body's call site of it; none is a declared prototype.
// 0x005034D0: 2 slots (push 0 0x0052C0C6, push 0 0x0052C0C8). Named from its manifest record.
std::uint32_t Sys_GetGlobal16_5034D0(std::uint32_t a1, std::uint32_t a2);
// 0x004E5360: 1 slot (push 0x0052BFE0 0x0052C17C). No manifest record; body unread.
std::uint32_t Sub_4E5360(std::uint32_t a1);
// 0x004CEA20: 1 slot (push 0x0052C580 0x0052C1AA). No manifest record; body unread.
std::uint32_t QTask_PhaseIdGet548177_4CEA20(std::uint32_t a1);
// 0x004CEA10: 1 slot (push 0x0052C5A0 0x0052C1C5). No manifest record; body unread.
std::uint32_t QTask_PhaseIdGet548176_4CEA10(std::uint32_t a1);
// 0x004D14D0: 1 slot (push 0 0x0052C1DF). Named from its manifest record.
std::uint32_t Render_GetByte_4D14D0(std::uint32_t a1);
// 0x004F1950: 1 slot (push 2 0x0052C226). Named from its manifest record.
std::uint32_t Script_TaskContainer_Alloc(std::uint32_t a1);
// 0x004F1010: 2 slots (push 0 0x0052C235, push 7 0x0052C237). Named from its manifest record.
std::uint32_t Sys_GetGlobal32_4F1010(std::uint32_t a1, std::uint32_t a2);
// 0x004F1A80: SIX slots, because the callee's prologue reads arg6 (the Lead's ruling at
// docs/fidelity/evidence/script-taskcontainer-append-arity-ruling.md). Each site here pushes four, and arg5/arg6 are
// the preceding getter's own two deferred pushes, still live above them: the getter was called as (7, 0), so arg5 = 7
// and arg6 = 0 at both sites. Derived from the push sequence, not omitted and not invented.
extern "C" void* Script_TaskContainer_Append(std::uint32_t container, void* block, void* slot,
                                             std::uint32_t value, std::uint32_t arg5, std::uint32_t arg6);
// 0x004F0FA0: 2 slots (push 0 0x0052C25B, push 7 0x0052C25D). Named from its manifest record.
std::uint32_t Core_GetRenderWordA_4F0FA0(std::uint32_t a1, std::uint32_t a2);
// 0x004F1A60 / 0x004F1A70: 1 slot each (push 0x0052C500 0x0052C146, push 0x0052C290 0x0052C161). script_state.hpp
// declares these with zero parameters; the observed arity is one, so they are declared here and that header is not included.
std::uint32_t Script_PhaseIdGetE4_4F1A60(std::uint32_t a1);
std::uint32_t Script_PhaseIdGetE5_4F1A70(std::uint32_t a1);

void NATIVE_DECL DiscardTerrain_Init(void)
{
    std::uint8_t frame[0x158]; // 0x0052C0C0: sub esp,0x158. frame[i] is E-0x158+i; slot offsets are stated at each use.

    // 0x0052C0C6-0x0052C0CA: push 0; push 0; call 0x5034d0.
    const std::uint32_t parent = Sys_GetGlobal16_5034D0(0U, 0U);

    // 0x0052C0CF-0x0052C0DA: push eax (parent); push 0x158 (object size); push 0x54f6d4 ("DiscardTerrain").
    // See the ARITY DEVIATION note in the provenance: retail pushes three dwords here, so the callee's fourth
    // parameter reads a stale slot; this transcription passes 0 for it and records the difference.
    const std::uint16_t type_id = TaskType_Register("DiscardTerrain", 0x158U, static_cast<std::uint16_t>(parent), 0U);
    g_a8454c = type_id; // 0x0052C0E7: mov word [0xa8454c],ax

    // 0x0052C0DF-0x0052C0ED: push 0x52c000 (handler); push 0 (event); push eax (id); call 0x401400.
    QTask_LinkQTaskEventHandler(g_a8454c, 0U, reinterpret_cast<igi::retail::core::FrameHandler>(0x0052C000U));
    // 0x0052C0F8-0x0052C100: event 4, handler 0x52bff0.
    QTask_LinkQTaskEventHandler(g_a8454c, 4U, reinterpret_cast<igi::retail::core::FrameHandler>(0x0052BFF0U));
    // 0x0052C10C-0x0052C114: event 0xf, handler 0x52c0b0.
    QTask_LinkQTaskEventHandler(g_a8454c, 0x0FU, reinterpret_cast<igi::retail::core::FrameHandler>(0x0052C0B0U));
    // 0x0052C120-0x0052C128: event 7, handler 0x52c510.
    QTask_LinkQTaskEventHandler(g_a8454c, 7U, reinterpret_cast<igi::retail::core::FrameHandler>(0x0052C510U));

    // 0x0052C136-0x0052C13E: event 2, handler 0x426860, through the no-context variant.
    QTask_LinkQTaskEventHandlerNoContextFn(g_a8454c, 2U, reinterpret_cast<igi::retail::core::FrameHandler>(0x00426860U));

    // 0x0052C146-0x0052C159: the event is 0x4f1a60(0x52c500)'s result, the handler is 0x52c500.
    QTask_LinkQTaskEventHandler(g_a8454c, Script_PhaseIdGetE4_4F1A60(0x0052C500U),
                                reinterpret_cast<igi::retail::core::FrameHandler>(0x0052C500U));
    // 0x0052C161-0x0052C174: the event is 0x4f1a70(0x52c290)'s result, the handler is 0x52c290.
    QTask_LinkQTaskEventHandler(g_a8454c, Script_PhaseIdGetE5_4F1A70(0x0052C290U),
                                reinterpret_cast<igi::retail::core::FrameHandler>(0x0052C290U));
    // 0x0052C17C-0x0052C18E: the event is 0x4e5360(0x52bfe0)'s result, the handler is 0x52bfe0.
    QTask_LinkQTaskEventHandler(g_a8454c, Sub_4E5360(0x0052BFE0U),
                                reinterpret_cast<igi::retail::core::FrameHandler>(0x0052BFE0U));
    // 0x0052C19A-0x0052C1A2: event 0xb, handler 0x52bf00.
    QTask_LinkQTaskEventHandler(g_a8454c, 0x0BU, reinterpret_cast<igi::retail::core::FrameHandler>(0x0052BF00U));
    // 0x0052C1AA-0x0052C1BD: the event is 0x4cea20(0x52c580)'s result, the handler is 0x52c580.
    QTask_LinkQTaskEventHandler(g_a8454c, QTask_PhaseIdGet548177_4CEA20(0x0052C580U),
                                reinterpret_cast<igi::retail::core::FrameHandler>(0x0052C580U));
    // 0x0052C1C5-0x0052C1D7: the event is 0x4cea10(0x52c5a0)'s result, the handler is 0x52c5a0.
    QTask_LinkQTaskEventHandler(g_a8454c, QTask_PhaseIdGet548176_4CEA10(0x0052C5A0U),
                                reinterpret_cast<igi::retail::core::FrameHandler>(0x0052C5A0U));
    // 0x0052C1DF-0x0052C1EF: the event is 0x4d14d0(0)'s result, the handler is 0.
    QTask_LinkQTaskEventHandler(g_a8454c, Render_GetByte_4D14D0(0U),
                                reinterpret_cast<igi::retail::core::FrameHandler>(0U));
    // 0x0052C1FB-0x0052C200: event 9, handler 0. 0x0052C20B-0x0052C210: event 8, handler 0. 0x0052C21C-0x0052C221: event 0xa, handler 0.
    QTask_LinkQTaskEventHandler(g_a8454c, 9U, reinterpret_cast<igi::retail::core::FrameHandler>(0U));
    QTask_LinkQTaskEventHandler(g_a8454c, 8U, reinterpret_cast<igi::retail::core::FrameHandler>(0U));
    QTask_LinkQTaskEventHandler(g_a8454c, 0x0AU, reinterpret_cast<igi::retail::core::FrameHandler>(0U));

    // 0x0052C226-0x0052C230: push 2; call 0x4f1950; mov [0xa84550],eax.
    g_a84550 = Script_TaskContainer_Alloc(2U);

    // 0x0052C235-0x0052C253: push 0; push 7; call 0x4f1010; then push eax (value), push edx (&frame[0xf0] = E-104),
    // push eax (&frame[0x00] = E-344), push ecx (the container); call 0x4f1a80. The getter's two pushes are still
    // live, so they are arg5 and arg6: 7 and 0.
    const std::uint32_t value_a = Sys_GetGlobal32_4F1010(7U, 0U);
    Script_TaskContainer_Append(g_a84550, frame + 0x00, frame + 0xF0, value_a, 7U, 0U); // 0x0052C258: add esp,0x18

    // 0x0052C25B-0x0052C279: push 0; push 7; call 0x4f0fa0; then the same container and first block, with the
    // companion block at &frame[0x150] = E-8. Same deferred pushes: arg5 = 7, arg6 = 0.
    const std::uint32_t value_b = Core_GetRenderWordA_4F0FA0(7U, 0U);
    Script_TaskContainer_Append(g_a84550, frame + 0x00, frame + 0x150, value_b, 7U, 0U);

    // 0x0052C27E: add esp,0x170 (0x158 frame + the 6 argument dwords still pushed); 0x0052C284: ret.
    // The caller declares this native void, and the listing sets no deliberate return value.
}

} // extern "C"

#undef NATIVE_DECL
