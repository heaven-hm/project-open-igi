// Native: TextureModifier_Init (name from the manifest record for 0x00505F60); retail VA 0x00505F60-0x00506239.
// Artifact: retail igi.exe SHA-256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; image base 0x00400000; .text VA 0x00401000 raw 0x1000.
// Tools/evidence: capstone 5.0.7 from entry 0x00505F60: 211 instructions, 730 bytes, contiguous, ONE `ret` at 0x00506239, ZERO conditional branches, ZERO jumps. With no branch the extent cannot be ambiguous and a linear sweep returns the same 211 instructions. CROSSCHECK: crosscheck_ndisasm(0x00505F60) reported (211, 212, False) before the Lead fixed the counter; the cause was ndisasm's `-0000` header/sync line being counted as an instruction, and the first divergence was at index 211, i.e. at 0x0050623A, one byte PAST the `ret` at 0x00506239. After his fix all six known cases agree, this body among them at 211/211.
// Behavior/CFG: straight-line registration, no branches, single block. Stores three words at 0x00A7ACF8 through 0x004B4720, writes 0x19111911 there through 0x004B47E0, reads the 16-bit class id at 0x0054875C (image bytes 81 01 = 0x0181), registers the task type under the byte-exact name "TextureModifier" (0x0054E7A4) with object size 0xF0, stores the returned id at 0x00A7A8F0, links FIFTEEN handlers against it (fourteen through 0x00401400, one - 0x00506240 - through the no-context variant 0x00401530), allocates a task container with 0x004F1950(9) storing it at 0x00A7A8EC, makes NINE appends through 0x004F1A80, and finally zeroes the 32-bit global 0x00B97DF8. Handler addresses are pushed as IMMEDIATES - there is no `E8` to 0x00505F20, 0x00505D50, 0x00505ED0, 0x00505E40, 0x00506240, 0x00506290, 0x005062A0 or 0x004F64E0 - so they are integer literals cast to the handler type, never declared prototypes and never address-of-symbol. The fifth and sixth link events are the results of 0x004F1A60(0x00506290) and 0x004F1A70(0x005062A0); the ninth is 0x004D14D0(0); the twelfth is 0x004E5360(0x004F64E0).
// ARITY: TaskType_Register here receives FIVE pushed dwords (0x00505F85, 0x00505F87, 0x00505F89, 0x00505F8A, 0x00505F8F) and the cleanup `add esp,0x44` at 0x00505FDA accounts for exactly 1+2+5+3+3+3 = 17 dwords, so this is one of the documented five-push sites and the transcription passes the four parameters the callee reads. For 0x004F1A80 the Lead's ruling applies (docs/fidelity/evidence/script-taskcontainer-append-arity-ruling.md): the callee reads six slots, each site here pushes four (value, slot, block, container, with `add esp,0x18` = 2 getter pushes + 4 append pushes), and the two missing slots are the GETTER's own deferred pushes still live above them - arg5 is the getter's first argument (7 or 6) and arg6 its second (0 at every site). Those derived values are passed explicitly at all nine sites, not omitted and not invented.
// Status/limits: TRANSCRIBED; BUILD_VERIFIED (single-file i686-w64-mingw32-g++ -std=c++20 -O2 -Wall -Wextra compile); STRUCTURALLY_VERIFIED against the 211-instruction listing with capstone and ndisasm agreeing inside the function; runtime UNOBSERVED. LIMITS: (a) the manifest record for this VA carries the signature `int* TextureModifier_Init(void)` and the same "Address-bound leaf instructions map the retail chunk walk onto the port's C++ helpers" rationale as 0x0052C0C0 and 0x004F8340 - false here too: it is not a leaf (41 calls) and no deliberate return value is set. The name is kept as the naming authority; neither the signature nor the rationale is adopted. (b) The frame is passed to 0x004F1A80 UNINITIALISED, exactly as retail does - nothing writes it before the appends. (c) The companion slots are byte-granular (E-128, E-56, E-44, E-52, E-24, E-32, E-31, E-16, E-27), so the record is not dword-spaced and the frame is transcribed as bytes. (d) 0x00B97DF8 is an existing tree global (terrain_loaders.hpp `g_b97df8`), so this body assigns it rather than defining it. Domain: core/, because the caller is core/native_5183d0.cpp (`call 0x00505F60` at 0x0051845C).

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
std::uint16_t g_a7a8f0 = 0U; // 0x00505FA1: mov word [0xa7a8f0],ax - the registered task type id
std::uint32_t g_a7a8ec = 0U; // 0x005060D4: mov dword [0xa7a8ec],eax - the task container

// NATIVE GAPS, arity measured from this body's push sequences; none is a declared prototype.
// 0x004B4720: 1 slot (push 0xa7acf8 0x00505F66). Named from its manifest record.
void Sys_StoreThreeWords_4B4720(void* target);
// 0x004B47E0: 2 slots (push 0x19111911 0x00505F70, push 0xa7acf8 0x00505F75). Implemented at core/native_4b47e0.cpp.
std::uint32_t Sub_4B47E0(void* target, std::uint32_t value);
// 0x004F1010: 2 slots. Named from its manifest record.
std::uint32_t Sys_GetGlobal32_4F1010(std::uint32_t a1, std::uint32_t a2);
// 0x004F0EF0: 2 slots. No manifest record.
std::uint32_t Sys_GetGlobal32_4F0EF0(std::uint32_t a1, std::uint32_t a2);
// 0x004F0FA0: 2 slots. Named from its manifest record.
std::uint32_t Core_GetRenderWordA_4F0FA0(std::uint32_t a1, std::uint32_t a2);
// 0x004D14D0: 1 slot (push 0 0x00506044). Named from its manifest record.
std::uint32_t Render_GetByte_4D14D0(std::uint32_t a1);
// 0x004F1950: 1 slot (push 9 0x005060CA). Named from its manifest record.
std::uint32_t Script_TaskContainer_Alloc(std::uint32_t a1);
// 0x004F1A80: SIX slots, per the Lead's ruling; the sites' four pushes plus the getter's two deferred pushes.
extern "C" void* Script_TaskContainer_Append(std::uint32_t container, void* block, void* slot,
                                             std::uint32_t value, std::uint32_t arg5, std::uint32_t arg6);
// 0x004F1A60 / 0x004F1A70: `mov al,[0x54e0e4]; ret` and `mov al,[0x54e0e5]; ret` - they read no stack argument, so the
// descriptor pointer each call site pushes is inert; the local arity records the push, not a read.
std::uint8_t Script_PhaseIdGetE4_4F1A60(std::uint32_t a1);
std::uint8_t Script_PhaseIdGetE5_4F1A70(std::uint32_t a1);
// 0x004E5360: 1 slot (push 0x4f64e0 0x00506080). Implemented at core/native_4e5360.cpp; the body reads no argument.
std::uint8_t Sub_4E5360(std::uint32_t a1);
// 0x004F1000: `mov eax,[0xA70C90]; ret` - two instructions, no argument read, so this takes none.
std::uint8_t Camera_Get_4F1000(void);

void NATIVE_DECL TextureModifier_Init(void)
{
    std::uint8_t frame[0xF0]; // 0x00505F60: sub esp,0xf0. frame[i] is E-0xf0+i; entry-relative offsets are stated at each use.

    // 0x00505F66-0x00505F6B: push 0xa7acf8; call 0x4b4720.
    Sys_StoreThreeWords_4B4720(reinterpret_cast<void*>(0x00A7ACF8U));
    // 0x00505F70-0x00505F7A: push 0x19111911; push 0xa7acf8; call 0x4b47e0.
    Sub_4B47E0(reinterpret_cast<void*>(0x00A7ACF8U), 0x19111911U);

    // 0x00505F7F: mov ax,word [0x54875c]; 0x00505F85-0x00505F8F: push 0; push 0; push eax; push 0xf0; push 0x54e7a4.
    // 0x0054E7A4 is the byte-exact name string "TextureModifier".
    const std::uint16_t type_id = TaskType_Register("TextureModifier", 0xF0U, g_54875c, 0U);
    g_a7a8f0 = type_id; // 0x00505FA1: mov word [0xa7a8f0],ax

    // 0x00505F99-0x00505FA7: event 0xd, handler 0x505f20.
    QTask_LinkQTaskEventHandler(g_a7a8f0, 0x0DU, reinterpret_cast<igi::retail::core::FrameHandler>(0x00505F20U));
    // 0x00505FB3-0x00505FBB: event 0, handler 0x505d50.
    QTask_LinkQTaskEventHandler(g_a7a8f0, 0U, reinterpret_cast<igi::retail::core::FrameHandler>(0x00505D50U));
    // 0x00505FC7-0x00505FCF: event 0x10, handler 0x505ed0.
    QTask_LinkQTaskEventHandler(g_a7a8f0, 0x10U, reinterpret_cast<igi::retail::core::FrameHandler>(0x00505ED0U));
    // 0x00505FDD-0x00505FE5: event 2, handler 0x506240, through the no-context variant.
    QTask_LinkQTaskEventHandlerNoContextFn(g_a7a8f0, 2U, reinterpret_cast<igi::retail::core::FrameHandler>(0x00506240U));
    // 0x00505FED-0x00506000: the event is 0x4f1a60(0x506290)'s result, the handler is 0x506290.
    QTask_LinkQTaskEventHandler(g_a7a8f0, Script_PhaseIdGetE4_4F1A60(0x00506290U),
                                reinterpret_cast<igi::retail::core::FrameHandler>(0x00506290U));
    // 0x00506008-0x0050601B: the event is 0x4f1a70(0x5062a0)'s result, the handler is 0x5062a0.
    QTask_LinkQTaskEventHandler(g_a7a8f0, Script_PhaseIdGetE5_4F1A70(0x005062A0U),
                                reinterpret_cast<igi::retail::core::FrameHandler>(0x005062A0U));
    // 0x00506026-0x0050602B: event 0xa, handler 0. 0x00506037-0x0050603C: event 0xb, handler 0.
    QTask_LinkQTaskEventHandler(g_a7a8f0, 0x0AU, reinterpret_cast<igi::retail::core::FrameHandler>(0U));
    QTask_LinkQTaskEventHandler(g_a7a8f0, 0x0BU, reinterpret_cast<igi::retail::core::FrameHandler>(0U));
    // 0x00506044-0x00506054: the event is 0x4d14d0(0)'s result, the handler is 0.
    QTask_LinkQTaskEventHandler(g_a7a8f0, Render_GetByte_4D14D0(0U),
                                reinterpret_cast<igi::retail::core::FrameHandler>(0U));
    // 0x0050605F-0x00506064: event 0xc, handler 0. 0x00506070-0x00506078: event 0xf, handler 0x505e40.
    QTask_LinkQTaskEventHandler(g_a7a8f0, 0x0CU, reinterpret_cast<igi::retail::core::FrameHandler>(0U));
    QTask_LinkQTaskEventHandler(g_a7a8f0, 0x0FU, reinterpret_cast<igi::retail::core::FrameHandler>(0x00505E40U));
    // 0x00506080-0x00506093: the event is 0x4e5360(0x4f64e0)'s result, the handler is 0x4f64e0.
    QTask_LinkQTaskEventHandler(g_a7a8f0, Sub_4E5360(0x004F64E0U),
                                reinterpret_cast<igi::retail::core::FrameHandler>(0x004F64E0U));
    // 0x0050609E-0x005060A3: event 8, handler 0. 0x005060AF-0x005060B4: event 4, handler 0.
    QTask_LinkQTaskEventHandler(g_a7a8f0, 8U, reinterpret_cast<igi::retail::core::FrameHandler>(0U));
    QTask_LinkQTaskEventHandler(g_a7a8f0, 4U, reinterpret_cast<igi::retail::core::FrameHandler>(0U));
    // 0x005060C0-0x005060C5: event 5, handler 0.
    QTask_LinkQTaskEventHandler(g_a7a8f0, 5U, reinterpret_cast<igi::retail::core::FrameHandler>(0U));

    // 0x005060CA-0x005060D4: push 9; call 0x4f1950; mov [0xa7a8ec],eax.
    g_a7a8ec = Script_TaskContainer_Alloc(9U);

    // The nine appends: four pushes each (value, slot, block, container), `add esp,0x18` = 2 getter pushes + 4 append
    // pushes, and the getter's two deferred pushes become arg5 and arg6. The frame is never written here, so the
    // record is passed uninitialised, as retail does.
    // 0x005060D9-0x005060F4: value 0x4f1010(7,0), slot E-128 (frame+0x70), deferred 7 and 0.
    Script_TaskContainer_Append(g_a7a8ec, frame + 0x00, frame + 0x70, Sys_GetGlobal32_4F1010(7U, 0U), 7U, 0U);
    // 0x005060FC-0x0050611A: value 0x4f0fa0(7,0), slot E-56 (frame+0xb8), deferred 7 and 0.
    Script_TaskContainer_Append(g_a7a8ec, frame + 0x00, frame + 0xB8, Core_GetRenderWordA_4F0FA0(7U, 0U), 7U, 0U);
    // 0x00506122-0x00506140: value 0x4f0fa0(7,0), slot E-44 (frame+0xc4), deferred 7 and 0.
    Script_TaskContainer_Append(g_a7a8ec, frame + 0x00, frame + 0xC4, Core_GetRenderWordA_4F0FA0(7U, 0U), 7U, 0U);
    // 0x00506148-0x00506166: value 0x4f0fa0(7,0), slot E-52 (frame+0xbc), deferred 7 and 0.
    Script_TaskContainer_Append(g_a7a8ec, frame + 0x00, frame + 0xBC, Core_GetRenderWordA_4F0FA0(7U, 0U), 7U, 0U);
    // 0x0050616E-0x0050618C: value 0x4f0fa0(7,0), slot E-24 (frame+0xd8), deferred 7 and 0.
    Script_TaskContainer_Append(g_a7a8ec, frame + 0x00, frame + 0xD8, Core_GetRenderWordA_4F0FA0(7U, 0U), 7U, 0U);
    // 0x00506194-0x005061B2: value 0x4f1000(7,0), slot E-32 (frame+0xd0), deferred 7 and 0.
    Script_TaskContainer_Append(g_a7a8ec, frame + 0x00, frame + 0xD0, Camera_Get_4F1000(), 7U, 0U);
    // 0x005061BA-0x005061D8: value 0x4f0ef0(6,0), slot E-31 (frame+0xd1), deferred 6 and 0.
    Script_TaskContainer_Append(g_a7a8ec, frame + 0x00, frame + 0xD1, Sys_GetGlobal32_4F0EF0(6U, 0U), 6U, 0U);
    // 0x005061E0-0x005061FE: value 0x4f0ef0(6,0), slot E-16 (frame+0xe0), deferred 6 and 0.
    Script_TaskContainer_Append(g_a7a8ec, frame + 0x00, frame + 0xE0, Sys_GetGlobal32_4F0EF0(6U, 0U), 6U, 0U);
    // 0x00506206-0x00506224: value 0x4f1000(6,0), slot E-27 (frame+0xd5), deferred 6 and 0.
    Script_TaskContainer_Append(g_a7a8ec, frame + 0x00, frame + 0xD5, Camera_Get_4F1000(), 6U, 0U);

    // 0x00506229: mov dword [0xb97df8],0 - the existing tree global g_b97df8 (terrain_loaders.hpp), reset here.
    igi::retail::terrain::g_b97df8 = 0;
    // 0x00506233: add esp,0x108 (0xf0 frame + the 6 argument dwords still pushed); 0x00506239: ret.
}

} // extern "C"

#undef NATIVE_DECL
