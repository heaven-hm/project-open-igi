// Native: Task_DefineSiren (behavior-derived placeholder name from the byte-identical name literal this body registers; retail symbol unknown);
//   retail VA 0x00449360-0x004494C8.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 361 bytes (0x169), 1 block,
//   102 instructions.
// Tools/evidence: radare2 6.2.0 af/afbj/pdf with bin.relocs.apply over the exact image - 1 block, 102 instructions, 0 edges (straight-line; the
//   `add esp,0x2F0` at 0x004494C2 followed by the ret is the only exit). The objdump listing build/retail-dis.txt was read for the same extent.
//   `ps` at 0x0053C860 (byte-identical to "Siren"). `px 4` at 0x0057C214 and 0x0057C21C gives zero for both. 0x00481350 was read from its own
//   bytes (`mov eax,[0x5C1184]; ret`, no argument), which is what makes the grow's argument order evidence rather than preference. The prologue is a
//   bare `sub esp,0x2D8` with NO saved-register pushes, so the frame-relative shift is ZERO. A push-by-push simulation closes exactly: 5 + 8*3 + 2 +
//   3*6 = 49 pushes = 196 bytes against `add esp,0x38` + five `add esp,0xC` + `add esp,8` + two `add esp,0x18` + the tail's (0x2F0 - 0x2D8) = 196
//   bytes; `esp` is 0 at the ret.
// Behavior/CFG: a 0x2D8-byte frame. Registers the "Siren" task type of size 0x2D8 (equal to its own frame) with parent Sys_GetGlobal16_481340()
//   into the word at 0x0057C214, links EIGHT handlers (four 0x00401400 and four 0x00401530), grows the shared container by three and stores the
//   returned pointer at 0x0057C21C, then appends THREE templates. **There is NO hash registration in this body** - the only body in this lane that
//   registers a type and links handlers but never calls 0x004B8810.
//   Links: 0x004492A0/e0, 0x004494D0/e1 and 0x00449330/e2 through the literal event ids 0, 1 and 2; then 0x004F1A60->0x00449600,
//   0x004F1A70->0x00449610, 0x004F1210->0x00449660, 0x004F1220->0x00449710 and 0x004E6610->0x004497B0.
//   Grow: 0x00449441 `push $0x3` happens BEFORE `call 0x00481350`, which takes no argument, so the 3 survives and becomes the SECOND argument of
//   0x004F1980 while the getter result pushed at 0x00449448 becomes the first: retail is `Grow(container, 3)`, and the single `add esp,8` consumes
//   both. Appends: THREE static sites through 0x004F1A80 with recorded frame deltas 0x238, 0x258 and 0x278 (argument 2 is frame+0x00 at every one),
//   argument 4 from 0x004F0FE0 (twice) and 0x004F0FC0, argument 5 always the literal 7 and argument 6 always zero.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. Census: 0x00401400 x4, 0x00401530 x4 (8 links), 0x00401900 x1, 0x00481340 x1, 0x00481350 x1,
//   0x004E6610 x1, 0x004F0FC0 x1, 0x004F0FE0 x2, 0x004F1210 x1, 0x004F1220 x1, 0x004F1980 x1, 0x004F1A60 x1, 0x004F1A70 x1, 0x004F1A80 x3 =
//   23 sites / 14 distinct.
//   Cleanup reconciliation, each `add esp` matched to the pushes it removes: 0x004493B9 `add esp,0x38` = the five registration pushes plus links 1-3;
//   FIVE `add esp,0xC` at 0x004493D4, 0x004493EE, 0x00449409, 0x00449424 and 0x0044943E = links 4-8, one each; 0x0044944E `add esp,8` = the grow's
//   two pushes; two `add esp,0x18` at 0x00449478 and 0x0044949D = appends 1-2; the tail `add esp,0x2F0` at 0x004494C2 = the 0x2D8 frame plus the
//   THIRD append's six outstanding pushes. Totals as above; the arithmetic closes.
//   **Three numbers: THREE static append sites, THREE records expected, and a GROW REQUEST of 3.** As in 0x00447DB0 the container is the shared array
//   at 0x005C1184 grown by this body, so its TOTAL capacity is not established here and is NOT claimed - what is corroborated is request == sites, and
//   the registered size equals the frame (0x2D8 both), which is a separate check that does hold here.
//   **The argument-3 `lea` position differs between the three appends and the deltas record it**: at appends 1 and 3 the `lea` executes with two
//   pushes outstanding (frame+0x238 and frame+0x278), while at append 2 the arg-4 `push %eax` at 0x00449484 executes FIRST so its `lea 0x264(%esp)`
//   resolves with three outstanding and is frame+0x258. Measured at a sibling's position that same displacement would have read 0x25C.
//   Documented shape differences, all value-preserving: (1) argument-less getters ignore the arguments retail pushes at their call sites and the tree's
//   transcriptions take no arguments, so those inert pushes are not reproduced EXCEPT where the same push carries a second role (the grow's count);
//   (2) the `add esp` cleanups are reproduced implicitly by the tree's calling convention rather than by explicit arithmetic, with the retail sizes
//   recorded above; (3) byte-valued getters are zero-extended into the 32-bit link arguments in the port while retail leaves the upper bits of eax
//   undefined - no effect on the links, both of which mask the event word.
//   Cross-references: parent getter 0x00481340 is the same one 0x00447DB0 registers under (the sentinel-reading parent). All EIGHT handler
//   immediates (0x004492A0, 0x004494D0, 0x00449330, 0x00449600, 0x00449610, 0x00449660, 0x00449710, 0x004497B0) have no transcription anywhere in
//   the tree.
//   Unestablished: why this type is not hash-registered while its lane neighbours are; the meaning of every recorded delta; and the eight untranscribed
//   handler bodies.

#include "igi/retail/core/task_types.hpp"

#include <cstdint>

// Callees, each with the return type and linkage of its own transcribed file. 0x00401400, 0x00401530 and 0x00401900 come from
// igi/retail/core/task_types.hpp (core/task_types.cpp) and are not redeclared here.
extern "C" std::uint16_t Sys_GetGlobal16_481340(void);     // 0x00481340: parent type getter, `mov ax,[0x54097C]` (sys/native_481340.cpp)
extern "C" std::uint32_t Sys_GetGlobal32_481350(void);     // 0x00481350: `mov eax,[0x5C1184]; ret` - takes NO argument (sys/native_481350.cpp)
extern "C" std::uint8_t Script_PhaseIdGetE4_4F1A60(void);  // 0x004F1A60 (script/native_4f1a60.cpp)
extern "C" std::uint8_t Script_PhaseIdGetE5_4F1A70(void);  // 0x004F1A70 (script/native_4f1a70.cpp)
extern "C" std::uint32_t Sys_GetGlobal32_4F0FC0(void);     // 0x004F0FC0 (sys/native_4f0fc0.cpp)
extern "C" std::uint32_t Sys_GetGlobal32_4F0FE0(void);     // 0x004F0FE0 (sys/native_4f0fe0.cpp)
extern "C" std::uint32_t Sys_GetGlobal32_4F1210(void);     // 0x004F1210 (sys/native_4f1210.cpp)
extern "C" std::uint32_t Sys_GetGlobal32_4F1220(void);     // 0x004F1220 (sys/native_4f1220.cpp)
extern "C" std::uint8_t Task_GetCachedEventId_4E6610(void); // 0x004E6610 (core/native_004e6610.cpp)
extern "C" void* Script_TaskContainer_Grow(std::int32_t* const container, std::int32_t const count); // 0x004F1980 (script/native_4f1980.cpp)
// 0x004F1A80 (script/native_4f1a80.cpp): arg5 is read as a byte (record +0x0C) and arg6 as a dword (record +0x08).
extern "C" void* Script_TaskContainer_Append(std::int32_t* const container, std::int32_t const arg2, std::int32_t const arg3,
                                             std::int32_t const arg4, std::int32_t const arg5, std::int32_t const arg6);

extern "C" {

// 0x0057C214: the "Siren" task type id (word). Image value 0 -> initialiser 0, symbol class `B`, writable `.data`: `.rdata` ends at 0x005358F6 and
// `.data` starts at 0x00536000, and this image has no `.bss`, so a zero-image global is `.data` with a zero initialiser and `B`/`D` is the symbol
// classification rather than the section. No other file in the tree owns it.
std::uint16_t g_57c214 = 0U;

// 0x0057C21C: the container pointer this body assigns from the grow. Image value 0 -> `B`. No other file in the tree owns it.
std::uint32_t g_57c21c = 0U;

void Task_DefineSiren()
{
    // 0x00449360: sub esp,0x2D8 - a plain frame: no saved-register pushes and no stack probe, so the frame-relative shift is ZERO.
    std::int32_t append_frame_base = 0; // retail frame slot at +0x00 - argument 2 of every append in this body
    const std::int32_t append_frame = reinterpret_cast<std::int32_t>(&append_frame_base);

    // 0x00449366: push 0; 0x00449368: push 0; 0x0044936A: call 0x481340 (parent type - reads the image-0x0181 sentinel at 0x0054097C);
    // 0x0044936F: push eax; 0x00449370: push 0x2D8; 0x00449375: push 0x0053C860; 0x0044937A: call 0x401900.
    const std::uint16_t siren_id = TaskType_Register("Siren",                     // 0x00449375: push 0x0053C860
                                                     0x2D8U,                      // 0x00449370: push 0x2D8 (object size, equal to the frame)
                                                     Sys_GetGlobal16_481340(),    // 0x0044936A: call 0x481340 (parent type)
                                                     0U);                         // 0x00449368: push 0
    ::g_57c214 = siren_id; // 0x00449387: mov word [0x57C214],ax

    // 0x44937F: push 0x4492A0 (handler, deferred argument 3);
    // 0x449384: push $0x0 (argument 2, literal event id);
    // 0x449386: push %eax (argument 1, still the id TaskType_Register returned); 0x44938D: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(siren_id), 0x0U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x4492A0U)));

    // 0x449398: push 0x4494D0 (handler, deferred argument 3);
    // 0x44939D: push $0x1 (argument 2, literal event id);
    // 0x449392: mov [0x57c214] into a register (argument 1, the id RELOADED from the global); 0x4493A0: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_57c214), 0x1U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x4494D0U)));

    // 0x4493AC: push 0x449330 (handler, deferred argument 3);
    // 0x4493B1: push $0x2 (argument 2, literal event id);
    // 0x4493A5: mov [0x57c214] into a register (argument 1, the id RELOADED from the global); 0x4493B4: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_57c214), 0x2U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x449330U)));

    // 0x4493BC: push 0x449600 (handler, deferred argument 3);
    // 0x4493C1: call Script_PhaseIdGetE4_4F1A60 (supplies argument 2, the event id);
    // 0x4493C6: mov [0x57c214] into a register (argument 1, the id RELOADED from the global); 0x4493CF: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57c214), static_cast<std::uint32_t>(Script_PhaseIdGetE4_4F1A60()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x449600U)));

    // 0x4493D7: push 0x449610 (handler, deferred argument 3);
    // 0x4493DC: call Script_PhaseIdGetE5_4F1A70 (supplies argument 2, the event id);
    // 0x4493E2: mov [0x57c214] into a register (argument 1, the id RELOADED from the global); 0x4493E9: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57c214), static_cast<std::uint32_t>(Script_PhaseIdGetE5_4F1A70()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x449610U)));

    // 0x4493F1: push 0x449660 (handler, deferred argument 3);
    // 0x4493F6: call Sys_GetGlobal32_4F1210 (supplies argument 2, the event id);
    // 0x4493FB: mov [0x57c214] into a register (argument 1, the id RELOADED from the global); 0x449404: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_57c214), static_cast<std::uint32_t>(Sys_GetGlobal32_4F1210()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x449660U)));

    // 0x44940C: push 0x449710 (handler, deferred argument 3);
    // 0x449411: call Sys_GetGlobal32_4F1220 (supplies argument 2, the event id);
    // 0x449416: mov [0x57c214] into a register (argument 1, the id RELOADED from the global); 0x44941F: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_57c214), static_cast<std::uint32_t>(Sys_GetGlobal32_4F1220()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x449710U)));

    // 0x449427: push 0x4497B0 (handler, deferred argument 3);
    // 0x44942C: call Task_GetCachedEventId_4E6610 (supplies argument 2, the event id);
    // 0x449432: mov [0x57c214] into a register (argument 1, the id RELOADED from the global); 0x449439: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57c214), static_cast<std::uint32_t>(Task_GetCachedEventId_4E6610()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x4497B0U)));

    // THE DEFERRED-PUSH INVERSION AGAIN: 0x00449441 `push $0x3` happens BEFORE `call 0x00481350`, which takes no argument
    // (`mov eax,[0x5C1184]; ret`), so the 3 survives that call and becomes the SECOND argument of 0x004F1980 while the getter result pushed at
    // 0x00449448 becomes the FIRST. Retail is Grow(container, 3); the single `add esp,8` at 0x0044944E consumes both, so only the callee's own body
    // settles the order. 0x00449451: mov [0x57C21C],eax.
    ::g_57c21c = reinterpret_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(
        Script_TaskContainer_Grow(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(Sys_GetGlobal32_481350())), 3))); // 0x00449441..0x00449451

    // 0x449456: push $0x0 (arg6); 0x449458: push $0x7 (arg5); 0x44945A: call Sys_GetGlobal32_4F0FE0 (arg4);
    // 0x44945F: lea 0x240(%esp) -> frame+0x238 (arg3); 0x44946C: lea 0xc(%esp) -> frame+0x0 (arg2); 0x449473: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57c21c)),
                                append_frame, append_frame + 0x238,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FE0()), 7, 0);

    // 0x44947B: push $0x0 (arg6); 0x44947D: push $0x7 (arg5); 0x44947F: call Sys_GetGlobal32_4F0FE0 (arg4);
    // 0x44948A: lea 0x264(%esp) -> frame+0x258 (arg3); 0x449491: lea 0xc(%esp) -> frame+0x0 (arg2); 0x449498: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57c21c)),
                                append_frame, append_frame + 0x258,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FE0()), 7, 0);

    // 0x4494A0: push $0x0 (arg6); 0x4494A2: push $0x7 (arg5); 0x4494A4: call Sys_GetGlobal32_4F0FC0 (arg4);
    // 0x4494A9: lea 0x280(%esp) -> frame+0x278 (arg3); 0x4494B6: lea 0xc(%esp) -> frame+0x0 (arg2); 0x4494BD: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57c21c)),
                                append_frame, append_frame + 0x278,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FC0()), 7, 0);

    // **This body has NO hash registration**: it never calls 0x004B8810, so unlike every other type registered in this lane the "Siren" type is not
    // entered in the task-type hash.
    // 0x004494C2: add esp,0x2F0 - the 0x2D8 frame plus the third append's six outstanding pushes; 0x004494C8: ret.
} // end of Task_DefineSiren

} // extern "C"
