// Native: Task_DefineAmbientArea (behavior-derived placeholder name from the byte-identical name literal this body registers; retail symbol unknown);
//   retail VA 0x0044C130-0x0044C2B7.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 392 bytes (0x188), 1 block,
//   118 instructions.
// Tools/evidence: radare2 6.2.0 af/afbj/pdf with bin.relocs.apply over the exact image - 1 block, 118 instructions, 0 edges (straight-line; the
//   `add esp,0x228` at 0x0044C2B1 followed by the ret is the only exit). The objdump listing build/retail-dis.txt was read for the same extent.
//   `ps` at 0x0053C90C (byte-identical to "AmbientArea") and at 0x0053C918 (byte-identical to "johan", the model name). `px 4` gives zero at
//   0x0057C734, 0x0057C738 and 0x0057C73C. The prologue is a bare `sub esp,0x210` with NO saved-register pushes, so the frame-relative shift is
//   ZERO. A push-by-push simulation closes exactly: 1 (model name) + 5 (registration) + 15 (five links) + 1 (capacity) + 36 (six appends) = 58
//   pushes = 232 bytes against `add esp,4` + `add esp,0x38` + `add esp,0xC` + `add esp,0x10` + five `add esp,0x18` + the tail's
//   (0x228 - 0x210) = 232 bytes; `esp` is 0 at the ret.
// Behavior/CFG: a 0x210-byte frame. The FIRST action loads a virtual LOD model by name: 0x0044C136 `push 0x0053C918` ("johan"),
//   `call 0x004CEBA0` (MTP_AddLODVirModel_4CEBA0), `add esp,4`, and the returned pointer is stored as a dword at 0x0057C73C. It then registers
//   the "AmbientArea" task type of size 0x210 (equal to its own frame) with parent Sys_GetGlobal16_5034D0() into the word at 0x0057C734, links
//   FIVE handlers (three 0x00401400 and two 0x00401530), allocates a SIX-record container into 0x0057C738 and appends six templates.
//   Links: 0x0044C2C0/e0, 0x0044C370/e1 and 0x0044C700/e2 through the literal event ids 0, 1 and 2; then 0x004F1A60->0x0044C730 and
//   0x004F1A70->0x0044C740.
//   Appends: SIX static sites through 0x004F1A80 with recorded frame deltas 0x20, 0x50, 0x38, 0x80, 0x88 and 0x94 (argument 2 is frame+0x00 at
//   every one), argument 4 from 0x004F1010, 0x004F0F70, 0x004F0F90, 0x004F0F80 (twice) and 0x004F0FF0, argument 5 always the literal 7 and
//   argument 6 always zero. The delta sequence is NOT monotonic: the second site (0x50) is above the third (0x38), so the recorded values are the
//   measured ones and no ordering is assumed. At all six sites the arg-3 `lea` executes before the arg-4 `push %eax`, so every delta here is
//   measured with exactly two pushes outstanding.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. Census: 0x00401400 x3, 0x00401530 x2 (5 links), 0x00401900 x1, 0x004CEBA0 x1, 0x004F0F70 x1,
//   0x004F0F80 x2, 0x004F0F90 x1, 0x004F0FF0 x1, 0x004F1010 x1, 0x004F1950 x1, 0x004F1A60 x1, 0x004F1A70 x1, 0x004F1A80 x6, 0x005034D0 x1 =
//   23 sites / 14 distinct.
//   Cleanup reconciliation, each `add esp` matched to the pushes it removes: `add esp,4` at 0x0044C140 = the model-name push; `add esp,0x38` at
//   0x0044C19B = the five registration pushes plus links 1-3 (fourteen dwords); `add esp,0xC` at 0x0044C1B6 = link 4; `add esp,0x10` at 0x0044C1D7
//   = link 5's three pushes, which have no cleanup of their own, PLUS the capacity push; five `add esp,0x18` at 0x0044C1FE, 0x0044C220, 0x0044C242,
//   0x0044C267 and 0x0044C28C = appends 1-5; the tail `add esp,0x228` at 0x0044C2B1 = the 0x210 frame plus the SIXTH append's six outstanding
//   pushes. Totals as above; the arithmetic closes.
//   **Three numbers: SIX static append sites, SIX records expected, capacity 6 by `push $0x6` at 0x0044C1D0.** The container is allocated by this
//   body, so the capacity is established here and `records == capacity` is corroborated by a literal in a different instruction from the six sites.
//   **NO HASH REGISTRATION: this body never calls 0x004B8810. That is the THIRD instance in this lane (after 0x00449360 Siren and 0x00449D30
//   GlassShatter), so the pattern is now three bodies deep and is reported as a pattern.**
//   **The registered size equals the frame here (0x210 both), which is the append-shape case**: this body builds its six templates on the stack and
//   appends them, so the frame carries the template object - the same consequence recorded in 0x00447DB0 and 0x00449360, and NOT a property of
//   registration (0x00449D30 registers 0x2B048 against a 0x14 frame).
//   The registration pushes FIVE dwords even though 0x00401900 reads only FOUR arguments (established from the callee's own bytes and recorded in
//   native_449d30.cpp): the fifth dword is pushed and cleaned by this call site and never read by the callee.
//   Documented shape differences, all value-preserving: (1) the model pointer's dword store of a 32-bit `eax` is reproduced by casting the model
//   pointer to `std::uint32_t`; (2) argument-less getters ignore the arguments retail pushes at their call sites and the tree's transcriptions take no
//   arguments, so those inert pushes are not reproduced; (3) the `add esp` cleanups are reproduced implicitly by the tree's calling convention rather
//   than by explicit arithmetic, with the retail sizes recorded above; (4) byte-valued getters are zero-extended into the 32-bit link arguments in the
//   port while retail leaves the upper bits of eax undefined - no effect on the links, both of which mask the event word.
//   Cross-references: the model name "johan" is an asset identifier, not a symbol name; 0x004CEBA0 is transcribed at model/native_4ceba0.cpp and
//   returns `std::uint8_t*`; 0x004F0F80 is Camera_Get_4F0F80 (render/camera_state.hpp).
//   Unestablished: what the "johan" model pointer at 0x0057C73C feeds (nothing in this body reads it back); the meaning of every recorded delta; and
//   the FIVE handler immediates (0x0044C2C0, 0x0044C370, 0x0044C700, 0x0044C730, 0x0044C740), none of which has a transcription anywhere in the
//   tree.

#include "igi/retail/core/task_types.hpp"

#include <cstdint>

// Callees, each with the return type and linkage of its own transcribed file. 0x00401400, 0x00401530 and 0x00401900 come from
// igi/retail/core/task_types.hpp (core/task_types.cpp) and are not redeclared here.
extern "C" std::uint16_t Sys_GetGlobal16_5034D0(void);      // 0x005034D0: parent type getter (sys/native_5034d0.cpp)
extern "C" std::uint8_t* MTP_AddLODVirModel_4CEBA0(char const* name); // 0x004CEBA0: loads a virtual LOD model by name (model/native_4ceba0.cpp)
extern "C" std::uint32_t Sys_GetGlobal32_4F0F70(void);      // 0x004F0F70 (sys/native_4f0f70.cpp)
extern "C" std::uint32_t Camera_Get_4F0F80(void);           // 0x004F0F80 (render/native_4f0f80.cpp)
extern "C" std::uint32_t Sys_GetGlobal32_4F0F90(void);      // 0x004F0F90 (sys/native_4f0f90.cpp)
extern "C" std::uint32_t Sys_GetGlobal32_4F0FF0(void);      // 0x004F0FF0 (sys/native_4f0ff0.cpp)
extern "C" std::uint32_t Sys_GetGlobal32_4F1010(void);      // 0x004F1010 (sys/native_4f1010.cpp)
extern "C" std::uint8_t Script_PhaseIdGetE4_4F1A60(void);   // 0x004F1A60 (script/native_4f1a60.cpp)
extern "C" std::uint8_t Script_PhaseIdGetE5_4F1A70(void);   // 0x004F1A70 (script/native_4f1a70.cpp)
extern "C" void* Script_TaskContainer_Alloc(std::int32_t const count); // 0x004F1950 (script/native_4f1950.cpp)
// 0x004F1A80 (script/native_4f1a80.cpp): arg5 is read as a byte (record +0x0C) and arg6 as a dword (record +0x08).
extern "C" void* Script_TaskContainer_Append(std::int32_t* const container, std::int32_t const arg2, std::int32_t const arg3,
                                             std::int32_t const arg4, std::int32_t const arg5, std::int32_t const arg6);

extern "C" {

// 0x0057C734: the "AmbientArea" task type id (word). Image value 0 -> initialiser 0, symbol class `B`, writable `.data`: `.rdata` ends at
// 0x005358F6 and `.data` starts at 0x00536000, and this image has no `.bss`, so a zero-image global is `.data` with a zero initialiser and `B`/`D`
// is the symbol classification rather than the section. No other file in the tree owns it.
std::uint16_t g_57c734 = 0U;

// 0x0057C738: the "AmbientArea" parameter container, six records (capacity 6, exactly the number of appends in this body). Image value 0 -> `B`.
std::uint32_t g_57c738 = 0U;

// 0x0057C73C: the dword that receives the virtual LOD model pointer 0x004CEBA0 returns for the name "johan". Retail stores a 32-bit `eax` here, so
// the port stores the pointer's 32-bit value rather than a wider type. Image value 0 -> `B`, writable. No other file in the tree owns it and nothing
// in THIS body reads it back, so its consumer is unestablished.
std::uint32_t g_57c73c = 0U;

void Task_DefineAmbientArea()
{
    // 0x0044C130: sub esp,0x210 - a plain frame: no saved-register pushes and no stack probe, so the frame-relative shift is ZERO.
    std::int32_t append_frame_base = 0; // retail frame slot at +0x00 - argument 2 of every append in this body
    const std::int32_t append_frame = reinterpret_cast<std::int32_t>(&append_frame_base);

    // 0x0044C136: push 0x0053C918 ("johan"); 0x0044C13B: call 0x004CEBA0; 0x0044C140: add esp,4; 0x0044C143: mov [0x57C73C],eax.
    // The FIRST action of the body is this model load, before the registration.
    ::g_57c73c = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(MTP_AddLODVirModel_4CEBA0("johan"))); // 0x0044C143

    // 0x0044C148: push 0; 0x0044C14A: push 0 - the task-list argument and a fifth dword the callee never reads (see the provenance block);
    // 0x0044C14C: call 0x005034D0 (parent type); 0x0044C151: push %eax; 0x0044C152: push 0x210; 0x0044C157: push 0x0053C90C;
    // 0x0044C15C: call 0x00401900.
    const std::uint16_t ambient_area_id = TaskType_Register("AmbientArea",            // 0x0044C157: push 0x0053C90C
                                                            0x210U,                   // 0x0044C152: push 0x210 (object size, equal to the frame)
                                                            Sys_GetGlobal16_5034D0(), // 0x0044C14C: call 0x005034D0 (parent type)
                                                            0U);                      // 0x0044C14A: push 0
    ::g_57c734 = ambient_area_id; // 0x0044C169: mov word [0x57C734],ax

    // 0x44C161: push 0x44C2C0 (handler, deferred argument 3);
    // 0x44C166: push $0x0 (argument 2, literal event id);
    // 0x44C168: push %eax (argument 1, still the id TaskType_Register returned); 0x44C16F: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(ambient_area_id), 0x0U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x44C2C0U)));

    // 0x44C17A: push 0x44C370 (handler, deferred argument 3);
    // 0x44C17F: push $0x1 (argument 2, literal event id);
    // 0x44C174: mov [0x57c734] into a register (argument 1, the id RELOADED from the global); 0x44C182: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_57c734), 0x1U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x44C370U)));

    // 0x44C18E: push 0x44C700 (handler, deferred argument 3);
    // 0x44C193: push $0x2 (argument 2, literal event id);
    // 0x44C187: mov [0x57c734] into a register (argument 1, the id RELOADED from the global); 0x44C196: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_57c734), 0x2U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x44C700U)));

    // 0x44C19E: push 0x44C730 (handler, deferred argument 3);
    // 0x44C1A3: call Script_PhaseIdGetE4_4F1A60 (supplies argument 2, the event id);
    // 0x44C1A8: mov [0x57c734] into a register (argument 1, the id RELOADED from the global); 0x44C1B1: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57c734), static_cast<std::uint32_t>(Script_PhaseIdGetE4_4F1A60()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x44C730U)));

    // 0x44C1B9: push 0x44C740 (handler, deferred argument 3);
    // 0x44C1BE: call Script_PhaseIdGetE5_4F1A70 (supplies argument 2, the event id);
    // 0x44C1C4: mov [0x57c734] into a register (argument 1, the id RELOADED from the global); 0x44C1CB: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57c734), static_cast<std::uint32_t>(Script_PhaseIdGetE5_4F1A70()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x44C740U)));

    // 0x0044C1D0: push $0x6 (container CAPACITY, six records - the same literal as the number of appends); 0x0044C1D2: call 0x004F1950;
    // 0x0044C1D7: add esp,0x10 (link 5's three outstanding pushes plus this capacity push); 0x0044C1DA: mov [0x57C738],eax.
    ::g_57c738 = reinterpret_cast<std::uint32_t>(Script_TaskContainer_Alloc(6)); // 0x0044C1D0: push 0x6

    // 0x44C1DC: push $0x0 (arg6); 0x44C1DE: push $0x7 (arg5); 0x44C1E3: call Sys_GetGlobal32_4F1010 (arg4);
    // 0x44C1E8: lea 0x28(%esp) -> frame+0x20 (arg3); 0x44C1F2: lea 0xc(%esp) -> frame+0x0 (arg2); 0x44C1F9: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57c738)),
                                append_frame, append_frame + 0x20,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F1010()), 7, 0);

    // 0x44C1FE: push $0x0 (arg6); 0x44C200: push $0x7 (arg5); 0x44C205: call Sys_GetGlobal32_4F0F70 (arg4);
    // 0x44C20A: lea 0x58(%esp) -> frame+0x50 (arg3); 0x44C214: lea 0xc(%esp) -> frame+0x0 (arg2); 0x44C21B: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57c738)),
                                append_frame, append_frame + 0x50,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F70()), 7, 0);

    // 0x44C220: push $0x0 (arg6); 0x44C222: push $0x7 (arg5); 0x44C227: call Sys_GetGlobal32_4F0F90 (arg4);
    // 0x44C22C: lea 0x40(%esp) -> frame+0x38 (arg3); 0x44C236: lea 0xc(%esp) -> frame+0x0 (arg2); 0x44C23D: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57c738)),
                                append_frame, append_frame + 0x38,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F90()), 7, 0);

    // 0x44C245: push $0x0 (arg6); 0x44C247: push $0x7 (arg5); 0x44C249: call Camera_Get_4F0F80 (arg4);
    // 0x44C24E: lea 0x88(%esp) -> frame+0x80 (arg3); 0x44C25B: lea 0xc(%esp) -> frame+0x0 (arg2); 0x44C262: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57c738)),
                                append_frame, append_frame + 0x80,
                                static_cast<std::int32_t>(Camera_Get_4F0F80()), 7, 0);

    // 0x44C26A: push $0x0 (arg6); 0x44C26C: push $0x7 (arg5); 0x44C26E: call Camera_Get_4F0F80 (arg4);
    // 0x44C273: lea 0x90(%esp) -> frame+0x88 (arg3); 0x44C280: lea 0xc(%esp) -> frame+0x0 (arg2); 0x44C287: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57c738)),
                                append_frame, append_frame + 0x88,
                                static_cast<std::int32_t>(Camera_Get_4F0F80()), 7, 0);

    // 0x44C28F: push $0x0 (arg6); 0x44C291: push $0x7 (arg5); 0x44C293: call Sys_GetGlobal32_4F0FF0 (arg4);
    // 0x44C298: lea 0x9c(%esp) -> frame+0x94 (arg3); 0x44C2A5: lea 0xc(%esp) -> frame+0x0 (arg2); 0x44C2AC: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57c738)),
                                append_frame, append_frame + 0x94,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FF0()), 7, 0);

    // 0x0044C2B1: add esp,0x228 - the 0x210 frame plus the sixth append's six outstanding pushes; 0x0044C2B7: ret.
    // **This body never calls 0x004B8810: "AmbientArea" is NOT hash-registered (third instance in this lane).**
} // end of Task_DefineAmbientArea

} // extern "C"
