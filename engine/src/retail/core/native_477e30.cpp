// Native: Task_DefineGun (behavior-derived placeholder name; retail symbol unknown); retail VA 0x00477E30-0x00478285.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 1110 bytes, 1 block, 295 instructions.
// Tools/evidence: radare2 6.2.0 af/afbj/pdf with bin.relocs.apply over the exact image (1 block / 295 instructions / 1110 bytes / 0 edges; `pd`
//   stops at the ret at 0x00478285) plus the objdump listing build/retail-dis.txt for the same extent, read in four explicit ranges
//   (0x00477E30-0x00477F3E, 0x00477F3E-0x0047804E, 0x0047804E-0x0047816E, 0x0047816E-0x00478286) so no part of the body is transcribed from a
//   summary; `ps` at 0x00540800, 0x00540810 and 0x0054081C for the three literals (byte-identical to "TASKTYPE_GUN", "GunCasing" and "Gun");
//   `pxw` at 0x005BE3C0, 0x005BE3C4, 0x005BE3C6 and 0x005407B8-0x005407BC for the image values; a full stack-pointer simulation over all 295
//   instructions, which returns esp to its entry value at the ret and accounts for every cleanup (0x40 = 16 pushes, 0xc x25, 0x24 = 9,
//   0x18 = 6 twice, 0x10 = 4, 0x44 = 17, 0x1c4 = the 0x1b0 frame plus 5 outstanding pushes). Every getter callee was re-read at its own VA and
//   is a single-instruction argument-less load ending in a ret, except the two that are documented as real routines below; each pre-pushed value
//   is simultaneously the next 0x00401400/0x00401530 link call's handler slot, which the stack simulation confirms because the getter is
//   __cdecl and cleans nothing. Argument numbers below were derived by counting the pushes above each access, not by reading an [esp+N] offset
//   as an argument number (the rule the Lead confirmed on Mem_AllocIndexedBlock 0x004B0B14).
// Behavior/CFG: one straight-line body that registers TWO task types and then wires thirty-five handler links, in retail order:
//   (1) "Gun", size 0x1B0, parent Sys_GetGlobal16_4C48C0() -> id at 0x005BE3C6, followed by five QTask_AllocateEvent(0) calls whose low bytes go
//       to 0x005407B8, 0x005407B9, 0x005407BA, 0x005407BB and 0x005407BC in that order, then links e0->0x00478540 (0x00401400), e1->0x00478640
//       and e2->0x00478840 (0x00401530), then the event id from 0x004D9850->0x00478900, e4->0x00478970, e7 with a NULL handler, the event ids from
//       0x004F1A60->0x004789B0, 0x004F1A70->0x004789C0, 0x00416830->0x0045E3D0, 0x00416840->0x004D3710, 0x00416850->0x0045F920,
//       0x00416880->0x0045F920, 0x004F1FD0->0x00478A80, 0x004F1FE0->0x00478A90, 0x004F1FF0->0x00478BA0, 0x004F2000->0x004798C0,
//       0x004F2010->0x00479AA0, 0x004F2020->0x00479B20 (0x00401530), 0x004F2030->0x00479C10 (0x00401530), 0x004F2040->0x00479C20,
//       0x00413B50->0x00479C80, 0x00413B60->0x00479CA0, 0x00487190->0x00479D20, 0x00489610->0x0047A8E0, 0x0047B100->0x0047A940,
//       0x00460BF0->0x00479780, 0x00460C10->0x00479990, 0x004CEA20->0x0047A960, 0x004CEA10->0x0047A9E0 and 0x0047E6F0 with a NULL handler.
//   (2) the script container for the Gun type: Script_TaskContainer_Alloc(3) stored at 0x005BE3C0, then TWO appends recording frame differences
//       0x104 and 0x124.
//   (3) "GunCasing", size 0x128, parent Sys_GetGlobal16_4C48C0() -> id at 0x005BE3C4, links e0->0x00478290 (0x00401400), e1->0x004782D0 and
//       e2->0x004784F0 (0x00401530), e4->0x00478510, then the event id from 0x00416830->0x0045E3D0.
//   (4) the script-name tail: Script_QHashNodeRegister("TASKTYPE_GUN", the Gun id) through 0x004B8810.
//   One basic block, no branch, no loop, 77 call sites over 36 distinct callees. The frame size 0x1B0 equals the size passed to
//   TaskType_Register for the Gun type, which is the same frame-is-task-object relation the Lead recorded for the seven framed registrars.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. THIS NATIVE HAS THE 0x004B8810 SCRIPT-NAME TAIL that the eight registrars transcribed before it
//   do not have, so "registrars have no script-name tail" is NOT a general rule - it was an over-generalisation from eight bodies, and this
//   body is the counter-example. The tail re-loads the Gun id with `xor ecx,ecx; mov cx,[0x005BE3C6]` (an explicit zero-extension) and passes
//   it as Script_QHashNodeRegister's SECOND argument, with the literal "TASKTYPE_GUN" first. Documented shape differences, all value-preserving:
//   (1) every getter callee is an argument-less load in retail that ignores the arguments the caller pushes, and the tree's transcriptions take
//   no arguments, so those inert pushes are not reproduced; each is simultaneously the next link call's pre-pushed handler, which the port keeps
//   as the handler value in the same order; (2) byte-valued event ids are zero-extended in the port, which cannot change behaviour because every
//   link fed by a byte getter here goes through 0x00401400, which masks the event word to its low byte for the phase index - no 0x00401530 link
//   in this body takes its event from a byte getter (its events are the constants 1 and 2, or the dword getters 0x004F2020/0x004F2030);
//   (3) the appends pass five arguments, as script/native_4f1a80.cpp declares and as the tree's other appends do, while retail's sixth stack slot
//   is the leftover first push of the preceding getter call and is never read by 0x004F1A80 - the same documented ABI-shape difference the
//   0x004F1A80 record already carries. Globals: 0x005BE3C6 is OWNED by sys/native_477bf0.cpp (the getter for it), so this file declares it
//   extern and does not define it; 0x005BE3C0, 0x005BE3C4 and the five bytes 0x005407B8-0x005407BC are unowned and defined here with their
//   retail image values (0 for the first two, 0xFF for each of the five bytes). The five allocated event ids are never read by this body; the
//   readers are elsewhere. 0x004F1E20 reads 0x0054E158 whose image value is 0xFFFFFFFF, so the value passed as the first append's fourth
//   argument is -1 in the image, which is exactly the value 0x004F1A80 rejects with Debug_Error("Undefined parameter type.") - something writes
//   that slot before this runs, and that writer is not identified here; it is recorded as an open item, not as a claim. Handlers 0x00478540,
//   0x00478640, 0x00478840, 0x00478900, 0x00478970, 0x004789B0, 0x004789C0, 0x00478A80, 0x00478A90, 0x00478BA0, 0x004798C0, 0x00479AA0,
//   0x00479B20, 0x00479C10, 0x00479C20, 0x00479C80, 0x00479CA0, 0x00479D20, 0x0047A8E0, 0x0047A940, 0x00479780, 0x00479990, 0x0047A960,
//   0x0047A9E0, 0x00478290, 0x004782D0, 0x004784F0 and 0x00478510 are raw code immediates in retail and are not transcribed yet (native gaps in
//   0x0046xxxx-0x0048xxxx); 0x004D3710 is already transcribed in script/native_4D3710.cpp. Handlers 0x0045E3D0 and 0x0045F920 are BELOW
//   0x00460000 and therefore belong to the 0x00400000-0x0045FFFF owner, not to this file. New callee gaps outside this file's scope: 0x00413B50,
//   0x00413B60, 0x00413DE0 and 0x00460BF0 have no implementation anywhere in the tree (0x00413B50/0x00413B60/0x00460BF0 are argument-less or
//   lazy-allocation getters, 0x00413DE0 returns the constant address 0x0056E228). Domain: core/, because it is one of the 0x00402930 startup
//   registrars.
#include "igi/retail/core/task_types.hpp"

#include <cstdint>

extern "C" std::uint16_t Sys_GetGlobal16_4C48C0(void);              // 0x004C48C0: argument-less word getter
extern "C" std::uint8_t Sys_GetGlobal8_4D9850(void);                // 0x004D9850: argument-less byte getter (sys/native_4d9850.cpp)
extern "C" std::uint8_t Script_PhaseIdGetE4_4F1A60(void);           // 0x004F1A60: argument-less byte getter (script/script_state.hpp)
extern "C" std::uint8_t Script_PhaseIdGetE5_4F1A70(void);           // 0x004F1A70: argument-less byte getter (script/script_state.hpp)
extern "C" std::uint8_t Sys_GetSettingByte_416830(void);            // 0x00416830: argument-less byte getter
extern "C" std::uint8_t Sys_GetSettingByte_416840(void);            // 0x00416840: argument-less byte getter (config/native_416840.cpp)
extern "C" std::uint8_t Sys_GetSettingByte_416850(void);            // 0x00416850: argument-less byte getter
extern "C" std::uint8_t Sys_GetSettingByte_416880(void);            // 0x00416880: argument-less byte getter
extern "C" std::uint8_t Sys_GetGlobal8_4F1FD0(void);                // 0x004F1FD0: argument-less byte getter (sys/native_4f1fd0.cpp)
extern "C" std::uint8_t Sys_GetGlobal8_4F1FE0(void);                // 0x004F1FE0: argument-less byte getter (sys/native_4f1fe0.cpp)
extern "C" std::uint8_t Sys_GetGlobal8_4F1FF0(void);                // 0x004F1FF0: argument-less byte getter (sys/native_4f1ff0.cpp)
extern "C" std::uint8_t Sys_GetGlobal8_4F2000(void);                // 0x004F2000: argument-less byte getter (sys/native_4f2000.cpp)
extern "C" std::uint8_t Sys_GetGlobal8_4F2010(void);                // 0x004F2010: argument-less byte getter (sys/native_4f2010.cpp)
extern "C" std::uint32_t Sys_GetGlobal32_4F2020(void);              // 0x004F2020: argument-less dword getter (sys/native_4f2020.cpp)
extern "C" std::uint32_t Sys_GetGlobal32_4F2030(void);              // 0x004F2030: argument-less dword getter (sys/native_4f2030.cpp)
extern "C" std::uint8_t Sys_GetGlobal8_4F2040(void);                // 0x004F2040: argument-less byte getter (sys/native_4f2040.cpp)
extern "C" std::uint8_t Sys_GetGlobal8_487190(void);                // 0x00487190: argument-less byte getter (sys/native_487190.cpp)
extern "C" std::uint8_t Sys_GetGlobal8_489610(void);                // 0x00489610: argument-less byte getter (sys/native_489610.cpp)
extern "C" std::uint8_t Sys_GetGlobal8_47B100(void);                // 0x0047B100: argument-less byte getter (sys/native_47b100.cpp)
extern "C" std::uint8_t Task_GetCachedEventId_460C10(void);         // 0x00460C10: lazily allocated event id (core/native_460c10.cpp)
extern "C" std::uint8_t QTask_PhaseIdGet548177_4CEA20(void);        // 0x004CEA20: argument-less byte getter (script/native_4cea20.cpp)
extern "C" std::uint8_t QTask_PhaseIdGet548176_4CEA10(void);        // 0x004CEA10: argument-less byte getter (script/native_4cea10.cpp)
extern "C" std::uint8_t Sys_GetGlobal8_47E6F0(void);                // 0x0047E6F0: argument-less byte getter (sys/native_47e6f0.cpp)
extern "C" void* Script_TaskContainer_Alloc(std::int32_t const count); // 0x004F1950
extern "C" std::uint32_t Sys_GetGlobal32_4F1E20(void);              // 0x004F1E20: argument-less dword getter (sys/native_4f1e20.cpp)
extern "C" std::uint32_t Sys_GetGlobal32_4F0FB0(void);              // 0x004F0FB0: argument-less dword getter (sys/native_4f0fb0.cpp)
// 0x004F1A80 (script/native_4f1a80.cpp). SIX arguments, corrected: the callee reads arg1..arg4, arg5 as a byte (recorded at record +0x0C)
// and arg6 as a dword (recorded at record +0x08). The two reads are both `[esp+0x20]` in retail, at different push depths, with a `pop edi`
// between them (0x004F1AB1 with twelve bytes pushed = arg5, 0x004F1ACA with eight = arg6). Earlier revisions of this file declared five
// parameters and the tree's implementation wrote arg5 into both fields; both are now corrected. Every append below passes the sixth word that
// retail pushes for it, so the 0x00413DE0 result at 0x0047819F and the 0x004F1CB0-style values are real arguments rather than discarded ones.
extern "C" void* Script_TaskContainer_Append(std::int32_t* const container, std::int32_t const arg2,
                                             std::int32_t const arg3, std::int32_t const arg4,
                                             std::int32_t const arg5, std::int32_t const arg6); // 0x004F1A80
extern "C" void Script_QHashNodeRegister(void* arg1, void* value);  // 0x004B8810 (script/native_4b8810.cpp)

// 0x00413B50, 0x00413B60, 0x00413DE0 and 0x00460BF0 have no implementation in the tree. Placeholder names describe the proven shape only.
extern "C" std::uint8_t Sys_GetSettingByte_413B50(void);            // 0x00413B50: argument-less byte getter, reads 0x005391B1
extern "C" std::uint8_t Sys_GetSettingByte_413B60(void);            // 0x00413B60: argument-less byte getter, reads 0x005391B2
extern "C" std::uint32_t Sys_GetGlobalPtr_413DE0(void);             // 0x00413DE0: argument-less getter returning the constant 0x0056E228
extern "C" std::uint8_t Task_GetCachedEventId_460BF0(void);         // 0x00460BF0: lazily allocated event id, reads/writes 0x0053F314

extern "C" {

// 0x005BE3C6: the "Gun" task type id. OWNED by sys/native_477bf0.cpp (the getter at 0x00477BF0 that returns it), so it is declared here and not
// defined.
extern std::uint16_t g_5be3c6;

// 0x005BE3C0: the Gun type's script container, a dword; retail stores the container pointer with `mov %eax,0x5BE3C0` (0x00478195).
std::uint32_t g_5be3c0 = 0U;

// 0x005BE3C4: the "GunCasing" task type id; retail stores it with a 16-bit store (0x0047820B).
std::uint16_t g_5be3c4 = 0U;

// 0x005407B8-0x005407BC: five consecutive one-byte event id slots, each with retail image value 0xFF (meaning "not allocated yet"); retail
// stores them with `mov byte [...],al` at 0x00477E5E, 0x00477E6A, 0x00477E76, 0x00477E82 and 0x00477E8C.
std::uint8_t g_5407b8 = 0xFFU;
std::uint8_t g_5407b9 = 0xFFU;
std::uint8_t g_5407ba = 0xFFU;
std::uint8_t g_5407bb = 0xFFU;
std::uint8_t g_5407bc = 0xFFU;

void Task_DefineGun()
{
    // The `sub esp,0x1b0` frame at 0x00477E30 is the Gun task object (0x1B0 is also the size passed to TaskType_Register below). The two
    // 0x004F1A80 calls pass two addresses inside it: the frame's first dword (each `lea ecx,[esp+0xc]` runs with 12 bytes of arguments already
    // pushed, so the address is frame+0x00) and one later slot each. 0x004F1A80 reads those two arguments at [esp+8]/[esp+0xC], subtracts them at
    // 0x004F1A88 and never dereferences them, so the port keeps one addressable dword and passes that address plus the same retail offset.
    std::int32_t append_frame_base = 0; // retail frame slot at +0x00
    const std::int32_t append_frame = reinterpret_cast<std::int32_t>(&append_frame_base);

    // ---- Registration 1: "Gun" ---------------------------------------------------------------------------------------------------------
    // 0x00477E36/0x00477E38: push 0; push 0 (inert arguments of the getter, and TaskType_Register's fourth/fifth slots);
    // 0x00477E3A: call 0x4C48C0; 0x00477E3F: push eax; 0x00477E40: push 0x1B0; 0x00477E45: push "Gun"; 0x00477E4A: call 0x401900.
    const std::uint16_t gun_id = TaskType_Register("Gun", // 0x00477E45: push 0x0054081C
                                                   0x1B0U, // 0x00477E40: push 0x1B0 (object size, equal to the frame size)
                                                   Sys_GetGlobal16_4C48C0(), // 0x00477E3A: call 0x4C48C0 (parent type)
                                                   0U);   // 0x00477E38: push 0 (task list count)

    // 0x00477E4F: push 0 (the first allocation's argument); 0x00477E51: mov word [0x5BE3C6],ax; 0x00477E57: call 0x4017C0. The store lands
    // between the push and the call, exactly as retail orders it.
    ::g_5be3c6 = gun_id; // 0x00477E51: a plain 16-bit store into the 16-bit slot owned by sys/native_477bf0.cpp
    ::g_5407b8 = static_cast<std::uint8_t>(QTask_AllocateEvent(0U)); // 0x00477E57: the first allocated event id

    // 0x00477E5C: push 0; 0x00477E5E: mov byte [0x5407B8],al; 0x00477E63: call 0x4017C0; 0x00477E68: push 0;
    // 0x00477E6A: mov byte [0x5407B9],al; 0x00477E6F: call 0x4017C0.
    ::g_5407b9 = static_cast<std::uint8_t>(QTask_AllocateEvent(0U)); // 0x00477E6F: the second allocated event id

    // 0x00477E74: push 0; 0x00477E76: mov byte [0x5407BA],al; 0x00477E7B: call 0x4017C0; 0x00477E80: push 0;
    // 0x00477E82: mov byte [0x5407BB],al; 0x00477E87: call 0x4017C0.
    ::g_5407ba = static_cast<std::uint8_t>(QTask_AllocateEvent(0U)); // 0x00477E7B: the third allocated event id
    ::g_5407bb = static_cast<std::uint8_t>(QTask_AllocateEvent(0U)); // 0x00477E87: the fourth allocated event id

    // 0x00477E8C: mov byte [0x5407BC],al - the fifth allocation's low byte, stored with no push of its own before it.
    ::g_5407bc = static_cast<std::uint8_t>(QTask_AllocateEvent(0U)); // 0x00477E8C: the fifth allocated event id

    // 0x00477E91: mov ax,[0x5BE3C6]; 0x00477E97: push 0x00478540; 0x00477E9C: push 0; 0x00477E9E: push eax; 0x00477E9F: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be3c6), // 0x00477E91: mov ax,[0x5BE3C6]
                                0U,                                     // 0x00477E9C: push 0 (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00478540U))); // 0x00477E97: push handler

    // 0x00477EA4: mov cx,[0x5BE3C6]; 0x00477EAB: push 0x00478640; 0x00477EB0: push 1; 0x00477EB2: push ecx; 0x00477EB3: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_5be3c6), // 0x00477EA4: mov cx,[0x5BE3C6]
                                           1U,                                     // 0x00477EB0: push 1 (argument 2)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x00478640U))); // 0x00477EAB: push handler

    // 0x00477EB8: mov dx,[0x5BE3C6]; 0x00477EBF: add esp,0x40 - 16 pushes above; 0x00477EC2: push 0x00478840; 0x00477EC7: push 2;
    // 0x00477EC9: push edx; 0x00477ECA: call 0x401530; 0x00477ECF: add esp,0xc.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_5be3c6), // 0x00477EB8: mov dx,[0x5BE3C6]
                                           2U,                                     // 0x00477EC7: push 2 (argument 2)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x00478840U))); // 0x00477EC2: push handler

    // 0x00477ED2: push 0x00478900 (pre-pushed handler, and the getter's inert argument); 0x00477ED7: call 0x4D9850 (event id);
    // 0x00477EDC: push eax; 0x00477EDD: mov ax,[0x5BE3C6]; 0x00477EE3: push eax; 0x00477EE4: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be3c6), // 0x00477EDD: mov ax,[0x5BE3C6]
                                static_cast<std::uint32_t>(Sys_GetGlobal8_4D9850()), // 0x00477EDC: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00478900U))); // 0x00477ED2: push handler

    // 0x00477EE9: mov cx,[0x5BE3C6]; 0x00477EF0: push 0x00478970; 0x00477EF5: push 4; 0x00477EF7: push ecx; 0x00477EF8: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be3c6), // 0x00477EE9: mov cx,[0x5BE3C6]
                                4U,                                     // 0x00477EF5: push 4 (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00478970U))); // 0x00477EF0: push handler

    // 0x00477EFD: mov dx,[0x5BE3C6]; 0x00477F04: push 0 (a NULL handler); 0x00477F06: push 7; 0x00477F08: push edx; 0x00477F09: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be3c6), // 0x00477EFD: mov dx,[0x5BE3C6]
                                7U,                                     // 0x00477F06: push 7 (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0U))); // 0x00477F04: push 0 (argument 3: a NULL handler)

    // 0x00477F0E: add esp,0x24 - 9 pushes above; 0x00477F11: push 0x004789B0; 0x00477F16: call 0x4F1A60 (event id); 0x00477F1B: push eax;
    // 0x00477F1C: mov ax,[0x5BE3C6]; 0x00477F22: push eax; 0x00477F23: call 0x401400; 0x00477F28: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be3c6), // 0x00477F1C: mov ax,[0x5BE3C6]
                                static_cast<std::uint32_t>(Script_PhaseIdGetE4_4F1A60()), // 0x00477F1B: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x004789B0U))); // 0x00477F11: push handler

    // 0x00477F2B: push 0x004789C0; 0x00477F30: call 0x4F1A70 (event id); 0x00477F35: mov cx,[0x5BE3C6]; 0x00477F3C: push eax;
    // 0x00477F3D: push ecx; 0x00477F3E: call 0x401400; 0x00477F43: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be3c6), // 0x00477F35: mov cx,[0x5BE3C6]
                                static_cast<std::uint32_t>(Script_PhaseIdGetE5_4F1A70()), // 0x00477F3C: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x004789C0U))); // 0x00477F2B: push handler

    // 0x00477F46: push 0x0045E3D0; 0x00477F4B: call 0x416830; 0x00477F50: mov dx,[0x5BE3C6]; 0x00477F57: push eax; 0x00477F58: push edx;
    // 0x00477F59: call 0x401400; 0x00477F5E: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be3c6), // 0x00477F50: mov dx,[0x5BE3C6]
                                static_cast<std::uint32_t>(Sys_GetSettingByte_416830()), // 0x00477F57: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x0045E3D0U))); // 0x00477F46: push handler (below 0x00460000: other owner's VA)

    // 0x00477F61: push 0x004D3710; 0x00477F66: call 0x416840; 0x00477F6B: push eax; 0x00477F6C: mov ax,[0x5BE3C6]; 0x00477F72: push eax;
    // 0x00477F73: call 0x401400; 0x00477F78: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be3c6), // 0x00477F6C: mov ax,[0x5BE3C6]
                                static_cast<std::uint32_t>(Sys_GetSettingByte_416840()), // 0x00477F6B: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x004D3710U))); // 0x00477F61: push handler (already transcribed)

    // 0x00477F7B: push 0x0045F920; 0x00477F80: call 0x416850; 0x00477F85: mov cx,[0x5BE3C6]; 0x00477F8C: push eax; 0x00477F8D: push ecx;
    // 0x00477F8E: call 0x401400; 0x00477F93: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be3c6), // 0x00477F85: mov cx,[0x5BE3C6]
                                static_cast<std::uint32_t>(Sys_GetSettingByte_416850()), // 0x00477F8C: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x0045F920U))); // 0x00477F7B: push handler (below 0x00460000)

    // 0x00477F96: push 0x0045F920; 0x00477F9B: call 0x416880; 0x00477FA0: mov dx,[0x5BE3C6]; 0x00477FA7: push eax; 0x00477FA8: push edx;
    // 0x00477FA9: call 0x401400; 0x00477FAE: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be3c6), // 0x00477FA0: mov dx,[0x5BE3C6]
                                static_cast<std::uint32_t>(Sys_GetSettingByte_416880()), // 0x00477FA7: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x0045F920U))); // 0x00477F96: push handler (below 0x00460000)

    // 0x00477FB1: push 0x00478A80; 0x00477FB6: call 0x4F1FD0; 0x00477FBB: push eax; 0x00477FBC: mov ax,[0x5BE3C6]; 0x00477FC2: push eax;
    // 0x00477FC3: call 0x401400; 0x00477FC8: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be3c6), // 0x00477FBC: mov ax,[0x5BE3C6]
                                static_cast<std::uint32_t>(Sys_GetGlobal8_4F1FD0()), // 0x00477FBB: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00478A80U))); // 0x00477FB1: push handler

    // 0x00477FCB: push 0x00478A90; 0x00477FD0: call 0x4F1FE0; 0x00477FD5: mov cx,[0x5BE3C6]; 0x00477FDC: push eax; 0x00477FDD: push ecx;
    // 0x00477FDE: call 0x401400; 0x00477FE3: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be3c6), // 0x00477FD5: mov cx,[0x5BE3C6]
                                static_cast<std::uint32_t>(Sys_GetGlobal8_4F1FE0()), // 0x00477FDC: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00478A90U))); // 0x00477FCB: push handler

    // 0x00477FE6: push 0x00478BA0; 0x00477FEB: call 0x4F1FF0; 0x00477FF0: mov dx,[0x5BE3C6]; 0x00477FF7: push eax; 0x00477FF8: push edx;
    // 0x00477FF9: call 0x401400; 0x00477FFE: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be3c6), // 0x00477FF0: mov dx,[0x5BE3C6]
                                static_cast<std::uint32_t>(Sys_GetGlobal8_4F1FF0()), // 0x00477FF7: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00478BA0U))); // 0x00477FE6: push handler

    // 0x00478001: push 0x004798C0; 0x00478006: call 0x4F2000; 0x0047800B: push eax; 0x0047800C: mov ax,[0x5BE3C6]; 0x00478012: push eax;
    // 0x00478013: call 0x401400; 0x00478018: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be3c6), // 0x0047800C: mov ax,[0x5BE3C6]
                                static_cast<std::uint32_t>(Sys_GetGlobal8_4F2000()), // 0x0047800B: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x004798C0U))); // 0x00478001: push handler

    // 0x0047801B: push 0x00479AA0; 0x00478020: call 0x4F2010; 0x00478025: mov cx,[0x5BE3C6]; 0x0047802C: push eax; 0x0047802D: push ecx;
    // 0x0047802E: call 0x401400; 0x00478033: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be3c6), // 0x00478025: mov cx,[0x5BE3C6]
                                static_cast<std::uint32_t>(Sys_GetGlobal8_4F2010()), // 0x0047802C: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00479AA0U))); // 0x0047801B: push handler

    // 0x00478036: push 0x00479B20; 0x0047803B: call 0x4F2020 (a dword getter, so the event word is 32-bit here); 0x00478040: mov dx,[0x5BE3C6];
    // 0x00478047: push eax; 0x00478048: push edx; 0x00478049: call 0x401530; 0x0047804E: add esp,0xc.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_5be3c6), // 0x00478040: mov dx,[0x5BE3C6]
                                           Sys_GetGlobal32_4F2020(), // 0x00478047: push eax (argument 2, the full 32-bit word)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x00479B20U))); // 0x00478036: push handler

    // 0x00478051: push 0x00479C10; 0x00478056: call 0x4F2030 (a dword getter); 0x0047805B: push eax; 0x0047805C: mov ax,[0x5BE3C6];
    // 0x00478062: push eax; 0x00478063: call 0x401530; 0x00478068: add esp,0xc.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_5be3c6), // 0x0047805C: mov ax,[0x5BE3C6]
                                           Sys_GetGlobal32_4F2030(), // 0x0047805B: push eax (argument 2, the full 32-bit word)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x00479C10U))); // 0x00478051: push handler

    // 0x0047806B: push 0x00479C20; 0x00478070: call 0x4F2040; 0x00478075: mov cx,[0x5BE3C6]; 0x0047807C: push eax; 0x0047807D: push ecx;
    // 0x0047807E: call 0x401400; 0x00478083: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be3c6), // 0x00478075: mov cx,[0x5BE3C6]
                                static_cast<std::uint32_t>(Sys_GetGlobal8_4F2040()), // 0x0047807C: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00479C20U))); // 0x0047806B: push handler

    // 0x00478086: push 0x00479C80; 0x0047808B: call 0x413B50; 0x00478090: mov dx,[0x5BE3C6]; 0x00478097: push eax; 0x00478098: push edx;
    // 0x00478099: call 0x401400; 0x0047809E: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be3c6), // 0x00478090: mov dx,[0x5BE3C6]
                                static_cast<std::uint32_t>(Sys_GetSettingByte_413B50()), // 0x00478097: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00479C80U))); // 0x00478086: push handler

    // 0x004780A1: push 0x00479CA0; 0x004780A6: call 0x413B60; 0x004780AB: push eax; 0x004780AC: mov ax,[0x5BE3C6]; 0x004780B2: push eax;
    // 0x004780B3: call 0x401400; 0x004780B8: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be3c6), // 0x004780AC: mov ax,[0x5BE3C6]
                                static_cast<std::uint32_t>(Sys_GetSettingByte_413B60()), // 0x004780AB: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00479CA0U))); // 0x004780A1: push handler

    // 0x004780BB: push 0x00479D20; 0x004780C0: call 0x487190; 0x004780C5: mov cx,[0x5BE3C6]; 0x004780CC: push eax; 0x004780CD: push ecx;
    // 0x004780CE: call 0x401400; 0x004780D3: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be3c6), // 0x004780C5: mov cx,[0x5BE3C6]
                                static_cast<std::uint32_t>(Sys_GetGlobal8_487190()), // 0x004780CC: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00479D20U))); // 0x004780BB: push handler

    // 0x004780D6: push 0x0047A8E0; 0x004780DB: call 0x489610; 0x004780E0: mov dx,[0x5BE3C6]; 0x004780E7: push eax; 0x004780E8: push edx;
    // 0x004780E9: call 0x401400; 0x004780EE: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be3c6), // 0x004780E0: mov dx,[0x5BE3C6]
                                static_cast<std::uint32_t>(Sys_GetGlobal8_489610()), // 0x004780E7: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x0047A8E0U))); // 0x004780D6: push handler

    // 0x004780F1: push 0x0047A940; 0x004780F6: call 0x47B100; 0x004780FB: push eax; 0x004780FC: mov ax,[0x5BE3C6]; 0x00478102: push eax;
    // 0x00478103: call 0x401400; 0x00478108: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be3c6), // 0x004780FC: mov ax,[0x5BE3C6]
                                static_cast<std::uint32_t>(Sys_GetGlobal8_47B100()), // 0x004780FB: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x0047A940U))); // 0x004780F1: push handler

    // 0x0047810B: push 0x00479780; 0x00478110: call 0x460BF0 (a real routine: it allocates the event id on first use); 0x00478115: mov cx,[0x5BE3C6];
    // 0x0047811C: push eax; 0x0047811D: push ecx; 0x0047811E: call 0x401400; 0x00478123: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be3c6), // 0x00478115: mov cx,[0x5BE3C6]
                                static_cast<std::uint32_t>(Task_GetCachedEventId_460BF0()), // 0x0047811C: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00479780U))); // 0x0047810B: push handler

    // 0x00478126: push 0x00479990; 0x0047812B: call 0x460C10; 0x00478130: mov dx,[0x5BE3C6]; 0x00478137: push eax; 0x00478138: push edx;
    // 0x00478139: call 0x401400; 0x0047813E: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be3c6), // 0x00478130: mov dx,[0x5BE3C6]
                                static_cast<std::uint32_t>(Task_GetCachedEventId_460C10()), // 0x00478137: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00479990U))); // 0x00478126: push handler

    // 0x00478141: push 0x0047A960; 0x00478146: call 0x4CEA20; 0x0047814B: push eax; 0x0047814C: mov ax,[0x5BE3C6]; 0x00478152: push eax;
    // 0x00478153: call 0x401400; 0x00478158: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be3c6), // 0x0047814C: mov ax,[0x5BE3C6]
                                static_cast<std::uint32_t>(QTask_PhaseIdGet548177_4CEA20()), // 0x0047814B: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x0047A960U))); // 0x00478141: push handler

    // 0x0047815B: push 0x0047A9E0; 0x00478160: call 0x4CEA10; 0x00478165: mov cx,[0x5BE3C6]; 0x0047816C: push eax; 0x0047816D: push ecx;
    // 0x0047816E: call 0x401400; 0x00478173: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be3c6), // 0x00478165: mov cx,[0x5BE3C6]
                                static_cast<std::uint32_t>(QTask_PhaseIdGet548176_4CEA10()), // 0x0047816C: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x0047A9E0U))); // 0x0047815B: push handler

    // 0x00478176: push 0 (a NULL handler, and the getter's inert argument); 0x00478178: call 0x47E6F0 (event id);
    // 0x0047817D: mov dx,[0x5BE3C6]; 0x00478184: push eax; 0x00478185: push edx; 0x00478186: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be3c6), // 0x0047817D: mov dx,[0x5BE3C6]
                                static_cast<std::uint32_t>(Sys_GetGlobal8_47E6F0()), // 0x00478184: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0U))); // 0x00478176: push 0 (argument 3: a NULL handler)

    // ---- The Gun type's script container and its two appends ---------------------------------------------------------------
    // 0x0047818B: push 3; 0x0047818D: call 0x4F1950; 0x00478192: add esp,0x10 - the three pushes of the link above plus this one;
    // 0x00478195: mov dword [0x5BE3C0],eax.
    ::g_5be3c0 = static_cast<std::uint32_t>(
        reinterpret_cast<std::uintptr_t>(Script_TaskContainer_Alloc(3))); // 0x0047818B: push 3

    // Append 1: 0x0047819A: call 0x413DE0; 0x0047819F/0x004781A0: push eax; push 7;
    // 0x004781A2: call 0x4F1E20; 0x004781A7: mov edx,[0x5BE3C0]; 0x004781AD: push eax; 0x004781AE: lea eax,[esp+0x110] (frame+0x104);
    // 0x004781B5: lea ecx,[esp+0xc] (frame+0x00); 0x004781BC: call 0x4F1A80; 0x004781C1: add esp,0x18.
    //
    // 0x0047819A is a real call site whose result is an argument: 0x413DE0 is an argument-less getter returning the constant address
    // 0x0056E228, and retail pushes that result first at 0x0047819F, which makes it the SIXTH and last argument of the 0x4F1A80 call below.
    // 0x004F1A80 reads six arguments and stores arg6 at record +0x08, so the value is passed here rather than discarded.
    const std::uint32_t casing_parameter_address =
        static_cast<std::uint32_t>(Sys_GetGlobalPtr_413DE0()); // 0x0047819A: call 0x413DE0 (0x0056E228, the append's sixth argument)

    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(::g_5be3c0), // argument 1: the container
                                append_frame,                                 // argument 2: retail frame+0x00
                                append_frame + 0x104,                         // argument 3: retail frame+0x104 (recorded difference 0x104)
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F1E20()), // argument 4: the type from 0x4F1E20
                                7,                                            // argument 5: retail's leftover 7
                                static_cast<std::int32_t>(casing_parameter_address)); // argument 6: 0x0056E228

    // Append 2: 0x004781C4/0x004781C6: push 0; push 7 (the first is the next getter's inert argument); 0x004781C8: call 0x4F0FB0;
    // 0x004781CD: mov edx,[0x5BE3C0]; 0x004781D4: lea eax,[esp+0x130] (frame+0x124); 0x004781DB: lea ecx,[esp+0xc] (frame+0x00);
    // 0x004781E2: call 0x4F1A80; 0x004781E7: add esp,0x18.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(::g_5be3c0),
                                append_frame,
                                append_frame + 0x124,                         // 0x004781D4: frame+0x124 (recorded difference 0x124)
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FB0()),
                                7, 0);                                        // argument 5/6: retail pushes 0 then 7

    // ---- Registration 2: "GunCasing" ---------------------------------------------------------------------------------------------------
    // 0x004781EA/0x004781EC: push 0; push 0x32 (both inert arguments of the getter); 0x004781EE: call 0x4C48C0; 0x004781F3: push eax;
    // 0x004781F4: push 0x128; 0x004781F9: push "GunCasing"; 0x004781FE: call 0x401900.
    const std::uint16_t gun_casing_id = TaskType_Register("GunCasing", // 0x004781F9: push 0x00540810
                                                          0x128U,       // 0x004781F4: push 0x128 (object size)
                                                          Sys_GetGlobal16_4C48C0(), // 0x004781EE: call 0x4C48C0 (parent type)
                                                          0U);          // 0x004781EC: push 0x32 (inert)

    // 0x00478203: push 0x00478290 (pre-pushed handler); 0x00478208: push 0; 0x0047820A: push eax; 0x0047820B: mov word [0x5BE3C4],ax;
    // 0x00478211: call 0x401400.
    ::g_5be3c4 = gun_casing_id; // 0x0047820B: a plain 16-bit store into the 16-bit slot
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be3c4), // 0x00478216: mov ax,[0x5BE3C4]
                                0U,                                     // 0x00478208: push 0 (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00478290U))); // 0x00478203: push handler

    // 0x0047821C: push 0x004782D0; 0x00478221: push 1; 0x00478223: push eax; 0x00478224: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_5be3c4), // 0x00478216: mov ax,[0x5BE3C4]
                                           1U,                                     // 0x00478221: push 1 (argument 2)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x004782D0U))); // 0x0047821C: push handler

    // 0x00478229: mov cx,[0x5BE3C4]; 0x00478230: push 0x004784F0; 0x00478235: push 2; 0x00478237: push ecx; 0x00478238: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_5be3c4), // 0x00478229: mov cx,[0x5BE3C4]
                                           2U,                                     // 0x00478235: push 2 (argument 2)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x004784F0U))); // 0x00478230: push handler

    // 0x0047823D: mov dx,[0x5BE3C4]; 0x00478244: push 0x00478510; 0x00478249: push 4; 0x0047824B: push edx; 0x0047824C: call 0x401400;
    // 0x00478251: add esp,0x44 - 17 pushes above.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be3c4), // 0x0047823D: mov dx,[0x5BE3C4]
                                4U,                                     // 0x00478249: push 4 (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00478510U))); // 0x00478244: push handler

    // 0x00478254: push 0x0045E3D0; 0x00478259: call 0x416830; 0x0047825E: push eax; 0x0047825F: mov ax,[0x5BE3C4]; 0x00478265: push eax;
    // 0x00478266: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be3c4), // 0x0047825F: mov ax,[0x5BE3C4]
                                static_cast<std::uint32_t>(Sys_GetSettingByte_416830()), // 0x0047825E: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x0045E3D0U))); // 0x00478254: push handler (below 0x00460000)

    // ---- The script-name tail ----------------------------------------------------------------------------------------------------------
    // 0x0047826B: xor ecx,ecx; 0x0047826D: mov cx,[0x5BE3C6] (an explicit zero-extension of the Gun id); 0x00478274: push ecx;
    // 0x00478275: push "TASKTYPE_GUN"; 0x0047827A: call 0x4B8810; 0x0047827F: add esp,0x1c4 - the 0x1b0 frame plus the five outstanding pushes
    // (the link above plus these two); 0x00478285: ret.
    Script_QHashNodeRegister(const_cast<char*>("TASKTYPE_GUN"), // 0x00478275: push 0x00540800 (argument 1, the last push is the first argument)
                             reinterpret_cast<void*>(static_cast<std::uintptr_t>(
                                 static_cast<std::uint16_t>(::g_5be3c6)))); // 0x0047826D: the zero-extended Gun id (argument 2)
} // end of Task_DefineGun

} // extern "C"
