// Native: Task_DefineGenericTBA (behavior-derived placeholder name from the byte-identical name literal this body registers; retail symbol unknown); retail VA 0x00447DB0-0x00447FE9.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 570 bytes (0x23A), 1 block,
//   157 instructions.
// Tools/evidence: radare2 6.2.0 af/afbj/pdf with bin.relocs.apply over the exact image - 1 block, 157 instructions, 0 edges (straight-line; the
//   `add esp,0x2C0` at 0x00447FE3 followed by the ret is the only exit). The objdump listing build/retail-dis.txt was read for the same extent.
//   `ps` at 0x0053C7E0 (byte-identical to "GenericTBA"). `px 4` gives 0x0181 at 0x0053C7DC (the word-sized task-id slot's UNREGISTERED sentinel,
//   the same image value established for 0x0053C680 and 0x0054097C) and zero at 0x0057C1F0, 0x0057C1F4 and 0x0057C1FC. The three non-obvious
//   callees were read from their own bytes before being trusted: 0x004871B0 is `mov eax,0x5C8A4C; ret` (returns a block pointer), 0x004F0F00 is
//   `mov eax,[0xA70C70]; ret` (no argument), and 0x00481350 is `mov eax,[0x5C1184]; ret` (no argument) - the last one matters because of the
//   deferred push described below. The prologue is a bare `sub esp,0x2A8` with NO saved-register pushes, so the frame-relative shift is ZERO.
// Behavior/CFG: a 0x2A8-byte frame. Before anything else the body stores the literal 10 into the dword at 0x0057C1F0 and stores the pointer that
//   0x004871B0 returns (0x005C8A4C) into 0x0057C1F4. It then registers the "GenericTBA" task type of size 0x2A8 (equal to its own frame) with parent
//   Sys_GetGlobal16_481340() - which reads the image-0x0181 sentinel, so this type's registered parent is "no parent yet" - and stores the id into the
//   word at 0x0053C7DC. Twelve handlers are linked (eight 0x00401400, four 0x00401530): 0x00447FF0/e0, 0x004480B0/e1 and 0x00448160/e2 through the
//   literal event ids 0, 1 and 2; then 0x004F1A60->0x004481F0, 0x004F1A70->0x00448200, 0x004F1210->0x004482D0, 0x004F1220->0x004483F0,
//   0x004167F0->0x00443C80, 0x00416810->0x0051BAA0, 0x00416800->0x004484F0, 0x00416820->0x00448510 and 0x00467D90->0x00443DC0. It then grows the
//   shared container by five and stores the returned pointer at 0x0057C1FC, and appends FIVE records through 0x004F1A80 with recorded frame deltas
//   0x238, 0x258, 0x24A, 0x24C and 0x250 (argument 2 is frame+0x00 at every one), argument 4 from 0x004F1000, 0x004F0FC0, 0x004F0FB0, 0x004F0F50 and
//   0x004F0F00 respectively, argument 5 always the literal 7, and argument 6 zero except at the LAST append where it is the address of 0x0057C1F0.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. Census: 0x00401400 x8, 0x00401530 x4 (12 links), 0x00401900 x1, 0x004167F0 x1, 0x00416800 x1,
//   0x00416810 x1, 0x00416820 x1, 0x00467D90 x1, 0x00481340 x1, 0x00481350 x1, 0x004871B0 x1, 0x004F0F00 x1, 0x004F0F50 x1, 0x004F0FB0 x1,
//   0x004F0FC0 x1, 0x004F1000 x1, 0x004F1210 x1, 0x004F1220 x1, 0x004F1980 x1, 0x004F1A60 x1, 0x004F1A70 x1, 0x004F1A80 x5 = 36 sites / 22 distinct.
//   Cleanup reconciliation, each `add esp` matched to the pushes it removes: 0x00447E1D `add esp,0x38` = the five registration pushes plus links 1-3;
//   NINE `add esp,0xC` at 0x00447E38, 0x00447E52, 0x00447E6D, 0x00447E88, 0x00447EA2, 0x00447EBD, 0x00447ED8, 0x00447EF2 and 0x00447F0D = links
//   4-12, one each; 0x00447F1D `add esp,8` = the GROW's two pushes (see below); FOUR `add esp,0x18` at 0x00447F48, 0x00447F6E, 0x00447F94 and
//   0x00447FBA = appends 1-4, and the FIFTH append's six pushes are folded into the tail `add esp,0x2C0` at 0x00447FE3, which is the 0x2A8 frame
//   plus those six (0x18). Totals: 5 + 12*3 + 2 + 5*6 = 73 pushes = 292 bytes against 0x38 + 9*0xC + 8 + 4*0x18 + (0x2C0 - 0x2A8) = 292 bytes;
//   `esp` is 0 at the ret.
//   **The GROW site is the deferred-push inversion in its cleanest form:** 0x00447F10 `push $0x5` happens BEFORE `call 0x00481350`, and 0x00481350
//   takes no argument (`mov eax,[0x5C1184]; ret`), so the 5 survives the call and becomes the SECOND argument of 0x004F1980 while the getter's result
//   pushed at 0x00447F17 becomes the FIRST: retail is `Grow(container, 5)`, but reading the two pushes in source order would give `Grow(5, container)`.
//   Both arguments are consumed by one `add esp,8`, so the census closes either way and only the callee's own body settles the order.
//   **Three numbers, and the third one is not "capacity": FIVE static append sites, FIVE records expected, and a GROW REQUEST of 5.** The container
//   this body fills is the shared array whose pointer lives at 0x005C1184, grown by this body - so the container's TOTAL capacity after the growth is
//   not established by anything in this function and is NOT claimed. What is corroborated is request == sites; "capacity == records" would be
//   unprovable here and is not asserted.
//   **External definitions this body writes but does not own** (both declared `extern` here; the writer-owns rule says their definitions should
//   migrate to this file, flagged to the Lead rather than changed): 0x0053C7DC is defined by sys/native_448550.cpp as `std::uint16_t g_53c7dc;` -
//   a zero-initialising tentative definition, while the retail image holds 0x0181, so that initialiser is WRONG (the same defect class found earlier
//   at 0x0053C680); 0x0057C1FC is defined by sys/native_448560.cpp as `std::uint32_t g_57c1fc;`, which is correct because its image value is zero.
//   New definitions owned here: 0x0057C1F0 (the dword stored with 10, later passed BY ADDRESS as the last append's argument 6) and 0x0057C1F4 (the
//   0x005C8A4C block pointer). Both image-zero, both `B`, no other owner.
//   Cross-references: handler 0x00443DC0 is shared with native_443680.cpp / native_443ed0.cpp / native_4464c0.cpp; 0x0051BAA0 is a handler immediate
//   with no transcription in the tree; 0x00443C80 likewise. Parent getter 0x00481340 reads 0x0054097C, the same slot whose image-0x0181 value makes
//   the sentinel meaning explicit.
//   Documented shape differences, all value-preserving: (1) argument-less getters ignore the arguments retail pushes at their call sites and the tree's
//   transcriptions take no arguments, so those inert pushes are not reproduced EXCEPT where the same push carries a second role (the grow's count, the
//   final append's argument 5); (2) the `add esp` cleanups are reproduced implicitly by the tree's calling convention rather than by explicit
//   arithmetic, with the retail sizes recorded above; (3) byte-valued getters are zero-extended into the 32-bit link arguments in the port while retail
//   leaves the upper bits of eax undefined - no effect on the links, both of which mask the event word; (4) the two prologue stores are ordered before
//   the registration exactly as retail does, because the last append reads 0x0057C1F0 by address.
//   Unestablished: the meaning of the literal 10 stored at 0x0057C1F0 (it is passed by address, so it is presumably an in/out count, but nothing in
//   this body reads it back); what 0x005C8A4C is (only its address is taken); the meaning of every recorded delta; the seven untranscribed handler
//   immediates (0x00447FF0, 0x004480B0, 0x00448160, 0x004481F0, 0x00448200, 0x004482D0, 0x004483F0, 0x004484F0, 0x00448510, 0x00443C80,
//   0x0051BAA0, 0x00443DC0); and the caller of 0x00448550/0x00448560, the two getters that read the slots this body writes.

#include "igi/retail/core/task_types.hpp"

#include <cstdint>

// Callees, each with the return type and linkage of its own transcribed file. 0x00401400, 0x00401530 and 0x00401900 come from
// igi/retail/core/task_types.hpp (core/task_types.cpp) and are not redeclared here.
extern "C" std::uint16_t Sys_GetGlobal16_481340(void);   // 0x00481340: parent type getter, `mov ax,[0x54097C]` (sys/native_481340.cpp)
extern "C" std::uint32_t Sys_GetGlobal32_481350(void);   // 0x00481350: `mov eax,[0x5C1184]; ret` - takes NO argument (sys/native_481350.cpp)
extern "C" std::uint8_t* Sys_GetBlock5C8A4C_4871B0(void); // 0x004871B0: `mov eax,0x5C8A4C; ret` (sys/native_4871b0.cpp)
extern "C" std::uint32_t Sys_GetGlobal32_4F0F00(void);   // 0x004F0F00: `mov eax,[0xA70C70]; ret` (sys/native_4f0f00.cpp)
extern "C" std::uint32_t Sys_GetGlobal32_4F0F50(void);   // 0x004F0F50 (sys/native_4f0f50.cpp)
extern "C" std::uint32_t Sys_GetGlobal32_4F0FB0(void);   // 0x004F0FB0 (sys/native_4f0fb0.cpp)
extern "C" std::uint32_t Sys_GetGlobal32_4F0FC0(void);   // 0x004F0FC0 (sys/native_4f0fc0.cpp)
extern "C" std::uint32_t Camera_Get_4F1000(void);        // 0x004F1000 (render/native_4f1000.cpp)
extern "C" std::uint32_t Sys_GetGlobal32_4F1210(void);   // 0x004F1210 (sys/native_4f1210.cpp)
extern "C" std::uint32_t Sys_GetGlobal32_4F1220(void);   // 0x004F1220 (sys/native_4f1220.cpp)
extern "C" std::uint8_t Script_PhaseIdGetE4_4F1A60(void); // 0x004F1A60 (script/native_4f1a60.cpp)
extern "C" std::uint8_t Script_PhaseIdGetE5_4F1A70(void); // 0x004F1A70 (script/native_4f1a70.cpp)
extern "C" std::uint8_t Sys_GetSettingByte_4167F0(void);  // 0x004167F0 (config/native_4167f0.cpp)
extern "C" std::uint8_t Sys_GetSettingByte_416800(void);  // 0x00416800 (config/native_416800.cpp)
extern "C" std::uint8_t Sys_GetSettingByte_416810(void);  // 0x00416810 (config/native_416810.cpp)
extern "C" std::uint8_t Sys_GetSettingByte_416820(void);  // 0x00416820 (config/native_416820.cpp)
extern "C" std::uint8_t Task_GetCachedEventId_467D90(void); // 0x00467D90 (core/native_00467d90.cpp)
extern "C" void* Script_TaskContainer_Grow(std::int32_t* const container, std::int32_t const count); // 0x004F1980 (script/native_4f1980.cpp)
// 0x004F1A80 (script/native_4f1a80.cpp): arg5 is read as a byte (record +0x0C) and arg6 as a dword (record +0x08).
extern "C" void* Script_TaskContainer_Append(std::int32_t* const container, std::int32_t const arg2, std::int32_t const arg3,
                                             std::int32_t const arg4, std::int32_t const arg5, std::int32_t const arg6);

// Slots written by this body but DEFINED by another file - declared here only, never defined here (one definition per VA).
// 0x0053C7DC: the "GenericTBA" task type id word. Defined by sys/native_448550.cpp as a zero-initialising tentative definition, while the retail
//   image holds 0x0181. The initialiser belongs to that file; the writer-owns rule says the definition should migrate to this one, which is recorded
//   in the provenance block and left to the Lead rather than changed here.
extern std::uint16_t g_53c7dc;
// 0x0057C1FC: the container pointer this body assigns. Defined by sys/native_448560.cpp (correct: image value zero).
extern std::uint32_t g_57c1fc;

extern "C" {

// 0x0057C1F0: the dword this body stores the literal 10 into before registering, and later passes BY ADDRESS as the last append's argument 6. Image
// value 0 -> initialiser 0, symbol class `B`, writable `.data`: `.rdata` ends at 0x005358F6 and `.data` starts at 0x00536000, and this image has no
// `.bss`, so a zero-image global is `.data` with a zero initialiser and `B`/`D` is the symbol classification rather than the section. No other file
// owns it.
std::uint32_t g_57c1f0 = 0U;

// 0x0057C1F4: receives the block pointer 0x005C8A4C that 0x004871B0 returns. Image value 0 -> `B`. No other file owns it.
std::uint32_t g_57c1f4 = 0U;

void Task_DefineGenericTBA()
{
    // 0x00447DB0: sub esp,0x2A8 - a plain frame: no saved-register pushes and no stack probe, so the frame-relative shift is ZERO.
    std::int32_t append_frame_base = 0; // retail frame slot at +0x00 - argument 2 of every append in this body
    const std::int32_t append_frame = reinterpret_cast<std::int32_t>(&append_frame_base);

    // 0x00447DB6: movl $0xa,0x57C1F0 (a dword initialised to 10, later passed BY ADDRESS as the last append's argument 6);
    // 0x00447DC0: call 0x4871B0 (`mov eax,0x5C8A4C; ret`); 0x00447DC9: mov [0x57C1F4],eax.
    ::g_57c1f0 = 0x0000000AU;
    ::g_57c1f4 = reinterpret_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Sys_GetBlock5C8A4C_4871B0()));

    // 0x00447DC5: push 0; 0x00447DC7: push 0; 0x00447DCE: call 0x481340 (parent type - reads the image-0x0181 sentinel at 0x0054097C);
    // 0x00447DD3: push eax; 0x00447DD4: push 0x2A8; 0x00447DD9: push 0x0053C7E0; 0x00447DDE: call 0x401900.
    const std::uint16_t generic_tba_id = TaskType_Register("GenericTBA",              // 0x00447DD9: push 0x0053C7E0
                                                           0x2A8U,                    // 0x00447DD4: push 0x2A8 (object size, equal to the frame)
                                                           Sys_GetGlobal16_481340(),  // 0x00447DCE: call 0x481340 (parent type)
                                                           0U);                       // 0x00447DC7: push 0
    ::g_53c7dc = generic_tba_id; // 0x00447DEB: mov word [0x53C7DC],ax

    // 0x447DE3: push 0x447FF0 (handler, deferred argument 3);
    // 0x447DE8: push $0x0 (argument 2, literal event id);
    // 0x447DEA: push %eax (argument 1, still the id TaskType_Register returned); 0x447DF1: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(generic_tba_id), 0x0U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x447FF0U)));

    // 0x447DFC: push 0x4480B0 (handler, deferred argument 3);
    // 0x447E01: push $0x1 (argument 2, literal event id);
    // 0x447DF6: mov [0x53c7dc] into a register (argument 1, the id RELOADED from the global); 0x447E04: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_53c7dc), 0x1U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x4480B0U)));

    // 0x447E10: push 0x448160 (handler, deferred argument 3);
    // 0x447E15: push $0x2 (argument 2, literal event id);
    // 0x447E09: mov [0x53c7dc] into a register (argument 1, the id RELOADED from the global); 0x447E18: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_53c7dc), 0x2U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x448160U)));

    // 0x447E20: push 0x4481F0 (handler, deferred argument 3);
    // 0x447E25: call Script_PhaseIdGetE4_4F1A60 (supplies argument 2, the event id);
    // 0x447E2A: mov [0x53c7dc] into a register (argument 1, the id RELOADED from the global); 0x447E33: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_53c7dc), static_cast<std::uint32_t>(Script_PhaseIdGetE4_4F1A60()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x4481F0U)));

    // 0x447E3B: push 0x448200 (handler, deferred argument 3);
    // 0x447E40: call Script_PhaseIdGetE5_4F1A70 (supplies argument 2, the event id);
    // 0x447E46: mov [0x53c7dc] into a register (argument 1, the id RELOADED from the global); 0x447E4D: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_53c7dc), static_cast<std::uint32_t>(Script_PhaseIdGetE5_4F1A70()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x448200U)));

    // 0x447E55: push 0x4482D0 (handler, deferred argument 3);
    // 0x447E5A: call Sys_GetGlobal32_4F1210 (supplies argument 2, the event id);
    // 0x447E5F: mov [0x53c7dc] into a register (argument 1, the id RELOADED from the global); 0x447E68: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_53c7dc), static_cast<std::uint32_t>(Sys_GetGlobal32_4F1210()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x4482D0U)));

    // 0x447E70: push 0x4483F0 (handler, deferred argument 3);
    // 0x447E75: call Sys_GetGlobal32_4F1220 (supplies argument 2, the event id);
    // 0x447E7A: mov [0x53c7dc] into a register (argument 1, the id RELOADED from the global); 0x447E83: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_53c7dc), static_cast<std::uint32_t>(Sys_GetGlobal32_4F1220()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x4483F0U)));

    // 0x447E8B: push 0x443C80 (handler, deferred argument 3);
    // 0x447E90: call Sys_GetSettingByte_4167F0 (supplies argument 2, the event id);
    // 0x447E96: mov [0x53c7dc] into a register (argument 1, the id RELOADED from the global); 0x447E9D: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_53c7dc), static_cast<std::uint32_t>(Sys_GetSettingByte_4167F0()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x443C80U)));

    // 0x447EA5: push 0x51BAA0 (handler, deferred argument 3);
    // 0x447EAA: call Sys_GetSettingByte_416810 (supplies argument 2, the event id);
    // 0x447EAF: mov [0x53c7dc] into a register (argument 1, the id RELOADED from the global); 0x447EB8: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_53c7dc), static_cast<std::uint32_t>(Sys_GetSettingByte_416810()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x51BAA0U)));

    // 0x447EC0: push 0x4484F0 (handler, deferred argument 3);
    // 0x447EC5: call Sys_GetSettingByte_416800 (supplies argument 2, the event id);
    // 0x447ECA: mov [0x53c7dc] into a register (argument 1, the id RELOADED from the global); 0x447ED3: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_53c7dc), static_cast<std::uint32_t>(Sys_GetSettingByte_416800()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x4484F0U)));

    // 0x447EDB: push 0x448510 (handler, deferred argument 3);
    // 0x447EE0: call Sys_GetSettingByte_416820 (supplies argument 2, the event id);
    // 0x447EE6: mov [0x53c7dc] into a register (argument 1, the id RELOADED from the global); 0x447EED: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_53c7dc), static_cast<std::uint32_t>(Sys_GetSettingByte_416820()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x448510U)));

    // 0x447EF5: push 0x443DC0 (handler, deferred argument 3);
    // 0x447EFA: call Task_GetCachedEventId_467D90 (supplies argument 2, the event id);
    // 0x447EFF: mov [0x53c7dc] into a register (argument 1, the id RELOADED from the global); 0x447F08: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_53c7dc), static_cast<std::uint32_t>(Task_GetCachedEventId_467D90()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x443DC0U)));

    // THE DEFERRED-PUSH INVERSION, at its cleanest: 0x00447F10 `push $0x5` happens BEFORE `call 0x00481350`, and 0x00481350 takes no argument
    // (`mov eax,[0x5C1184]; ret`), so the 5 survives that call and becomes the SECOND argument of 0x004F1980 while the getter result pushed at
    // 0x00447F17 becomes the FIRST. Retail is Grow(container, 5); reading the pushes in source order would give Grow(5, container). One
    // `add esp,8` at 0x00447F1D consumes both, so the census closes either way - only the callee's own body settles the order.
    ::g_57c1fc = reinterpret_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(
        Script_TaskContainer_Grow(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(Sys_GetGlobal32_481350())), 5))); // 0x00447F10..0x00447F20

    // 0x447F26: push $0x0 (arg6); 0x447F28: push $0x7 (arg5); 0x447F29: call Camera_Get_4F1000 (arg4);
    // 0x447F35: lea 0x244(%esp) -> frame+0x238 (arg3); 0x447F3C: lea 0xc(%esp) -> frame+0x0 (arg2); 0x447F43: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57c1fc)),
                                append_frame, append_frame + 0x238,
                                static_cast<std::int32_t>(Camera_Get_4F1000()), 7, 0);

    // 0x447F4C: push $0x0 (arg6); 0x447F4E: push $0x7 (arg5); 0x447F4F: call Sys_GetGlobal32_4F0FC0 (arg4);
    // 0x447F5B: lea 0x264(%esp) -> frame+0x258 (arg3); 0x447F62: lea 0xc(%esp) -> frame+0x0 (arg2); 0x447F69: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57c1fc)),
                                append_frame, append_frame + 0x258,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FC0()), 7, 0);

    // 0x447F72: push $0x0 (arg6); 0x447F74: push $0x7 (arg5); 0x447F75: call Sys_GetGlobal32_4F0FB0 (arg4);
    // 0x447F81: lea 0x256(%esp) -> frame+0x24A (arg3); 0x447F88: lea 0xc(%esp) -> frame+0x0 (arg2); 0x447F8F: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57c1fc)),
                                append_frame, append_frame + 0x24A,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FB0()), 7, 0);

    // 0x447F98: push $0x0 (arg6); 0x447F9A: push $0x7 (arg5); 0x447F9B: call Sys_GetGlobal32_4F0F50 (arg4);
    // 0x447FA7: lea 0x258(%esp) -> frame+0x24C (arg3); 0x447FAE: lea 0xc(%esp) -> frame+0x0 (arg2); 0x447FB5: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57c1fc)),
                                append_frame, append_frame + 0x24C,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F50()), 7, 0);

    // The FIFTH append is the only one in this body whose argument 6 is NOT zero: 0x00447FBD `push $0x57C1F0` passes the address of the dword that
    // was initialised to 10 at 0x00447DB6, so this append hands the container an out/in pointer. The pushes then follow the usual grid -
    // 0x00447FC2 `push $0x7` (argument 5), 0x00447FC4 `call 0x004F0F00` (argument 4, which takes no argument and ignores the 7), 0x00447FCF
    // `push %eax`, 0x00447FD0 `lea 0x25C(%esp) -> frame+0x250` (argument 3), 0x00447FD7 `lea 0xC(%esp) -> frame+0x00` (argument 2),
    // 0x00447FDD `push %ecx`, 0x00447FDE `call 0x004F1A80` - and this site has no cleanup of its own: its six pushes are folded into the tail.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57c1fc)),
                                append_frame, append_frame + 0x250,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F00()), 7,
                                static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(&::g_57c1f0)));

    // 0x00447FE3: add esp,0x2C0 - the 0x2A8 frame plus this last append's six outstanding pushes; 0x00447FE9: ret.
} // end of Task_DefineGenericTBA

} // extern "C"
