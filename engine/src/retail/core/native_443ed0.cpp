// Native: Task_DefineCabinetAndDrawer (behavior-derived placeholder name from the two task-type name literals this body registers; retail symbol
//   unknown); retail VA 0x00443ED0-0x0044414E.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 639 bytes, 1 block, 175 instructions.
// Tools/evidence: radare2 6.2.0 af/afbj/pdf with bin.relocs.apply over the exact image - 1 block, 175 instructions, 639 bytes, 0 edges (the body is
//   straight-line and has no branch of any kind; established before the body was written). The objdump listing build/retail-dis.txt was read for the
//   same extent. `ps` at 0x0053C6D4, 0x0053C6CC and 0x0053C6BC (byte-identical to "Cabinet", "Drawer" and "TASKTYPE_DRAWER"). `px 4` at all three
//   output globals - 0x0057C1AC, 0x0057C1A4 and 0x0057C1A8 - gives zero, so every initialiser is 0 and every symbol class is `B`. The prologue was
//   read before any `lea` was resolved: `sub esp,0x138` with NO saved-register pushes and no stack probe, so the frame-relative shift is ZERO. A
//   push-by-push simulation closes exactly: 85 pushes (340 bytes) against `add esp,0x38` + `add esp,0xC` x9 + `add esp,0x10` + `add esp,0x18` x4 +
//   `add esp,0x40` = 340 bytes, then the final `add esp,0x138` removes the frame; `esp` is 0 at the ret. All 22 distinct callees were swept against
//   the tree's `// Native:` index and every one resolves to an existing file with the return type used here.
// Behavior/CFG: one straight-line body with a 0x138-byte frame that registers TWO task types and wires each one up before moving to the next.
//   (1) "Cabinet", object size 0x138 (equal to its own frame), parent Sys_GetGlobal16_4E0830 -> id 0x0057C1AC; THIRTEEN handlers (nine 0x00401400
//       and four 0x00401530): 0x00444420/e0, 0x00444590/e1, 0x00444660/e2 through the literal event ids 0, 1 and 2; then 0x004F1A60->0x004446D0,
//       0x004F1A70->0x004446E0, 0x004167F0->0x00444860, 0x00416810->0x00427D70, 0x00416800->0x00444940, 0x00416820->0x004449A0,
//       0x004D3710 (the shared no-op handler)->0x004449C0 with 0x004F1210, 0x004F1220->..., and 0x00467D90->0x00443DC0 (the SAME handler the
//       preceding native_443680.cpp links as its last event); then Script_TaskContainer_Alloc(4) -> 0x0057C1A8 and FOUR appends;
//   (2) "Drawer", object size 0x160 (NOT the frame size), parent Sys_GetGlobal16_4D9FA0 (a DIFFERENT parent from "Cabinet") -> id 0x0057C1A4;
//       THREE handlers (one 0x00401400 and two 0x00401530): 0x00444150/e0, 0x00444260/e1, 0x00426860/e2 through the literal event ids 0, 1, 2;
//       then Script_QHashNodeRegister("TASKTYPE_DRAWER", drawid) - **"Cabinet" is NOT hash-registered and the "Drawer" registration, its three
//       links and the hash registration share a single folded `add esp,0x40`, so this type has NO parameter container at all**.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. Census: 0x00401400 x10, 0x00401530 x6 (16 links), 0x00401900 x2, 0x004167F0 x1, 0x00416800 x1,
//   0x00416810 x1, 0x00416820 x1, 0x00416840 x1, 0x00467D90 x1, 0x004B8810 x1, 0x004D9FA0 x1, 0x004E0830 x1, 0x004F0F20 x1, 0x004F0F50 x1,
//   0x004F0FD0 x1, 0x004F1010 x1, 0x004F1210 x1, 0x004F1220 x1, 0x004F1950 x1, 0x004F1A60 x1, 0x004F1A70 x1, 0x004F1A80 x4 = 40 sites /
//   22 distinct. Link split: 10 + 6 = 16, i.e. 13 for "Cabinet" and 3 for "Drawer". Cleanup reconciliation: 0x00443F29 `add esp,0x38` = the five
//   registration pushes plus links 1-3; 0x00443F44, 0x00443F5E, 0x00443F79, 0x00443F94, 0x00443FAE, 0x00443FC9, 0x00443FE4, 0x00443FFE and
//   0x00444019 `add esp,0xC` NINE times = links 4-12, one each; 0x0044403B `add esp,0x10` = link 13's three pushes plus the alloc's capacity push;
//   0x00444066/0x0044408C/0x004440B2/0x004440D8 `add esp,0x18` = one per append; 0x00444145 `add esp,0x40` = SIXTEEN pushes = the "Drawer"
//   registration's five + links 14-16's nine + the hash registration's two (a folded cleanup covering the whole second section);
//   0x00444148 `add esp,0x138` = the frame.
//   **Three numbers for the one container this body owns: FOUR static append sites, FOUR dynamic records, capacity 4 requested by `push $0x4` at
//   0x00444034.** records == capacity, corroborated because the four sites are distinct static sites rather than a loop body and the capacity is a
//   literal in a different instruction. The "Drawer" type has no container, so for it the check is vacuous and is recorded as such rather than counted
//   as a pass.
//   Cross-references to already-landed files: the last "Cabinet" handler is 0x00443DC0, which native_443680.cpp (the preceding target, "Terminal")
//   links as well - a shared handler, not a duplicate definition. 0x004D3710 is the shared no-op handler (163 link sites image-wide) and 0x00427D70
//   and 0x00426860 also appear as handler immediates elsewhere in the image, but neither is transcribed in the tree yet.
//   Documented shape differences, all value-preserving: (1) the getter callees ignore the arguments retail pushes at their call sites and the tree's
//   transcriptions take no arguments, so those inert pushes are not reproduced; (2) the `add esp` cleanups are reproduced implicitly by the tree's
//   calling convention rather than by explicit arithmetic, with the retail sizes recorded above; (3) byte-valued getters are zero-extended into the
//   32-bit link arguments in the port while retail leaves the upper bits of eax undefined - no effect on the links, both of which mask the event word.
//   Unestablished: the meaning of the four recorded deltas (retail's own frame-relative values, transcribed and not decoded); whether the two types'
//   handlers share state through the container; the handler immediates 0x00444420, 0x00444590, 0x00444660, 0x004446D0, 0x004446E0, 0x00444860,
//   0x00427D70, 0x00444940, 0x004449A0, 0x004D3710, 0x004449C0, 0x00444A70, 0x00443DC0, 0x00444150, 0x00444260 and 0x00426860 are raw code
//   immediates with no transcription in the tree yet, and this file deliberately does not reference them as symbols.

#include "igi/retail/core/task_types.hpp"

#include <cstdint>

// Callees, each with the return type and linkage of its own transcribed file. 0x00401400, 0x00401530 and 0x00401900 come from
// igi/retail/core/task_types.hpp (core/task_types.cpp) and are not redeclared here.
extern "C" std::uint16_t Sys_GetGlobal16_4E0830(void);       // 0x004E0830: parent type getter of "Cabinet" (sys/native_4e0830.cpp)
extern "C" std::uint16_t Sys_GetGlobal16_4D9FA0(void);       // 0x004D9FA0: parent type getter of "Drawer" (sys/native_4d9fa0.cpp)
extern "C" std::uint32_t Sys_GetGlobal32_4F1010(void);       // 0x004F1010 (sys/native_4f1010.cpp)
extern "C" std::uint32_t Sys_GetGlobal32_4F0F20(void);       // 0x004F0F20 (sys/native_4f0f20.cpp)
extern "C" std::uint32_t Sys_GetGlobal32_4F0F50(void);       // 0x004F0F50 (sys/native_4f0f50.cpp)
extern "C" std::uint32_t Core_GetRenderWordB_4F0FD0(void);   // 0x004F0FD0 (core/native_4f0fd0.cpp)
extern "C" std::uint32_t Sys_GetGlobal32_4F1210(void);       // 0x004F1210 (sys/native_4f1210.cpp)
extern "C" std::uint32_t Sys_GetGlobal32_4F1220(void);       // 0x004F1220 (sys/native_4f1220.cpp)
extern "C" std::uint8_t Script_PhaseIdGetE4_4F1A60(void);    // 0x004F1A60 (script/native_4f1a60.cpp)
extern "C" std::uint8_t Script_PhaseIdGetE5_4F1A70(void);    // 0x004F1A70 (script/native_4f1a70.cpp)
extern "C" std::uint8_t Sys_GetSettingByte_4167F0(void);     // 0x004167F0 (config/native_4167f0.cpp)
extern "C" std::uint8_t Sys_GetSettingByte_416800(void);     // 0x00416800 (config/native_416800.cpp)
extern "C" std::uint8_t Sys_GetSettingByte_416810(void);     // 0x00416810 (config/native_416810.cpp)
extern "C" std::uint8_t Sys_GetSettingByte_416820(void);     // 0x00416820 (config/native_416820.cpp)
extern "C" std::uint8_t Sys_GetSettingByte_416840(void);     // 0x00416840 (config/native_416840.cpp)
extern "C" std::uint8_t Task_GetCachedEventId_467D90(void);  // 0x00467D90 (core/native_00467d90.cpp)
extern "C" void* Script_TaskContainer_Alloc(std::int32_t const count); // 0x004F1950 (script/native_4f1950.cpp)
// 0x004F1A80 (script/native_4f1a80.cpp): arg5 is read as a byte (record +0x0C) and arg6 as a dword (record +0x08).
extern "C" void* Script_TaskContainer_Append(std::int32_t* const container, std::int32_t const arg2, std::int32_t const arg3,
                                             std::int32_t const arg4, std::int32_t const arg5, std::int32_t const arg6);
extern "C" void Script_QHashNodeRegister(void* const arg1, void* const value); // 0x004B8810 (script/native_4b8810.cpp)

extern "C" {


// 0x0057C1AC: the "Cabinet" task type id (word). Image value 0 -> initialiser 0, symbol class `B`, writable `.data`: `.rdata` ends at 0x005358F6 and
// `.data` starts at 0x00536000, and this image has no `.bss`, so a zero-image global is `.data` with a zero initialiser and `B`/`D` is the symbol
// classification rather than the section. No other file in the tree owns it.
std::uint16_t g_57c1ac = 0U;

// 0x0057C1A8: the "Cabinet" parameter container, four records (capacity 4, exactly the number of appends in this body). Image value 0 -> `B`.
std::uint32_t g_57c1a8 = 0U;

// 0x0057C1A4: the "Drawer" task type id (word). Image value 0 -> `B`. It is the only one of the two ids that is hash-registered.
std::uint16_t g_57c1a4 = 0U;

void Task_DefineCabinetAndDrawer()
{
    // 0x00443ED0: sub esp,0x138 - a plain frame: no saved-register pushes and no stack probe, so no frame-relative shift applies. The frame is
    // annotated with the "Cabinet" object size (0x138) and holds the four append templates.
    std::int32_t append_frame_base = 0; // retail frame slot at +0x00 - argument 2 of every append
    const std::int32_t append_frame = reinterpret_cast<std::int32_t>(&append_frame_base);

    // 0x00443ED6: push 0; 0x00443ED8: push 0; 0x00443EDA: call 0x4E0830; 0x00443EDF: push eax; 0x00443EE0: push 0x138;
    // 0x00443EE5: push 0x0053C6D4; 0x00443EEA: call 0x401900.
    const std::uint16_t cabinet_id = TaskType_Register("Cabinet",                 // 0x00443EE5: push 0x0053C6D4
                                                       0x138U,                    // 0x00443EE0: push 0x138 (object size, equal to the frame)
                                                       Sys_GetGlobal16_4E0830(),  // 0x00443EDA: call 0x4E0830 (parent type)
                                                       0U);                       // 0x00443ED8: push 0
    ::g_57c1ac = cabinet_id; // 0x00443EF7: mov word [0x57C1AC],ax - the only store to this slot, made between the pushes of the first link

    // 0x443EEF: push 0x444420 (handler, deferred argument 3);
    // 0x443EF4: push $0x0 (argument 2, literal event id);
    // 0x443EF6: push %eax (argument 1, still the id TaskType_Register returned); 0x443EFD: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(cabinet_id), 0x0U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x444420U)));

    // 0x443F08: push 0x444590 (handler, deferred argument 3);
    // 0x443F0D: push $0x1 (argument 2, literal event id);
    // 0x443F02: mov [0x57c1ac] into a register (argument 1, the id RELOADED from the global); 0x443F10: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_57c1ac), 0x1U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x444590U)));

    // 0x443F1C: push 0x444660 (handler, deferred argument 3);
    // 0x443F21: push $0x2 (argument 2, literal event id);
    // 0x443F15: mov [0x57c1ac] into a register (argument 1, the id RELOADED from the global); 0x443F24: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_57c1ac), 0x2U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x444660U)));

    // 0x443F2C: push 0x4446D0 (handler, deferred argument 3);
    // 0x443F31: call Script_PhaseIdGetE4_4F1A60 (supplies argument 2, the event id);
    // 0x443F36: mov [0x57c1ac] into a register (argument 1, the id RELOADED from the global); 0x443F3F: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57c1ac), static_cast<std::uint32_t>(Script_PhaseIdGetE4_4F1A60()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x4446D0U)));

    // 0x443F47: push 0x4446E0 (handler, deferred argument 3);
    // 0x443F4C: call Script_PhaseIdGetE5_4F1A70 (supplies argument 2, the event id);
    // 0x443F52: mov [0x57c1ac] into a register (argument 1, the id RELOADED from the global); 0x443F59: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57c1ac), static_cast<std::uint32_t>(Script_PhaseIdGetE5_4F1A70()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x4446E0U)));

    // 0x443F61: push 0x444860 (handler, deferred argument 3);
    // 0x443F66: call Sys_GetSettingByte_4167F0 (supplies argument 2, the event id);
    // 0x443F6B: mov [0x57c1ac] into a register (argument 1, the id RELOADED from the global); 0x443F74: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57c1ac), static_cast<std::uint32_t>(Sys_GetSettingByte_4167F0()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x444860U)));

    // 0x443F7C: push 0x427D70 (handler, deferred argument 3);
    // 0x443F81: call Sys_GetSettingByte_416810 (supplies argument 2, the event id);
    // 0x443F86: mov [0x57c1ac] into a register (argument 1, the id RELOADED from the global); 0x443F8F: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57c1ac), static_cast<std::uint32_t>(Sys_GetSettingByte_416810()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x427D70U)));

    // 0x443F97: push 0x444940 (handler, deferred argument 3);
    // 0x443F9C: call Sys_GetSettingByte_416800 (supplies argument 2, the event id);
    // 0x443FA2: mov [0x57c1ac] into a register (argument 1, the id RELOADED from the global); 0x443FA9: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57c1ac), static_cast<std::uint32_t>(Sys_GetSettingByte_416800()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x444940U)));

    // 0x443FB1: push 0x4449A0 (handler, deferred argument 3);
    // 0x443FB6: call Sys_GetSettingByte_416820 (supplies argument 2, the event id);
    // 0x443FBB: mov [0x57c1ac] into a register (argument 1, the id RELOADED from the global); 0x443FC4: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57c1ac), static_cast<std::uint32_t>(Sys_GetSettingByte_416820()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x4449A0U)));

    // 0x443FCC: push 0x4D3710 (handler, deferred argument 3);
    // 0x443FD1: call Sys_GetSettingByte_416840 (supplies argument 2, the event id);
    // 0x443FD6: mov [0x57c1ac] into a register (argument 1, the id RELOADED from the global); 0x443FDF: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57c1ac), static_cast<std::uint32_t>(Sys_GetSettingByte_416840()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x4D3710U)));

    // 0x443FE7: push 0x4449C0 (handler, deferred argument 3);
    // 0x443FEC: call Sys_GetGlobal32_4F1210 (supplies argument 2, the event id);
    // 0x443FF2: mov [0x57c1ac] into a register (argument 1, the id RELOADED from the global); 0x443FF9: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_57c1ac), static_cast<std::uint32_t>(Sys_GetGlobal32_4F1210()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x4449C0U)));

    // 0x444001: push 0x444A70 (handler, deferred argument 3);
    // 0x444006: call Sys_GetGlobal32_4F1220 (supplies argument 2, the event id);
    // 0x44400B: mov [0x57c1ac] into a register (argument 1, the id RELOADED from the global); 0x444014: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_57c1ac), static_cast<std::uint32_t>(Sys_GetGlobal32_4F1220()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x444A70U)));

    // 0x44401C: push 0x443DC0 (handler, deferred argument 3);
    // 0x444021: call Task_GetCachedEventId_467D90 (supplies argument 2, the event id);
    // 0x444026: mov [0x57c1ac] into a register (argument 1, the id RELOADED from the global); 0x44402F: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57c1ac), static_cast<std::uint32_t>(Task_GetCachedEventId_467D90()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x443DC0U)));

    // 0x00444034: push $0x4 (container CAPACITY, four records); 0x00444036: call 0x4F1950; 0x0044403B: add esp,0x10 (this push plus the
    // thirteenth link's three); 0x0044403E: mov [0x57C1A8],eax.
    ::g_57c1a8 = reinterpret_cast<std::uint32_t>(Script_TaskContainer_Alloc(4)); // 0x00444034: push 0x4

    // 0x444044: push $0x0 (arg6); 0x444046: push $0x7 (arg5); 0x444047: call Sys_GetGlobal32_4F1010 (arg4);
    // 0x444053: lea 0xfc(%esp) -> frame+0xF0 (arg3); 0x44405A: lea 0xc(%esp) -> frame+0x0 (arg2); 0x444061: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57c1a8)),
                                append_frame, append_frame + 0xF0,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F1010()), 7, 0);

    // 0x44406A: push $0x0 (arg6); 0x44406C: push $0x7 (arg5); 0x44406D: call Sys_GetGlobal32_4F0F20 (arg4);
    // 0x444079: lea 0x114(%esp) -> frame+0x108 (arg3); 0x444080: lea 0xc(%esp) -> frame+0x0 (arg2); 0x444087: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57c1a8)),
                                append_frame, append_frame + 0x108,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F20()), 7, 0);

    // 0x444090: push $0x0 (arg6); 0x444092: push $0x7 (arg5); 0x444093: call Core_GetRenderWordB_4F0FD0 (arg4);
    // 0x44409F: lea 0x118(%esp) -> frame+0x10C (arg3); 0x4440A6: lea 0xc(%esp) -> frame+0x0 (arg2); 0x4440AD: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57c1a8)),
                                append_frame, append_frame + 0x10C,
                                static_cast<std::int32_t>(Core_GetRenderWordB_4F0FD0()), 7, 0);

    // 0x4440B6: push $0x0 (arg6); 0x4440B8: push $0x7 (arg5); 0x4440B9: call Sys_GetGlobal32_4F0F50 (arg4);
    // 0x4440C5: lea 0x138(%esp) -> frame+0x12C (arg3); 0x4440CC: lea 0xc(%esp) -> frame+0x0 (arg2); 0x4440D3: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57c1a8)),
                                append_frame, append_frame + 0x12C,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F50()), 7, 0);

    // 0x004440DB: push 0; 0x004440DD: push 0; 0x004440DF: call 0x4D9FA0 (a DIFFERENT parent type from the "Cabinet" registration);
    // 0x004440E4: push eax; 0x004440E5: push 0x160; 0x004440EA: push 0x0053C6CC; 0x004440EF: call 0x401900. No `add esp` follows this
    // registration: its five pushes stay outstanding and are removed together with the three links and the hash registration below.
    const std::uint16_t drawer_id = TaskType_Register("Drawer",                   // 0x004440EA: push 0x0053C6CC
                                                      0x160U,                     // 0x004440E5: push 0x160 (NOT the frame size)
                                                      Sys_GetGlobal16_4D9FA0(),   // 0x004440DF: call 0x4D9FA0 (parent type)
                                                      0U);                        // 0x004440DD: push 0
    ::g_57c1a4 = drawer_id; // 0x004440FC: mov word [0x57C1A4],ax

    // 0x4440F4: push 0x444150 (handler, deferred argument 3);
    // 0x4440F9: push $0x0 (argument 2, literal event id);
    // 0x4440FB: push %eax (argument 1, still the id TaskType_Register returned); 0x444102: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(drawer_id), 0x0U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x444150U)));

    // 0x44410D: push 0x444260 (handler, deferred argument 3);
    // 0x444112: push $0x1 (argument 2, literal event id);
    // 0x444107: mov [0x57c1a4] into a register (argument 1, the id RELOADED from the global); 0x444115: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_57c1a4), 0x1U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x444260U)));

    // 0x444121: push 0x426860 (handler, deferred argument 3);
    // 0x444126: push $0x2 (argument 2, literal event id);
    // 0x44411A: mov [0x57c1a4] into a register (argument 1, the id RELOADED from the global); 0x444129: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_57c1a4), 0x2U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x426860U)));

    // 0x0044412E: mov edx,[0x57C1A4] (the drawer id RELOADED from the global); 0x00444134: and $0xffff,%edx; 0x0044413A: push edx (argument 2);
    // 0x0044413B: push 0x0053C6BC ("TASKTYPE_DRAWER", argument 1); 0x00444140: call 0x4B8810.
    // **Only the SECOND type is hash-registered**: "Cabinet" is not.
    const std::uint32_t registered_id = static_cast<std::uint32_t>(::g_57c1a4) & 0xFFFFU; // 0x00444134: and $0xffff,%edx
    Script_QHashNodeRegister(const_cast<char*>("TASKTYPE_DRAWER"),                        // 0x0044413B: push 0x0053C6BC
                             reinterpret_cast<void*>(static_cast<std::uintptr_t>(registered_id))); // 0x0044413A: push edx

    // 0x00444145: add esp,0x40 - sixteen pushes: the "Drawer" registration's five, the three links above and the hash registration's two;
    // 0x00444148: add esp,0x138 - the frame itself; 0x0044414E: ret.
} // end of Task_DefineCabinetAndDrawer

} // extern "C"
