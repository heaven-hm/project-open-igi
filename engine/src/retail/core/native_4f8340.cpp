// Native: TerrainLightMap_Init (name from the manifest record for 0x004F8340); retail VA 0x004F8340-0x004F86DF.
// Artifact: retail igi.exe SHA-256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; image base 0x00400000; .text VA 0x00401000 raw 0x1000.
// Tools/evidence: capstone 5.0.7 from entry 0x004F8340: 276 instructions, 928 bytes, contiguous, ONE `ret` at 0x004F86DF, ZERO conditional branches, ZERO jumps. With no branch the extent cannot be ambiguous and a linear sweep returns the same 276 instructions. CROSSCHECK, AND A TOOL DISAGREEMENT RESOLVED BY EVIDENCE RATHER THAN BY CHOICE: scripts/native_fidelity_audit.py crosscheck_ndisasm(0x004F8340) reports (276, 277, False). I checked the byte accounting and the instruction stream before trusting either: capstone's 276 instructions sum to exactly 928 bytes, equal to the extent 0x004F8340-0x004F86E0, contiguous with no gap; and ndisasm decodes the SAME instruction stream at the SAME addresses for all 276 - the first divergence is at index 276, i.e. at 0x004F86E0, which is one byte PAST the `ret` at 0x004F86DF. The extra instruction is therefore outside this function, and the disagreement is a window/stopping difference in the crosscheck, not a dropped or mis-split instruction in capstone. The same holds for 0x00505F60 (211 vs 212, extra at 0x0050623A, past the ret at 0x00506239). This is recorded because the auditor asks for a disagreement to be reported as one; the report is that capstone agrees with ndisasm inside the function on both bodies, and the crosscheck's window needs to stop at the first `ret` or it will flag every body followed by padding.
// Behavior/CFG: straight-line registration, no branches, single block. Stores three words at 0x00A76CD0 through 0x004B4720, writes 0x19111911 there through 0x004B47E0, reads the 16-bit class id at 0x0054875C (image bytes 81 01 = 0x0181), registers the task type under the byte-exact name "TerrainLightMap" (0x0054E304) with object size 0x110, stores the returned id at 0x00A76CDC, links FOURTEEN handlers against it (thirteen through 0x00401400, one - 0x004F86E0 - through the no-context variant 0x00401530), allocates a task container with 0x004F1950(0xF) storing it at 0x00A76D04, then makes FIFTEEN appends through 0x004F1A80, and finally zeroes the 32-bit global 0x00B97D24. The appends all pass the same first block (the frame base) and a different companion slot inside that block, with values from 0x004F1010(7,0), 0x004F0FA0(7,0) twice, 0x004F0EF0(6,0) five times, 0x004F1000(6,0) five times, 0x004F0FA0(1,0), 0x004F1000(1,0) and 0x004F0EF0(6,0). The companion slots are BYTE-granular (E-160, E-88, E-60, E-75, E-76, E-74, E-77, E-80, E-78, E-79, E-52, E-72, E-71, E-69, E-68), so the record is not dword-spaced and the frame is transcribed as bytes. Handler addresses are pushed as IMMEDIATES - there is no `E8` to 0x004F8190, 0x004F8320, 0x004F82B0, 0x004F8770, 0x004F8780 or 0x004F86E0 - so they are integer literals cast to the handler type, never declared prototypes and never address-of-symbol.
// ARITY, AND THE LEAD'S RULING APPLIED (docs/fidelity/evidence/script-taskcontainer-append-arity-ruling.md): TaskType_Register here receives FIVE pushed dwords (0x004F8365, 0x004F8367, 0x004F8369, 0x004F836A, 0x004F836F), matching the five-push pattern task_types.hpp documents, and the cleanup `add esp,0x44` at 0x004F83B4 accounts for exactly 1+2+5+3+3+3 = 17 dwords. The fourth argument is the pushed 0 and the fifth is the extra slot the callee never reads, so this transcription passes four arguments exactly as that header's own note prescribes. For 0x004F1A80 the ruling is the opposite outcome: its prologue reads arg6 after its register saves, so the callee genuinely consumes six slots, and every site in THIS body pushes only four (e.g. 0x004F84AA-0x004F84B5: push value, lea slot, lea block, push container; each cleanup is `add esp,0x18` = 6 dwords = 2 getter pushes + 4 append pushes). The two missing slots are NOT invented and NOT omitted: they are the getter's own two deferred pushes, still live above the four because the caller defers cleanup, so arg5 is the getter's first argument (7, 6 or 1) and arg6 is its second (0 at every site). Those derived values are passed explicitly at all fifteen sites. The rule is per-case, decided by reading the callee's prologue: reads it and deferred pushes supply it -> pass the derived value; reads nothing (0x004F1A60, 0x004F1A70) -> the zero-argument declaration is correct.
// Status/limits: TRANSCRIBED; NOT LINKABLE - Sub_4B47E0 has no definition in the tree. STRUCTURALLY_VERIFIED against the 276-instruction listing with capstone and ndisasm agreeing inside the function; BUILD_VERIFIED (single-file i686-w64-mingw32-g++ -std=c++20 -O2 -Wall -Wextra compile); runtime UNOBSERVED. LIMITS: (a) the manifest record for this VA carries the signature `int* TerrainLightMap_Init(void)` and the same "Address-bound leaf instructions map the retail chunk walk onto the port's C++ helpers" rationale as 0x0052C0C0 and 0x00505F60 - the rationale is false here too (this is not a leaf; it makes 51 calls) and no deliberate return value is set. The name is kept as the naming authority; neither the signature nor the rationale is adopted. (b) the frame is passed to 0x004F1A80 UNINITIALISED, exactly as retail does - nothing in the body writes it before the appends. (c) what the appended records mean is 0x004F1A80's business, not this body's. (d) 0x00B97D24 is an existing tree global (terrain_loaders.hpp `g_b97d24`), so this body assigns it rather than defining it. Domain: core/, because the caller is core/native_5183d0.cpp (`call 0x004F8340` at 0x0051851A).

#include "igi/retail/core/task_types.hpp"
#include "igi/retail/terrain/terrain_loaders.hpp"

#include <cstdint>

// task_types.hpp defines NATIVE_DECL for its own declarations and #undefs it again, so it is re-established here.
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

// 0x0054875C: the parent class id, defined in dynobj/native_004e5370.cpp (image bytes 81 01 = 0x0181).
extern std::uint16_t g_54875c;

// Globals this body owns. Both are zero-fill in the image (their VAs are beyond the file-backed data).
std::uint16_t g_a76cdc = 0U; // 0x004F8381: mov word [0xa76cdc],ax - the registered task type id
std::uint32_t g_a76d04 = 0U; // 0x004F8496: mov dword [0xa76d04],eax - the task container

// NATIVE GAPS, arity measured from this body's push sequences; none is a declared prototype.
// 0x004B4720: 1 slot (push 0xa76cd0 0x004F8346). Named from its manifest record.
void Sys_StoreThreeWords_4B4720(void* target);
// 0x004B47E0: 2 slots (push 0x19111911 0x004F8350, push 0xa76cd0 0x004F8355). No manifest record; no definition in the tree.
std::uint32_t Sub_4B47E0(void* target, std::uint32_t value);
// 0x004F1010: 2 slots. Named from its manifest record.
std::uint32_t Sys_GetGlobal32_4F1010(std::uint32_t a1, std::uint32_t a2);
// 0x004F0EF0: 2 slots. No manifest record.
std::uint32_t Sys_GetGlobal32_4F0EF0(std::uint32_t a1, std::uint32_t a2);
// 0x004F0FA0: 2 slots. Named from its manifest record.
std::uint32_t Core_GetRenderWordA_4F0FA0(std::uint32_t a1, std::uint32_t a2);
// 0x004D14D0: 1 slot (push 0 0x004F840D). Named from its manifest record.
std::uint32_t Render_GetByte_4D14D0(std::uint32_t a1);
// 0x004F1950: 1 slot (push 0xf 0x004F848C). Named from its manifest record.
std::uint32_t Script_TaskContainer_Alloc(std::uint32_t a1);
// 0x004F1A80: SIX slots, because the callee's prologue reads arg6 (the Lead's ruling at
// docs/fidelity/evidence/script-taskcontainer-append-arity-ruling.md). This body's call sites push four, and arg5 and
// arg6 are the getter's own two deferred pushes, still live above them, so the values passed here are DERIVED from the
// push sequence rather than omitted or invented: arg5 is the getter's index (7, 6 or 1) and arg6 is always 0. The
// parameter types are this port's reading of the slots; the slot count and cdecl convention match the definition in
// script/native_4f1a80.cpp, which is what the ruling requires.
extern "C" void* Script_TaskContainer_Append(std::uint32_t container, void* block, void* slot,
                                             std::uint32_t value, std::uint32_t arg5, std::uint32_t arg6);
// 0x004F1A60 / 0x004F1A70: the bodies are `mov al,[0x54e0e4]; ret` and `mov al,[0x54e0e5]; ret` and read no stack argument,
// so the descriptor pointer each call site pushes is inert; the local arity records the push, not a read.
std::uint8_t Script_PhaseIdGetE4_4F1A60(std::uint32_t a1);
std::uint8_t Script_PhaseIdGetE5_4F1A70(std::uint32_t a1);
// 0x004F1000: `mov eax,[0xA70C90]; ret` - two instructions, no argument read, so this takes none.
std::uint8_t Camera_Get_4F1000(void);

void NATIVE_DECL TerrainLightMap_Init(void)
{
    std::uint8_t frame[0x110]; // 0x004F8340: sub esp,0x110. frame[i] is E-0x110+i; entry-relative offsets are stated at each use.

    // 0x004F8346-0x004F834B: push 0xa76cd0; call 0x4b4720.
    Sys_StoreThreeWords_4B4720(reinterpret_cast<void*>(0x00A76CD0U));
    // 0x004F8350-0x004F835A: push 0x19111911; push 0xa76cd0; call 0x4b47e0.
    Sub_4B47E0(reinterpret_cast<void*>(0x00A76CD0U), 0x19111911U);

    // 0x004F835F: mov ax,word [0x54875c]; 0x004F8365-0x004F836F: push 0; push 0; push eax; push 0x110; push 0x54e304.
    // 0x0054E304 is the byte-exact name string "TerrainLightMap".
    const std::uint16_t type_id = TaskType_Register("TerrainLightMap", 0x110U, g_54875c, 0U);
    g_a76cdc = type_id; // 0x004F8381: mov word [0xa76cdc],ax

    // 0x004F8379-0x004F8387: event 0, handler 0x4f8190.
    QTask_LinkQTaskEventHandler(g_a76cdc, 0U, reinterpret_cast<igi::retail::core::FrameHandler>(0x004F8190U));
    // 0x004F8393-0x004F839B: event 0x10, handler 0x4f8320.
    QTask_LinkQTaskEventHandler(g_a76cdc, 0x10U, reinterpret_cast<igi::retail::core::FrameHandler>(0x004F8320U));
    // 0x004F83A7-0x004F83AF: event 2, handler 0x4f86e0, through the no-context variant.
    QTask_LinkQTaskEventHandlerNoContextFn(g_a76cdc, 2U, reinterpret_cast<igi::retail::core::FrameHandler>(0x004F86E0U));
    // 0x004F83B7-0x004F83C9: the event is 0x4f1a60(0x4f8770)'s result, the handler is 0x4f8770.
    QTask_LinkQTaskEventHandler(g_a76cdc, Script_PhaseIdGetE4_4F1A60(0x004F8770U),
                                reinterpret_cast<igi::retail::core::FrameHandler>(0x004F8770U));
    // 0x004F83D1-0x004F83E4: the event is 0x4f1a70(0x4f8780)'s result, the handler is 0x4f8780.
    QTask_LinkQTaskEventHandler(g_a76cdc, Script_PhaseIdGetE5_4F1A70(0x004F8780U),
                                reinterpret_cast<igi::retail::core::FrameHandler>(0x004F8780U));
    // 0x004F83F0-0x004F83F5: event 0xa, handler 0. 0x004F8400-0x004F8405: event 0xb, handler 0.
    QTask_LinkQTaskEventHandler(g_a76cdc, 0x0AU, reinterpret_cast<igi::retail::core::FrameHandler>(0U));
    QTask_LinkQTaskEventHandler(g_a76cdc, 0x0BU, reinterpret_cast<igi::retail::core::FrameHandler>(0U));
    // 0x004F840D-0x004F841D: the event is 0x4d14d0(0)'s result, the handler is 0.
    QTask_LinkQTaskEventHandler(g_a76cdc, Render_GetByte_4D14D0(0U),
                                reinterpret_cast<igi::retail::core::FrameHandler>(0U));
    // 0x004F8429-0x004F842E: event 9, handler 0. 0x004F8439-0x004F843E: event 0xc, handler 0.
    QTask_LinkQTaskEventHandler(g_a76cdc, 9U, reinterpret_cast<igi::retail::core::FrameHandler>(0U));
    QTask_LinkQTaskEventHandler(g_a76cdc, 0x0CU, reinterpret_cast<igi::retail::core::FrameHandler>(0U));
    // 0x004F844A-0x004F844F: event 8, handler 0. 0x004F845B-0x004F8460: event 4, handler 0.
    QTask_LinkQTaskEventHandler(g_a76cdc, 8U, reinterpret_cast<igi::retail::core::FrameHandler>(0U));
    QTask_LinkQTaskEventHandler(g_a76cdc, 4U, reinterpret_cast<igi::retail::core::FrameHandler>(0U));
    // 0x004F8465-0x004F8470: event 5, handler 0. 0x004F847F-0x004F8487: event 0xf, handler 0x4f82b0.
    QTask_LinkQTaskEventHandler(g_a76cdc, 5U, reinterpret_cast<igi::retail::core::FrameHandler>(0U));
    QTask_LinkQTaskEventHandler(g_a76cdc, 0x0FU, reinterpret_cast<igi::retail::core::FrameHandler>(0x004F82B0U));

    // 0x004F848C-0x004F8496: push 0xf; call 0x4f1950; mov [0xa76d04],eax.
    g_a76d04 = Script_TaskContainer_Alloc(0x0FU);

    // The fifteen appends. Each site pushes FOUR dwords (value, slot, block, container) but the callee reads SIX, and
    // arg5/arg6 are the getter's own two deferred pushes still live above them: the getter was called as (N, 0), so
    // arg5 = N (7, 6 or 1) and arg6 = 0 at every site. Those derived values are passed explicitly, per the ruling.
    // 0x004F849B-0x004F84B6: value 0x4f1010(7,0), slot E-160 (frame+0x70), deferred pushes 7 and 0.
    Script_TaskContainer_Append(g_a76d04, frame + 0x00, frame + 0x70, Sys_GetGlobal32_4F1010(7U, 0U), 7U, 0U);
    // 0x004F84BE-0x004F84DC: value 0x4f0fa0(7,0), slot E-88 (frame+0xb8), deferred pushes 7 and 0.
    Script_TaskContainer_Append(g_a76d04, frame + 0x00, frame + 0xB8, Core_GetRenderWordA_4F0FA0(7U, 0U), 7U, 0U);
    // 0x004F84E4-0x004F8502: value 0x4f0fa0(7,0), slot E-60 (frame+0xd4), deferred pushes 7 and 0.
    Script_TaskContainer_Append(g_a76d04, frame + 0x00, frame + 0xD4, Core_GetRenderWordA_4F0FA0(7U, 0U), 7U, 0U);
    // 0x004F850A-0x004F8528: value 0x4f0ef0(6,0), slot E-75 (frame+0xc5), deferred pushes 6 and 0.
    Script_TaskContainer_Append(g_a76d04, frame + 0x00, frame + 0xC5, Sys_GetGlobal32_4F0EF0(6U, 0U), 6U, 0U);
    // 0x004F8530-0x004F854E: value 0x4f1000(6,0), slot E-76 (frame+0xc4), deferred pushes 6 and 0. The getter itself
    // reads no argument, but its two pushes are still on the stack and are what arg5/arg6 receive.
    Script_TaskContainer_Append(g_a76d04, frame + 0x00, frame + 0xC4, Camera_Get_4F1000(), 6U, 0U);
    // 0x004F8556-0x004F8574: value 0x4f1000(6,0), slot E-74 (frame+0xc6), deferred pushes 6 and 0. Retail loads the
    // container AFTER the two `lea`s at 0x004F8560/0x004F8567; the net argument order is unchanged.
    Script_TaskContainer_Append(g_a76d04, frame + 0x00, frame + 0xC6, Camera_Get_4F1000(), 6U, 0U);
    // 0x004F857C-0x004F859A: value 0x4f1000(6,0), slot E-77 (frame+0xc3), deferred pushes 6 and 0.
    Script_TaskContainer_Append(g_a76d04, frame + 0x00, frame + 0xC3, Camera_Get_4F1000(), 6U, 0U);
    // 0x004F85A2-0x004F85C0: value 0x4f0ef0(6,0), slot E-80 (frame+0xc0), deferred pushes 6 and 0.
    Script_TaskContainer_Append(g_a76d04, frame + 0x00, frame + 0xC0, Sys_GetGlobal32_4F0EF0(6U, 0U), 6U, 0U);
    // 0x004F85C8-0x004F85E6: value 0x4f0ef0(6,0), slot E-78 (frame+0xc2), deferred pushes 6 and 0.
    Script_TaskContainer_Append(g_a76d04, frame + 0x00, frame + 0xC2, Sys_GetGlobal32_4F0EF0(6U, 0U), 6U, 0U);
    // 0x004F85EE-0x004F860C: value 0x4f0ef0(6,0), slot E-79 (frame+0xc1), deferred pushes 6 and 0.
    Script_TaskContainer_Append(g_a76d04, frame + 0x00, frame + 0xC1, Sys_GetGlobal32_4F0EF0(6U, 0U), 6U, 0U);
    // 0x004F8614-0x004F8632: value 0x4f0fa0(1,0), slot E-52 (frame+0xdc), deferred pushes 1 and 0.
    Script_TaskContainer_Append(g_a76d04, frame + 0x00, frame + 0xDC, Core_GetRenderWordA_4F0FA0(1U, 0U), 1U, 0U);
    // 0x004F863A-0x004F8658: value 0x4f1000(1,0), slot E-72 (frame+0xc8), deferred pushes 1 and 0.
    Script_TaskContainer_Append(g_a76d04, frame + 0x00, frame + 0xC8, Camera_Get_4F1000(), 1U, 0U);
    // 0x004F8660-0x004F867E: value 0x4f0ef0(6,0), slot E-71 (frame+0xc9), deferred pushes 6 and 0.
    Script_TaskContainer_Append(g_a76d04, frame + 0x00, frame + 0xC9, Sys_GetGlobal32_4F0EF0(6U, 0U), 6U, 0U);
    // 0x004F8686-0x004F86A4: value 0x4f1000(6,0), slot E-69 (frame+0xcb), deferred pushes 6 and 0.
    Script_TaskContainer_Append(g_a76d04, frame + 0x00, frame + 0xCB, Camera_Get_4F1000(), 6U, 0U);
    // 0x004F86AC-0x004F86CA: value 0x4f1000(6,0), slot E-68 (frame+0xcc), deferred pushes 6 and 0.
    Script_TaskContainer_Append(g_a76d04, frame + 0x00, frame + 0xCC, Camera_Get_4F1000(), 6U, 0U);

    // 0x004F86CF: mov dword [0xb97d24],0 - the existing tree global g_b97d24 (terrain_loaders.hpp), reset here.
    igi::retail::terrain::g_b97d24 = 0;
    // 0x004F86D9: add esp,0x128 (0x110 frame + the 6 argument dwords still pushed); 0x004F86DF: ret.
}

} // extern "C"

#undef NATIVE_DECL
