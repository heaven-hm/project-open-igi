// Native: Task_DefineSCameraTypes (behavior-derived placeholder name from the three task-type name literals this body registers; retail symbol
//   unknown); retail VA 0x00440FD0-0x004415BA.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 1515 bytes, 1 block, 437 instructions.
// Tools/evidence: radare2 6.2.0 af/afbj/pdf with bin.relocs.apply over the exact image - 1 block, 437 instructions, 1515 bytes, 0 edges, i.e. the
//   body is straight-line and contains NO branch of any kind (this was established before the body was written, and it is why the transcription is a
//   single basic block). The objdump listing build/retail-dis.txt was read for the same extent. `ps` at 0x0053C5E0, 0x0053C5D4, 0x0053C5C0,
//   0x0053C5A0 and 0x0053C25C (byte-identical to "SCamera", "SCameraLens", "SCameraLensViewCone", "TASKTYPE_SCAMERALENSVIEWCONE" and
//   "alarmlight.spr"). `px 4` at all seven output globals - every image value is ZERO, so every initialiser is 0 and every symbol class is `B`.
//   The prologue was read before any `lea` was resolved: `sub esp,0x1C8` with NO saved-register pushes and no stack probe, so the frame-relative
//   shift for this body is ZERO. A push-by-push simulation over all 437 instructions closes EXACTLY: `esp` is 0 at the ret, and the per-section
//   cleanups reconcile against the pushes they remove (see Status/limits). Every callee was swept against the tree's `// Native:` provenance index:
//   all 30 distinct callees resolve EXCEPT 0x00489640, which had no implementation anywhere in the tree and is transcribed separately as
//   core/native_489640.cpp for exactly that reason; 0x00401400/0x00401530/0x00401900 are declared by task_types.hpp.
// Behavior/CFG: one straight-line block, 0x1C8-byte frame, 96 call sites over 30 distinct callees. The body registers THREE task types in sequence
//   and wires each one up before moving to the next:
//     (1) "SCamera", object size 0x1C8, parent Sys_GetGlobal16_4E0830 -> id 0x0057C180; ELEVEN handlers (seven 0x00401400 and four 0x00401530):
//         0x004423F0/e0, 0x00442610/e1, 0x004426D0/e2, then 0x004F1A60->0x00442730, 0x004F1A70->0x00442740, 0x004F1210->0x00442890,
//         0x004F1220->0x00442970, 0x00489620->0x00442A30, 0x00489610->0x0042FC50, 0x004CEA20->0x00443450, 0x004CEA10->0x004434F0; then
//         Script_TaskContainer_Alloc(0xF) -> 0x0057C184 and FIFTEEN appends whose recorded frame deltas are 0xF0, 0x108, 0x10C, 0x120, 0x124, 0x128,
//         0x138, 0x148, 0x14A, 0x14C, 0x150, 0x154, 0x156, 0x158, 0x174, with argument 4 taken from 0x004F1010, 0x004F0F20 (x3), 0x004F0FD0 (x3),
//         0x004F0FB0 (x5), 0x004F0F50 (x2) and 0x004F0FC0 in that order;
//     (2) "SCameraLens", object size 0x25E8, parent Sys_GetGlobal16_4E0830 -> id 0x0057C190; TWELVE handlers (ten 0x00401400 and two 0x00401530):
//         0x004415C0/e0, 0x00441770/e1, 0x00442070/e2, then 0x004F1A70->0x004420D0, 0x00416840->0x004422B0, 0x00416860->0x004422F0,
//         0x00489610->0x004423C0, literal event 4 ->0x00442A70, 0x00467D90->0x00442C60, 0x00467DB0->0x00442DD0, 0x004CEA20->0x00443450,
//         0x004CEA10->0x004432F0; then 0x0057C17C = Sys_LookupOrAppendRecord_4169D0("alarmlight.spr") and 0x0057C18C = QTaskList_New(0x40) - neither
//         result is passed to the other call, and this type gets NO parameter container;
//     (3) "SCameraLensViewCone", object size 0x138, parent Sys_GetGlobal16_4D9FA0 (a DIFFERENT parent from (1) and (2)) -> id 0x0057C188;
//         ELEVEN handlers, **ALL of them NULL HANDLERS pushed as the immediate $0x0** (every one is a 0x00401400 link, none is 0x00401530):
//         events 0x00416840, 0x00416860, 0x00489610, literal 0xC, literal 0xA, literal 0xB, 0x004D14D0, literal 0x9, literal 0x8, literal 0xC,
//         0x004D14D0; then Script_QHashNodeRegister("TASKTYPE_SCAMERALENSVIEWCONE", id) and finally
//         0x0057C198 = AiDetection_RegisterTaskMask_489640(0x0057C180), i.e. the mask bit claimed for the FIRST type registered, not the third.
//   The three literal event ids of the first registration are 0, 1 and 2; the fourth registration-order literal is 4 (event id 3 is NOT linked by
//   this body); the third type's literal event ids are 0xC, 0xA, 0xB, 0x9, 0x8 and 0xC again, so event id 0xC is linked TWICE for that type.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. **The pre-write census and the cleanup reconciliation both close.** Census (96 sites / 30 distinct):
//   0x00401400 x28, 0x00401530 x6, 0x00401900 x3, 0x00416840 x2, 0x00416860 x2, 0x004169D0 x1, 0x00467D90 x1, 0x00467DB0 x1, 0x00489610 x3,
//   0x00489620 x1, 0x00489640 x1, 0x004B8810 x1, 0x004C1800 x1, 0x004CEA10 x2, 0x004CEA20 x2, 0x004D14D0 x2, 0x004D9FA0 x1, 0x004E0830 x2,
//   0x004F0F20 x3, 0x004F0F50 x2, 0x004F0FB0 x5, 0x004F0FC0 x1, 0x004F0FD0 x3, 0x004F1010 x1, 0x004F1210 x1, 0x004F1220 x1, 0x004F1950 x1,
//   0x004F1A60 x1, 0x004F1A70 x2, 0x004F1A80 x15 = 96. Link split: 28 + 6 = 34 links, i.e. 11 + 12 + 11 across the three registrations.
//   Cleanup reconciliation (each `add esp` matched against the pushes it removes, section by section): 0x00441029 `add esp,0x38` = 14 pushes = the
//   five registration pushes plus links 1-3; 0x00441044..0x004410E4 `add esp,0xC` SEVEN times = links 4-10; 0x00441105 `add esp,0x10` = link 11's
//   three pushes plus the alloc's capacity push; 0x0044112F..0x00441335 `add esp,0x18` FIFTEEN times = one per append; 0x0044138C `add esp,0x38` =
//   the second registration's five plus links 12-14; 0x004413A6/0x004413C1/0x004413DC `add esp,0xC` = links 15-17; 0x0044140A `add esp,0x18` =
//   links 18 AND 19 together (link 18 has NO cleanup of its own - a folded cleanup); 0x00441425/0x0044143F/0x0044145A `add esp,0xC` = links 20-22;
//   0x0044148B `add esp,0x14` = link 23's three pushes plus the lookup's name push and the list-capacity push; 0x004414AC `add esp,0x14` = the third
//   registration's five; 0x004414C9/0x004414E1 `add esp,0xC` = links 24-25; 0x0044152B `add esp,0x30` = links 26-29; 0x00441574 `add esp,0x30` =
//   links 30-33; 0x004415B4 `add esp,0x1E0` = 0x1C8 frame + link 34's three + the hash registration's two + the mask registration's one. Every push
//   is accounted for exactly once and the simulated `esp` is 0 at 0x004415BA.
//   **0x0057C184 is the one container this body owns and its three numbers are: fifteen static append sites, fifteen dynamic records, capacity 0xF
//   requested at the single alloc site.** records == capacity here, and unlike earlier targets this equality is corroborated independently - the
//   fifteen append sites are distinct static sites, not a loop body, and the capacity is a literal in a different instruction. The other two types
//   register NO container at all, so for them the three-number check is vacuous rather than passing.
//   **0x00489640 is a newly discovered dependency with no tree implementation.** It is a 62-byte, FIVE-BLOCK routine that scans the 32-word table
//   at 0x005C8AC4 for the unregistered sentinel 0x0181 and writes the id it is given into the first free slot, returning `1 << slot` (or 0 after
//   Script_WarningLog("AIDetection - QTask mask is full")). It is transcribed as core/native_489640.cpp in this same change, because a file that
//   declared it without defining it would fail at link time - the same defect class the Lead has found four times. **This body is the second,
//   independent confirmation of the 0x0181 sentinel**: 0x004895A6-0x004895B4 (native_489560.cpp) fills that table with `rep stosd` of 0x01810181 and
//   this body's callee searches for exactly that value.
//   Documented shape differences, all value-preserving: (1) every getter callee is an argument-less load in retail that ignores whatever the caller
//   pushes, and the tree's transcriptions take no arguments, so those inert pushes are not reproduced; (2) the `add esp` cleanups are reproduced
//   implicitly by the tree's calling convention rather than by explicit arithmetic - the retail sizes are recorded above so a reviewer can check the
//   arithmetic; (3) byte-valued getters (0x00416840/0x00416860/0x00489610/0x00489620/0x00467D90/0x00467DB0/0x004CEA10/0x004CEA20/0x004D14D0/
//   0x004F1A60/0x004F1A70) are zero-extended into the 32-bit append and link arguments in the port, whereas retail leaves the upper bits of eax
//   undefined; this cannot change behaviour in the links (both link callees mask the event word) and is recorded as an unestablished item for the
//   append records, where the port's zero extension is a choice and not an observation; (4) the first link of each registration passes the id
//   straight from TaskType_Register's return value (retail pushes the live eax), while later links reload it from the global - the port reproduces
//   that difference with a local for the first link and an explicit global read for the rest.
//   Unestablished: the meaning of every recorded frame delta (they are transcribed as retail's own frame-relative values, not decoded as field
//   offsets); the meaning of the mask bit at 0x0057C198 and whether anything reads 0x0057C17C/0x0057C18C; the handler immediates 0x004423F0,
//   0x00442610, 0x004426D0, 0x00442730, 0x00442740, 0x00442890, 0x00442970, 0x00442A30, 0x0042FC50, 0x00443450, 0x004434F0, 0x004415C0, 0x00441770,
//   0x00442070, 0x004420D0, 0x004422B0, 0x004422F0, 0x004423C0, 0x00442A70, 0x00442C60, 0x00442DD0 and 0x004432F0 are raw code immediates, none of
//   which is transcribed in the tree yet, and this file deliberately does not reference them as symbols; the 0x25E8 object size is larger than any
//   frame in this body and is used only as an argument.

#include "igi/retail/core/task_types.hpp"

#include <cstdint>

extern "C" {

// Callees, each with the return type and linkage of its own transcribed file. 0x00401400, 0x00401530 and 0x00401900 are declared by
// igi/retail/core/task_types.hpp (core/task_types.cpp), so they are not redeclared here.
extern "C" std::uint16_t Sys_GetGlobal16_4E0830(void);        // 0x004E0830: parent type getter of the two SCamera* types (sys/native_4e0830.cpp)
extern "C" std::uint16_t Sys_GetGlobal16_4D9FA0(void);        // 0x004D9FA0: parent type getter of SCameraLensViewCone (sys/native_4d9fa0.cpp)
extern "C" std::uint32_t Sys_GetGlobal32_4F1010(void);        // 0x004F1010 (sys/native_4f1010.cpp)
extern "C" std::uint32_t Sys_GetGlobal32_4F0F20(void);        // 0x004F0F20 (sys/native_4f0f20.cpp)
extern "C" std::uint32_t Sys_GetGlobal32_4F0F50(void);        // 0x004F0F50 (sys/native_4f0f50.cpp)
extern "C" std::uint32_t Sys_GetGlobal32_4F0FB0(void);        // 0x004F0FB0 (sys/native_4f0fb0.cpp)
extern "C" std::uint32_t Sys_GetGlobal32_4F0FC0(void);        // 0x004F0FC0 (sys/native_4f0fc0.cpp)
extern "C" std::uint32_t Core_GetRenderWordB_4F0FD0(void);    // 0x004F0FD0 (core/native_4f0fd0.cpp)
extern "C" std::uint32_t Sys_GetGlobal32_4F1210(void);        // 0x004F1210 (sys/native_4f1210.cpp)
extern "C" std::uint32_t Sys_GetGlobal32_4F1220(void);        // 0x004F1220 (sys/native_4f1220.cpp)
extern "C" std::uint8_t Script_PhaseIdGetE4_4F1A60(void);     // 0x004F1A60 (script/native_4f1a60.cpp)
extern "C" std::uint8_t Script_PhaseIdGetE5_4F1A70(void);     // 0x004F1A70 (script/native_4f1a70.cpp)
extern "C" std::uint8_t Sys_GetGlobal8_489610(void);          // 0x00489610 (sys/native_489610.cpp)
extern "C" std::uint8_t Sys_GetGlobal8_489620(void);          // 0x00489620 (sys/native_489620.cpp)
extern "C" std::uint8_t Sys_GetSettingByte_416840(void);      // 0x00416840 (config/native_416840.cpp)
extern "C" std::uint8_t Sys_GetSettingByte_416860(void);      // 0x00416860 (config/native_416860.cpp)
extern "C" std::uint8_t Task_GetCachedEventId_467D90(void);   // 0x00467D90 (core/native_00467d90.cpp)
extern "C" std::uint8_t Task_GetCachedEventId_467DB0(void);   // 0x00467DB0 (core/native_467db0.cpp)
extern "C" std::uint8_t QTask_PhaseIdGet548177_4CEA20(void);  // 0x004CEA20 (script/native_4cea20.cpp)
extern "C" std::uint8_t QTask_PhaseIdGet548176_4CEA10(void);  // 0x004CEA10 (script/native_4cea10.cpp)
extern "C" std::uint8_t Render_GetByte_4D14D0(void);          // 0x004D14D0 (render/native_4d14d0.cpp)
extern "C" void* Sys_LookupOrAppendRecord_4169D0(char const* name); // 0x004169D0 (config/native_4169d0.cpp)
extern "C" void* QTaskList_New(std::int32_t const capacity);  // 0x004C1800 (core/qtask_list.cpp)
extern "C" std::uint32_t AiDetection_RegisterTaskMask_489640(std::uint16_t const arg1); // 0x00489640 (core/native_489640.cpp)
extern "C" void* Script_TaskContainer_Alloc(std::int32_t const count); // 0x004F1950 (script/native_4f1950.cpp)
// 0x004F1A80 (script/native_4f1a80.cpp): arg5 is read as a byte (record +0x0C) and arg6 as a dword (record +0x08).
extern "C" void* Script_TaskContainer_Append(std::int32_t* const container, std::int32_t const arg2, std::int32_t const arg3,
                                             std::int32_t const arg4, std::int32_t const arg5, std::int32_t const arg6);
extern "C" void Script_QHashNodeRegister(void* const arg1, void* const value); // 0x004B8810 (script/native_4b8810.cpp)


// 0x0057C180: the "SCamera" task type id (word, stored with `mov word [0x57C180],ax`). Image value 0 -> initialiser 0, symbol class `B`, writable
// `.data`: the section table puts `.rdata` at VA 0x00533000 with virtual size 0x28F6 (ending 0x005358F6) and `.data` at VA 0x00536000, and this image
// has no `.bss`, so a zero-image global is `.data` with a zero initialiser and `B`/`D` is the symbol classification rather than the section.
std::uint16_t g_57c180 = 0U;

// 0x0057C184: the "SCamera" parameter container, fifteen records (capacity 0xF, exactly the number of appends below). Image value 0 -> `B`.
std::uint32_t g_57c184 = 0U;

// 0x0057C190: the "SCameraLens" task type id (word). Image value 0 -> `B`.
std::uint16_t g_57c190 = 0U;

// 0x0057C17C: the Sys_LookupOrAppendRecord_4169D0("alarmlight.spr") result, stored as a dword pointer. Image value 0 -> `B`. No reader of this
// global is established by this body.
std::uint32_t g_57c17c = 0U;

// 0x0057C18C: the QTaskList_New(0x40) result, stored as a dword pointer. Image value 0 -> `B`. No reader is established by this body.
std::uint32_t g_57c18c = 0U;

// 0x0057C188: the "SCameraLensViewCone" task type id (word). Image value 0 -> `B`.
std::uint16_t g_57c188 = 0U;

// 0x0057C198: the AiDetection_RegisterTaskMask_489640 mask bit for the "SCamera" type. Image value 0 -> `B`. No reader is established by this body.
std::uint32_t g_57c198 = 0U;

void Task_DefineSCameraTypes()
{
    // 0x00440FD0: sub esp,0x1C8 - a plain frame: no saved-register pushes and no stack probe, so no frame-relative shift applies. The frame holds the
    // sixteen argument templates this body hands to Script_TaskContainer_Append, hence its size equals the "SCamera" object size.
    std::int32_t append_frame_base = 0; // retail frame slot at +0x00 - argument 2 of every append
    const std::int32_t append_frame = reinterpret_cast<std::int32_t>(&append_frame_base);

    // 0x00440FD6: push 0 (TaskType_Register's fourth argument, pushed first); 0x00440FD8: push 0 (the parent getter's inert argument);
    // 0x00440FDA: call 0x4E0830; 0x00440FDF: push eax; 0x00440FE0: push 0x1C8; 0x00440FE5: push 0x0053C5E0; 0x00440FEA: call 0x401900.
    const std::uint16_t scamera_id = TaskType_Register("SCamera",                 // 0x00440FE5: push 0x0053C5E0
                                                       0x1C8U,                    // 0x00440FE0: push 0x1C8 (object size, equal to the frame)
                                                       Sys_GetGlobal16_4E0830(),  // 0x00440FDA: call 0x4E0830 (parent type)
                                                       0U);                      // 0x00440FD8: push 0
    ::g_57c180 = scamera_id; // 0x00440FF7: mov word [0x57C180],ax - stored between the pushes of the first link, and REUSED as argument 1 below.

    // 0x440FEF: push 0x4423F0 (handler, deferred argument 3);
    // 0x440FF4: push $0x0 (argument 2, literal event id);
    // 0x440FFB: push %eax (argument 1, still the id TaskType_Register returned); 0x440FFD: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(scamera_id), 0x0U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x4423F0U)));

    // 0x441008: push 0x442610 (handler, deferred argument 3);
    // 0x44100D: push $0x1 (argument 2, literal event id);
    // 0x441002: mov [0x57c180] into a register (argument 1, the id RELOADED from the global); 0x441010: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_57c180), 0x1U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x442610U)));

    // 0x44101C: push 0x4426D0 (handler, deferred argument 3);
    // 0x441021: push $0x2 (argument 2, literal event id);
    // 0x441015: mov [0x57c180] into a register (argument 1, the id RELOADED from the global); 0x441024: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_57c180), 0x2U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x4426D0U)));

    // 0x44102C: push 0x442730 (handler, deferred argument 3);
    // 0x441031: call Script_PhaseIdGetE4_4F1A60 (supplies argument 2, the event id);
    // 0x441036: mov [0x57c180] into a register (argument 1, the id RELOADED from the global); 0x44103F: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57c180), static_cast<std::uint32_t>(Script_PhaseIdGetE4_4F1A60()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x442730U)));

    // 0x441047: push 0x442740 (handler, deferred argument 3);
    // 0x44104C: call Script_PhaseIdGetE5_4F1A70 (supplies argument 2, the event id);
    // 0x441052: mov [0x57c180] into a register (argument 1, the id RELOADED from the global); 0x441059: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57c180), static_cast<std::uint32_t>(Script_PhaseIdGetE5_4F1A70()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x442740U)));

    // 0x441061: push 0x442890 (handler, deferred argument 3);
    // 0x441066: call Sys_GetGlobal32_4F1210 (supplies argument 2, the event id);
    // 0x44106B: mov [0x57c180] into a register (argument 1, the id RELOADED from the global); 0x441074: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_57c180), static_cast<std::uint32_t>(Sys_GetGlobal32_4F1210()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x442890U)));

    // 0x44107C: push 0x442970 (handler, deferred argument 3);
    // 0x441081: call Sys_GetGlobal32_4F1220 (supplies argument 2, the event id);
    // 0x441086: mov [0x57c180] into a register (argument 1, the id RELOADED from the global); 0x44108F: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_57c180), static_cast<std::uint32_t>(Sys_GetGlobal32_4F1220()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x442970U)));

    // 0x441097: push 0x442A30 (handler, deferred argument 3);
    // 0x44109C: call Sys_GetGlobal8_489620 (supplies argument 2, the event id);
    // 0x4410A2: mov [0x57c180] into a register (argument 1, the id RELOADED from the global); 0x4410A9: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57c180), static_cast<std::uint32_t>(Sys_GetGlobal8_489620()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x442A30U)));

    // 0x4410B1: push 0x42FC50 (handler, deferred argument 3);
    // 0x4410B6: call Sys_GetGlobal8_489610 (supplies argument 2, the event id);
    // 0x4410BB: mov [0x57c180] into a register (argument 1, the id RELOADED from the global); 0x4410C4: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57c180), static_cast<std::uint32_t>(Sys_GetGlobal8_489610()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x42FC50U)));

    // 0x4410CC: push 0x443450 (handler, deferred argument 3);
    // 0x4410D1: call QTask_PhaseIdGet548177_4CEA20 (supplies argument 2, the event id);
    // 0x4410D6: mov [0x57c180] into a register (argument 1, the id RELOADED from the global); 0x4410DF: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57c180), static_cast<std::uint32_t>(QTask_PhaseIdGet548177_4CEA20()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x443450U)));

    // 0x4410E7: push 0x4434F0 (handler, deferred argument 3);
    // 0x4410EC: call QTask_PhaseIdGet548176_4CEA10 (supplies argument 2, the event id);
    // 0x4410F2: mov [0x57c180] into a register (argument 1, the id RELOADED from the global); 0x4410F9: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57c180), static_cast<std::uint32_t>(QTask_PhaseIdGet548176_4CEA10()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x4434F0U)));

    // 0x004410FB: push 0xF (the container CAPACITY - fifteen slots); 0x00441100: call 0x4F1950; 0x00441105: add esp,0x10 (this push plus the
    // eleventh link's three); 0x00441108: mov [0x57C184],eax.
    ::g_57c184 = reinterpret_cast<std::uint32_t>(Script_TaskContainer_Alloc(0xF)); // 0x004410FB: push 0xF

    // 0x44110D: push $0x0 (arg6); 0x44110F: push $0x7 (arg5); 0x441111: call Sys_GetGlobal32_4F1010 (arg4);
    // 0x441116: lea 0xf8(%esp),%ecx -> frame+0xF0 (arg3); 0x441123: lea 0xc(%esp),%edx -> frame+0x0 (arg2);
    // 0x441127: push %ecx; 0x441128: push %edx; 0x441129: push %eax (arg1, the container); 0x44112A: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57c184)),
                                append_frame, append_frame + 0xF0,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F1010()), 7, 0);

    // 0x441132: push $0x0 (arg6); 0x441134: push $0x7 (arg5); 0x441136: call Sys_GetGlobal32_4F0F20 (arg4);
    // 0x44113B: lea 0x110(%esp),%ecx -> frame+0x108 (arg3); 0x441148: lea 0xc(%esp),%edx -> frame+0x0 (arg2);
    // 0x44114C: push %ecx; 0x44114D: push %edx; 0x44114E: push %eax (arg1, the container); 0x44114F: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57c184)),
                                append_frame, append_frame + 0x108,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F20()), 7, 0);

    // 0x441157: push $0x0 (arg6); 0x441159: push $0x7 (arg5); 0x44115B: call Core_GetRenderWordB_4F0FD0 (arg4);
    // 0x441160: lea 0x114(%esp),%ecx -> frame+0x10C (arg3); 0x44116D: lea 0xc(%esp),%edx -> frame+0x0 (arg2);
    // 0x441171: push %ecx; 0x441172: push %edx; 0x441173: push %eax (arg1, the container); 0x441174: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57c184)),
                                append_frame, append_frame + 0x10C,
                                static_cast<std::int32_t>(Core_GetRenderWordB_4F0FD0()), 7, 0);

    // 0x44117C: push $0x0 (arg6); 0x44117E: push $0x7 (arg5); 0x441180: call Sys_GetGlobal32_4F0F20 (arg4);
    // 0x441185: lea 0x128(%esp),%ecx -> frame+0x120 (arg3); 0x441192: lea 0xc(%esp),%edx -> frame+0x0 (arg2);
    // 0x441196: push %ecx; 0x441197: push %edx; 0x441198: push %eax (arg1, the container); 0x441199: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57c184)),
                                append_frame, append_frame + 0x120,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F20()), 7, 0);

    // 0x4411A1: push $0x0 (arg6); 0x4411A3: push $0x7 (arg5); 0x4411A5: call Sys_GetGlobal32_4F0F20 (arg4);
    // 0x4411AA: lea 0x12c(%esp),%ecx -> frame+0x124 (arg3); 0x4411B7: lea 0xc(%esp),%edx -> frame+0x0 (arg2);
    // 0x4411BB: push %ecx; 0x4411BC: push %edx; 0x4411BD: push %eax (arg1, the container); 0x4411BE: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57c184)),
                                append_frame, append_frame + 0x124,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F20()), 7, 0);

    // 0x4411C6: push $0x0 (arg6); 0x4411C8: push $0x7 (arg5); 0x4411CA: call Core_GetRenderWordB_4F0FD0 (arg4);
    // 0x4411CF: lea 0x130(%esp),%ecx -> frame+0x128 (arg3); 0x4411DC: lea 0xc(%esp),%edx -> frame+0x0 (arg2);
    // 0x4411E0: push %ecx; 0x4411E1: push %edx; 0x4411E2: push %eax (arg1, the container); 0x4411E3: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57c184)),
                                append_frame, append_frame + 0x128,
                                static_cast<std::int32_t>(Core_GetRenderWordB_4F0FD0()), 7, 0);

    // 0x4411EB: push $0x0 (arg6); 0x4411ED: push $0x7 (arg5); 0x4411EF: call Core_GetRenderWordB_4F0FD0 (arg4);
    // 0x4411F4: lea 0x140(%esp),%ecx -> frame+0x138 (arg3); 0x4411FD: lea 0x10(%esp),%edx -> frame+0x0 (arg2);
    // 0x441205: push %ecx; 0x441206: push %edx; 0x441207: push %eax (arg1, the container); 0x441208: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57c184)),
                                append_frame, append_frame + 0x138,
                                static_cast<std::int32_t>(Core_GetRenderWordB_4F0FD0()), 7, 0);

    // 0x441210: push $0x0 (arg6); 0x441212: push $0x7 (arg5); 0x441214: call Sys_GetGlobal32_4F0FB0 (arg4);
    // 0x441219: lea 0x150(%esp),%ecx -> frame+0x148 (arg3); 0x441226: lea 0xc(%esp),%edx -> frame+0x0 (arg2);
    // 0x44122A: push %ecx; 0x44122B: push %edx; 0x44122C: push %eax (arg1, the container); 0x44122D: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57c184)),
                                append_frame, append_frame + 0x148,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FB0()), 7, 0);

    // 0x441235: push $0x0 (arg6); 0x441237: push $0x7 (arg5); 0x441239: call Sys_GetGlobal32_4F0FB0 (arg4);
    // 0x44123E: lea 0x152(%esp),%ecx -> frame+0x14A (arg3); 0x44124B: lea 0xc(%esp),%edx -> frame+0x0 (arg2);
    // 0x44124F: push %ecx; 0x441250: push %edx; 0x441251: push %eax (arg1, the container); 0x441252: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57c184)),
                                append_frame, append_frame + 0x14A,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FB0()), 7, 0);

    // 0x44125A: push $0x0 (arg6); 0x44125C: push $0x7 (arg5); 0x44125E: call Sys_GetGlobal32_4F0FB0 (arg4);
    // 0x441263: lea 0x154(%esp),%ecx -> frame+0x14C (arg3); 0x441270: lea 0xc(%esp),%edx -> frame+0x0 (arg2);
    // 0x441274: push %ecx; 0x441275: push %edx; 0x441276: push %eax (arg1, the container); 0x441277: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57c184)),
                                append_frame, append_frame + 0x14C,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FB0()), 7, 0);

    // 0x44127F: push $0x0 (arg6); 0x441281: push $0x7 (arg5); 0x441283: call Sys_GetGlobal32_4F0F50 (arg4);
    // 0x441288: lea 0x158(%esp),%ecx -> frame+0x150 (arg3); 0x441295: lea 0xc(%esp),%edx -> frame+0x0 (arg2);
    // 0x441299: push %ecx; 0x44129A: push %edx; 0x44129B: push %eax (arg1, the container); 0x44129C: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57c184)),
                                append_frame, append_frame + 0x150,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F50()), 7, 0);

    // 0x4412A4: push $0x0 (arg6); 0x4412A6: push $0x7 (arg5); 0x4412A8: call Sys_GetGlobal32_4F0FB0 (arg4);
    // 0x4412AD: lea 0x15c(%esp),%ecx -> frame+0x154 (arg3); 0x4412BA: lea 0xc(%esp),%edx -> frame+0x0 (arg2);
    // 0x4412BE: push %ecx; 0x4412BF: push %edx; 0x4412C0: push %eax (arg1, the container); 0x4412C1: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57c184)),
                                append_frame, append_frame + 0x154,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FB0()), 7, 0);

    // 0x4412C9: push $0x0 (arg6); 0x4412CB: push $0x7 (arg5); 0x4412CD: call Sys_GetGlobal32_4F0FB0 (arg4);
    // 0x4412D2: lea 0x15e(%esp),%ecx -> frame+0x156 (arg3); 0x4412DF: lea 0xc(%esp),%edx -> frame+0x0 (arg2);
    // 0x4412E3: push %ecx; 0x4412E4: push %edx; 0x4412E5: push %eax (arg1, the container); 0x4412E6: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57c184)),
                                append_frame, append_frame + 0x156,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FB0()), 7, 0);

    // 0x4412EE: push $0x0 (arg6); 0x4412F0: push $0x7 (arg5); 0x4412F2: call Sys_GetGlobal32_4F0F50 (arg4);
    // 0x4412FD: lea 0x164(%esp),%ecx -> frame+0x158 (arg3); 0x441304: lea 0xc(%esp),%edx -> frame+0x0 (arg2);
    // 0x441308: push %ecx; 0x441309: push %edx; 0x44130A: push %eax (arg1, the container); 0x44130B: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57c184)),
                                append_frame, append_frame + 0x158,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F50()), 7, 0);

    // 0x441313: push $0x0 (arg6); 0x441315: push $0x7 (arg5); 0x441317: call Sys_GetGlobal32_4F0FC0 (arg4);
    // 0x44131C: lea 0x17c(%esp),%ecx -> frame+0x174 (arg3); 0x441329: lea 0xc(%esp),%edx -> frame+0x0 (arg2);
    // 0x44132D: push %ecx; 0x44132E: push %edx; 0x44132F: push %eax (arg1, the container); 0x441330: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57c184)),
                                append_frame, append_frame + 0x174,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FC0()), 7, 0);

    // 0x00441338: push 0; 0x0044133A: push 0; 0x0044133C: call 0x4E0830; 0x00441341: push eax; 0x00441342: push 0x25E8;
    // 0x00441347: push 0x0053C5D4; 0x0044134C: call 0x401900. Note that this registration follows the FIFTEENTH append's own `add esp,0x18`.
    const std::uint16_t scamera_lens_id = TaskType_Register("SCameraLens",             // 0x00441347: push 0x0053C5D4
                                                            0x25E8U,                   // 0x00441342: push 0x25E8
                                                            Sys_GetGlobal16_4E0830(),  // 0x0044133C: call 0x4E0830 (parent type)
                                                            0U);                       // 0x0044133A: push 0
    ::g_57c190 = scamera_lens_id; // 0x00441359: mov word [0x57C190],ax

    // 0x441351: push 0x4415C0 (handler, deferred argument 3);
    // 0x441356: push $0x0 (argument 2, literal event id);
    // 0x44135D: push %eax (argument 1, still the id TaskType_Register returned); 0x44135F: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(scamera_lens_id), 0x0U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x4415C0U)));

    // 0x44136B: push 0x441770 (handler, deferred argument 3);
    // 0x441370: push $0x1 (argument 2, literal event id);
    // 0x441364: mov [0x57c190] into a register (argument 1, the id RELOADED from the global); 0x441373: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_57c190), 0x1U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x441770U)));

    // 0x44137F: push 0x442070 (handler, deferred argument 3);
    // 0x441384: push $0x2 (argument 2, literal event id);
    // 0x441378: mov [0x57c190] into a register (argument 1, the id RELOADED from the global); 0x441387: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_57c190), 0x2U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x442070U)));

    // 0x44138F: push 0x4420D0 (handler, deferred argument 3);
    // 0x441394: call Script_PhaseIdGetE5_4F1A70 (supplies argument 2, the event id);
    // 0x44139A: mov [0x57c190] into a register (argument 1, the id RELOADED from the global); 0x4413A1: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57c190), static_cast<std::uint32_t>(Script_PhaseIdGetE5_4F1A70()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x4420D0U)));

    // 0x4413A9: push 0x4422B0 (handler, deferred argument 3);
    // 0x4413AE: call Sys_GetSettingByte_416840 (supplies argument 2, the event id);
    // 0x4413B3: mov [0x57c190] into a register (argument 1, the id RELOADED from the global); 0x4413BC: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57c190), static_cast<std::uint32_t>(Sys_GetSettingByte_416840()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x4422B0U)));

    // 0x4413C4: push 0x4422F0 (handler, deferred argument 3);
    // 0x4413C9: call Sys_GetSettingByte_416860 (supplies argument 2, the event id);
    // 0x4413CE: mov [0x57c190] into a register (argument 1, the id RELOADED from the global); 0x4413D7: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57c190), static_cast<std::uint32_t>(Sys_GetSettingByte_416860()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x4422F0U)));

    // 0x4413DF: push 0x4423C0 (handler, deferred argument 3);
    // 0x4413E4: call Sys_GetGlobal8_489610 (supplies argument 2, the event id);
    // 0x4413EA: mov [0x57c190] into a register (argument 1, the id RELOADED from the global); 0x4413F1: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57c190), static_cast<std::uint32_t>(Sys_GetGlobal8_489610()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x4423C0U)));

    // 0x4413FD: push 0x442A70 (handler, deferred argument 3);
    // 0x441402: push $0x4 (argument 2, literal event id);
    // 0x4413F6: mov [0x57c190] into a register (argument 1, the id RELOADED from the global); 0x441405: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57c190), 0x4U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x442A70U)));

    // 0x44140D: push 0x442C60 (handler, deferred argument 3);
    // 0x441412: call Task_GetCachedEventId_467D90 (supplies argument 2, the event id);
    // 0x441417: mov [0x57c190] into a register (argument 1, the id RELOADED from the global); 0x441420: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57c190), static_cast<std::uint32_t>(Task_GetCachedEventId_467D90()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x442C60U)));

    // 0x441428: push 0x442DD0 (handler, deferred argument 3);
    // 0x44142D: call Task_GetCachedEventId_467DB0 (supplies argument 2, the event id);
    // 0x441433: mov [0x57c190] into a register (argument 1, the id RELOADED from the global); 0x44143A: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57c190), static_cast<std::uint32_t>(Task_GetCachedEventId_467DB0()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x442DD0U)));

    // 0x441442: push 0x443450 (handler, deferred argument 3);
    // 0x441447: call QTask_PhaseIdGet548177_4CEA20 (supplies argument 2, the event id);
    // 0x44144C: mov [0x57c190] into a register (argument 1, the id RELOADED from the global); 0x441455: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57c190), static_cast<std::uint32_t>(QTask_PhaseIdGet548177_4CEA20()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x443450U)));

    // 0x44145D: push 0x4432F0 (handler, deferred argument 3);
    // 0x441462: call QTask_PhaseIdGet548176_4CEA10 (supplies argument 2, the event id);
    // 0x441467: mov [0x57c190] into a register (argument 1, the id RELOADED from the global); 0x441470: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57c190), static_cast<std::uint32_t>(QTask_PhaseIdGet548176_4CEA10()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x4432F0U)));

    // 0x00441475: push 0x0053C25C ("alarmlight.spr"); 0x0044147A: call 0x4169D0; 0x00441481: mov [0x57C17C],eax - the lookup result is stored
    // BEFORE the next call and is NOT passed to it.
    ::g_57c17c = static_cast<std::uint32_t>(
        reinterpret_cast<std::uintptr_t>(Sys_LookupOrAppendRecord_4169D0("alarmlight.spr"))); // 0x0044147A: call 0x4169D0

    // 0x0044147F: push 0x40 (the list capacity); 0x00441486: call 0x4C1800; 0x0044148B: add esp,0x14 (the twenty-third link's three pushes,
    // the lookup's one and this one); 0x0044148E: mov [0x57C18C],eax.
    ::g_57c18c = reinterpret_cast<std::uint32_t>(QTaskList_New(0x40)); // 0x0044147F: push 0x40

    // 0x00441493: push 0; 0x00441495: push 0; 0x00441497: call 0x4D9FA0 (a DIFFERENT parent type from the two above);
    // 0x0044149C: push eax; 0x0044149D: push 0x138; 0x004414A2: push 0x0053C5C0; 0x004414A7: call 0x401900;
    // 0x004414AC: add esp,0x14 (all five registration pushes at once); 0x004414AF: mov word [0x57C188],ax.
    const std::uint16_t scamera_lens_view_cone_id = TaskType_Register("SCameraLensViewCone",   // 0x004414A2: push 0x0053C5C0
                                                                      0x138U,                  // 0x0044149D: push 0x138
                                                                      Sys_GetGlobal16_4D9FA0(),// 0x00441497: call 0x4D9FA0 (parent type)
                                                                      0U);                     // 0x00441495: push 0
    ::g_57c188 = scamera_lens_view_cone_id; // 0x004414AF: mov word [0x57C188],ax

    // 0x4414B5: push $0x0 (handler immediate ZERO - the handler slot is a NULL HANDLER);
    // 0x4414B7: call Sys_GetSettingByte_416840 (supplies argument 2, the event id);
    // 0x4414BD: mov [0x57c188] into a register (argument 1, the id RELOADED from the global); 0x4414C4: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57c188), static_cast<std::uint32_t>(Sys_GetSettingByte_416840()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0U)));

    // 0x4414CC: push $0x0 (handler immediate ZERO - the handler slot is a NULL HANDLER);
    // 0x4414CE: call Sys_GetSettingByte_416860 (supplies argument 2, the event id);
    // 0x4414D3: mov [0x57c188] into a register (argument 1, the id RELOADED from the global); 0x4414DC: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57c188), static_cast<std::uint32_t>(Sys_GetSettingByte_416860()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0U)));

    // 0x4414E4: push $0x0 (handler immediate ZERO - the handler slot is a NULL HANDLER);
    // 0x4414E6: call Sys_GetGlobal8_489610 (supplies argument 2, the event id);
    // 0x4414EB: mov [0x57c188] into a register (argument 1, the id RELOADED from the global); 0x4414F4: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57c188), static_cast<std::uint32_t>(Sys_GetGlobal8_489610()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0U)));

    // 0x4414FF: push $0x0 (handler immediate ZERO - the handler slot is a NULL HANDLER);
    // 0x441501: push $0xC (argument 2, literal event id);
    // 0x4414F9: mov [0x57c188] into a register (argument 1, the id RELOADED from the global); 0x441504: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57c188), 0xCU,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0U)));

    // 0x441510: push $0x0 (handler immediate ZERO - the handler slot is a NULL HANDLER);
    // 0x441512: push $0xA (argument 2, literal event id);
    // 0x441509: mov [0x57c188] into a register (argument 1, the id RELOADED from the global); 0x441515: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57c188), 0xAU,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0U)));

    // 0x441521: push $0x0 (handler immediate ZERO - the handler slot is a NULL HANDLER);
    // 0x441523: push $0xB (argument 2, literal event id);
    // 0x44151A: mov [0x57c188] into a register (argument 1, the id RELOADED from the global); 0x441526: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57c188), 0xBU,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0U)));

    // 0x44152E: push $0x0 (handler immediate ZERO - the handler slot is a NULL HANDLER);
    // 0x441530: call Render_GetByte_4D14D0 (supplies argument 2, the event id);
    // 0x441536: mov [0x57c188] into a register (argument 1, the id RELOADED from the global); 0x44153D: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57c188), static_cast<std::uint32_t>(Render_GetByte_4D14D0()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0U)));

    // 0x441542: push $0x0 (handler immediate ZERO - the handler slot is a NULL HANDLER);
    // 0x44154B: push $0x9 (argument 2, literal event id);
    // 0x441544: mov [0x57c188] into a register (argument 1, the id RELOADED from the global); 0x44154E: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57c188), 0x9U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0U)));

    // 0x44155A: push $0x0 (handler immediate ZERO - the handler slot is a NULL HANDLER);
    // 0x44155C: push $0x8 (argument 2, literal event id);
    // 0x441553: mov [0x57c188] into a register (argument 1, the id RELOADED from the global); 0x44155F: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57c188), 0x8U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0U)));

    // 0x44156A: push $0x0 (handler immediate ZERO - the handler slot is a NULL HANDLER);
    // 0x44156C: push $0xC (argument 2, literal event id);
    // 0x441564: mov [0x57c188] into a register (argument 1, the id RELOADED from the global); 0x44156F: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57c188), 0xCU,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0U)));

    // 0x441577: push $0x0 (handler immediate ZERO - the handler slot is a NULL HANDLER);
    // 0x441579: call Render_GetByte_4D14D0 (supplies argument 2, the event id);
    // 0x44157E: mov [0x57c188] into a register (argument 1, the id RELOADED from the global); 0x441587: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57c188), static_cast<std::uint32_t>(Render_GetByte_4D14D0()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0U)));

    // 0x0044158C: mov edx,[0x57C188] (RELODED from the global, not taken from the local above); 0x00441592: and $0xffff,%edx;
    // 0x00441598: push edx (argument 2); 0x00441599: push 0x0053C5A0 ("TASKTYPE_SCAMERALENSVIEWCONE", argument 1); 0x0044159E: call 0x4B8810.
    const std::uint32_t registered_id = static_cast<std::uint32_t>(::g_57c188) & 0xFFFFU; // 0x00441592: and $0xffff,%edx
    Script_QHashNodeRegister(const_cast<char*>("TASKTYPE_SCAMERALENSVIEWCONE"),          // 0x00441599: push 0x0053C5A0
                             reinterpret_cast<void*>(static_cast<std::uintptr_t>(registered_id))); // 0x00441598: push edx

    // 0x004415A3: mov ax,[0x57C180] (the SCamera id - the FIRST type registered, reloaded as a WORD, so the upper half of eax is left as retail
    // found it); 0x004415A9: push eax; 0x004415AA: call 0x489640; 0x004415AF: mov [0x57C198],eax.
    // The callee reads the argument back with `mov ax,[esp+4]`, so only the low sixteen bits are ever consumed - the port zero-extends instead of
    // leaving an undefined upper half, which cannot change behaviour because no instruction on either side reads those bits.
    ::g_57c198 = AiDetection_RegisterTaskMask_489640(static_cast<std::uint16_t>(::g_57c180)); // 0x004415AA: call 0x489640

    // 0x004415B4: add esp,0x1E0 (0x1C8 frame plus the thirty-fourth link's three pushes, the hash registration's two and the mask registration's
    // one); 0x004415BA: ret.
} // end of Task_DefineSCameraTypes

} // extern "C"
