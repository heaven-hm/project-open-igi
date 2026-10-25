// Native: Task_DefineConditionalContainer (behavior-derived placeholder name; retail symbol unknown); retail VA 0x004872C0-0x0048749B.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 476 bytes, 1 block, 132 instructions.
// Tools/evidence: radare2 6.2.0 af/afbj/pdf with bin.relocs.apply over the exact image (1 block / 132 instructions / 476 bytes / 0 edges; `pd`
//   stops at the ret at 0x0048749B) plus the objdump listing build/retail-dis.txt for the same extent, read instruction by instruction across the
//   whole range (0x004872C0-0x004873AD, 0x004873AD-0x00487447 and 0x00487449-0x0048749B were each dumped separately so no part of the body is
//   transcribed from a summary); `p8`/`ps` at 0x0054107C for the class name (bytes 436f6e646974696f6e616c436f6e7461696e657200 =
//   "ConditionalContainer", byte-identical to the literal below); `pxw` at 0x005C8A80 and 0x005C8A7C for the image values of the two globals; a
//   full stack-pointer simulation over the 132 instructions, which returns esp to its entry value at the ret and confirms every `add esp`
//   (0x38 = 14 pushes, 0xc = 3 seven times, 0x18 = 6 once for the 0x00467D90 plus event-3 pair, 0x10 = 4 for the 0x004CEA10 group plus its
//   `push 3`, 0x18 twice for the first two appends, 0x12c = the 0x114 frame plus the last append's 6 pushes). All three 0x004F1A80 calls were
//   checked against 0x004F1A80's own bytes: it reads its second and third arguments at [esp+8]/[esp+0xC], subtracts them at 0x004F1A88 and never
//   dereferences either, so the values that reach the container are the differences 0x20, 0x70 and 0xC0. Each event-id callee was re-read at its
//   own VA (0x00505D40, 0x004C56F0, 0x004F1A60, 0x004F1A70, 0x00467D90, 0x004F1210, 0x004F1220, 0x004C5800, 0x004CEA20, 0x004CEA10): all are
//   argument-less getters ending in a ret, except 0x00467D90, which branches internally to 0x004017C0 when its byte is 0xFF.
// Behavior/CFG: registers the "ConditionalContainer" task type as TaskType_Register("ConditionalContainer", 0x114, Sys_GetGlobal16_505D40(), 0)
//   and stores the returned 16-bit id as a word at 0x005C8A80 (image value 0); links the id to handler 0x004874A0 on event 0 through 0x00401400,
//   then to 0x004874E0 on event 1, 0x00487730 on event 2 and 0x00487860 on event 3 through 0x00401530; then, in order, links the handlers
//   0x004E0890, 0x00487790, 0x004877A0, 0x004877D0, 0x00487920, 0x00487980, 0x004879E0, 0x00487B20 and 0x00487A70 on the event ids from
//   0x004C56F0, 0x004F1A60, 0x004F1A70, 0x00467D90, 0x004F1210, 0x004F1220, 0x004C5800, 0x004CEA20 and 0x004CEA10 (the 0x00401400 calls carry
//   the 0x004C56F0, 0x004F1A60, 0x004F1A70, 0x00467D90, 0x004C5800, 0x004CEA20 and 0x004CEA10 ids, the 0x00401530 calls the 0x004F1210 and
//   0x004F1220 ids); allocates a 3-entry parameter container through 0x004F1950 into 0x005C8A7C and appends three descriptors to it through
//   0x004F1A80, all with the type value from 0x004F0FC0 and the recorded differences 0x20, 0x70 and 0xC0. Straight-line: one basic block, no
//   branch, no loop, 31 call sites over 16 distinct callees. There is no 0x004B8810/0x004B8890 script-name tail in this body.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. Documented shape differences, all value-preserving: (1) every event-id callee is an
//   argument-less getter in retail that ignores the arguments the caller pushes, and the tree's transcriptions take no arguments, so those inert
//   pushes are not reproduced; each such push is simultaneously the next link call's pre-pushed handler slot, which the port keeps as the handler
//   value in the same order; (2) the same applies to the two zeros pushed before 0x00505D40; (3) retail's 0x114-byte frame is not reproduced -
//   only the two addresses whose difference 0x004F1A80 records are, via one addressable dword plus the retail offsets, because that difference is
//   the only thing the callee reads from them. The id slot 0x005C8A80 is declared as the two 16-bit words it holds (image dword 0x00000000), so
//   retail's 16-bit store at 0x004872E7 and its eleven 16-bit re-loads are plain 16-bit accesses with no type punning. All twelve handler
//   addresses (0x004874A0, 0x004874E0, 0x00487730, 0x00487860, 0x00487790, 0x004877A0, 0x004877D0, 0x00487920, 0x00487980, 0x004879E0,
//   0x00487B20, 0x00487A70) and the handler 0x004E0890 are raw code immediates in retail and are not transcribed yet; all except 0x004E0890 are
//   inside 0x0046xxxx-0x0048xxxx. 0x005C8A80 and 0x005C8A7C have no other owner and both have image value 0, so this file defines them.
//   Domain: core/, because it is one of the 0x00402930 startup registrars.
#include "igi/retail/core/task_types.hpp"

#include <cstdint>

extern "C" std::uint16_t Sys_GetGlobal16_505D40(void);                  // 0x00505D40: argument-less word getter (sys/native_505d40.cpp)
extern "C" std::uint8_t Script_PhaseIdGetE4_4F1A60(void);               // 0x004F1A60: argument-less byte getter
extern "C" std::uint8_t Script_PhaseIdGetE5_4F1A70(void);               // 0x004F1A70: argument-less byte getter
extern "C" std::uint32_t Sys_GetGlobal32_4F1210(void);                  // 0x004F1210: argument-less dword getter
extern "C" std::uint32_t Sys_GetGlobal32_4F1220(void);                  // 0x004F1220: argument-less dword getter
extern "C" std::uint32_t Sys_GetGlobal32_4F0FC0(void);                  // 0x004F0FC0: argument-less dword getter
extern "C" std::uint8_t QTask_PhaseIdGet548176_4CEA10(void);            // 0x004CEA10: argument-less byte getter (script/native_4cea10.cpp)
extern "C" std::uint8_t QTask_PhaseIdGet548177_4CEA20(void);            // 0x004CEA20: argument-less byte getter (script/native_4cea20.cpp)
extern "C" std::uint8_t DynObj_GetByte_4C56F0(void);                    // 0x004C56F0: argument-less byte getter (dynobj/native_4c56f0.cpp)
extern "C" std::uint8_t DynObj_GetByte_4C5800(void);                    // 0x004C5800: argument-less byte getter (dynobj/native_4c5800.cpp)
extern "C" std::uint8_t Task_GetCachedEventId_467D90(void);             // 0x00467D90: argument-less byte getter (core/native_00467d90.cpp)
extern "C" void* Script_TaskContainer_Alloc(std::int32_t const count);  // 0x004F1950
extern "C" void* Script_TaskContainer_Append(std::int32_t* const container, std::int32_t const arg2,
                                             std::int32_t const arg3, std::int32_t const arg4,
                                             std::int32_t const arg5);  // 0x004F1A80 (copied from script/native_4f1a80.cpp)

extern "C" {

// Retail 0x005C8A80 is a 4-byte slot whose image dword is 0x00000000, and retail stores and re-loads only its low word (0x004872E7
// `mov word [0x5C8A80],ax`, then `mov ax/cx/dx,[0x5C8A80]` at 0x004872F2/0x00487305/0x00487326/0x00487342/0x0048735B/0x00487376/0x00487384/
// 0x004873A4/0x004873BF/0x004873DB/0x004873F5/0x0048740F). It is declared as the two 16-bit words the slot actually holds, so the byte layout is
// identical and every store and load is a plain 16-bit access.
std::uint16_t g_5c8a80[2] = {0x0000U, 0x0000U}; // retail 0x005C8A80 (low word: the "ConditionalContainer" task type id) and 0x005C8A82 (high word)
void* g_5c8a7c = nullptr;                       // retail 0x005C8A7C: the parameter container from 0x004F1950 (image value 0)

void Task_DefineConditionalContainer()
{
    // 0x004872C0: sub esp,0x114 - retail's 276-byte frame. The three 0x004F1A80 calls below pass two addresses inside it: its first dword (each
    // `lea ecx,[esp+0xc]` runs with 12 bytes of arguments already pushed, so the address is frame+0x00) and one later slot at retail offsets
    // 0x20, 0x70 and 0xC0. 0x004F1A80 reads those two arguments at [esp+8]/[esp+0xC], subtracts them at 0x004F1A88 and never dereferences them,
    // so the port keeps one addressable dword and passes that address plus the same retail offset.
    std::int32_t append_frame_base = 0; // retail frame slot at +0x00
    const std::int32_t append_frame = reinterpret_cast<std::int32_t>(&append_frame_base);

    // 0x004872C6/0x004872C8: push 0; push 0 (inert arguments of the getter, and TaskType_Register's fourth/fifth slots);
    // 0x004872CA: call 0x505D40; 0x004872CF: push eax; 0x004872D0: push 0x114; 0x004872D5: push "ConditionalContainer";
    // 0x004872DA: call 0x401900.
    const std::uint16_t class_id = TaskType_Register("ConditionalContainer", // 0x004872D5: push 0x0054107C
                                                     0x114U,                  // 0x004872D0: push 0x114 (object size)
                                                     Sys_GetGlobal16_505D40(), // 0x004872CA: call 0x505D40 (parent type)
                                                     0U);                    // 0x004872C8: push 0 (task list count)

    // 0x004872DF: push 0x004874A0 (pre-pushed third argument); 0x004872E4: push 0; 0x004872E6: push eax;
    // 0x004872E7: mov word [0x5C8A80],ax; 0x004872ED: call 0x401400.
    ::g_5c8a80[0] = class_id; // 0x004872E7: the id is stored between the argument pushes and the call, exactly as retail orders it
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5c8a80[0]), // 0x004872F2: mov ax,[0x5C8A80] (retail re-loads the word)
                                0U,                                     // 0x004872E4: push 0 (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x004874A0U))); // 0x004872DF: push handler

    // 0x004872F8: push 0x004874E0 (pre-pushed third argument); 0x004872FD: push 1; 0x004872FF: push eax; 0x00487300: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_5c8a80[0]), // 0x004872F2: mov ax,[0x5C8A80]
                                           1U,                                     // 0x004872FD: push 1
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x004874E0U))); // 0x004872F8: push handler

    // 0x0048730C: push 0x00487730 (pre-pushed third argument); 0x00487311: push 2; 0x00487313: push ecx; 0x00487314: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_5c8a80[0]), // 0x00487305: mov cx,[0x5C8A80]
                                           2U,                                     // 0x00487311: push 2
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x00487730U))); // 0x0048730C: push handler
    // 0x00487319: add esp,0x38 - 14 pushes above (2 inert zeros + 3 registration arguments + three 3-argument links).

    // 0x0048731C: push 0x004E0890 (pre-pushed third argument, and the getter's inert argument); 0x00487321: call 0x4C56F0 (event id);
    // 0x00487326: mov dx,[0x5C8A80]; 0x0048732D: push eax; 0x0048732E: push edx; 0x0048732F: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5c8a80[0]), // 0x00487326: mov dx,[0x5C8A80]
                                static_cast<std::uint32_t>(DynObj_GetByte_4C56F0()), // 0x0048732D: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x004E0890U))); // 0x0048731C: push handler (argument 3)
    // 0x00487334: add esp,0xc - 3 pushes above.

    // 0x00487337: push 0x00487790 (pre-pushed third argument, and the getter's inert argument); 0x0048733C: call 0x4F1A60 (event id);
    // 0x00487341: push eax; 0x00487342: mov ax,[0x5C8A80]; 0x00487348: push eax; 0x00487349: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5c8a80[0]), // 0x00487342: mov ax,[0x5C8A80]
                                static_cast<std::uint32_t>(Script_PhaseIdGetE4_4F1A60()), // 0x00487341: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00487790U))); // 0x00487337: push handler (argument 3)
    // 0x0048734E: add esp,0xc - 3 pushes above.

    // 0x00487351: push 0x004877A0 (pre-pushed third argument, and the getter's inert argument); 0x00487356: call 0x4F1A70 (event id);
    // 0x0048735B: mov cx,[0x5C8A80]; 0x00487362: push eax; 0x00487363: push ecx; 0x00487364: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5c8a80[0]), // 0x0048735B: mov cx,[0x5C8A80]
                                static_cast<std::uint32_t>(Script_PhaseIdGetE5_4F1A70()), // 0x00487362: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x004877A0U))); // 0x00487351: push handler (argument 3)
    // 0x00487369: add esp,0xc - 3 pushes above.

    // 0x0048736C: push 0x004877D0 (pre-pushed third argument, and the getter's inert argument); 0x00487371: call 0x467D90 (event id);
    // 0x00487376: mov dx,[0x5C8A80]; 0x0048737D: push eax; 0x0048737E: push edx; 0x0048737F: call 0x401400 (no cleanup of its own).
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5c8a80[0]), // 0x00487376: mov dx,[0x5C8A80]
                                static_cast<std::uint32_t>(Task_GetCachedEventId_467D90()), // 0x0048737D: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x004877D0U))); // 0x0048736C: push handler (argument 3)

    // 0x0048738A: push 0x00487860 (pre-pushed third argument); 0x0048738F: push 3; 0x00487391: push eax; 0x00487392: call 0x401530;
    // 0x00487397: add esp,0x18 (this link's 3 pushes plus the previous link's 3).
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_5c8a80[0]), // 0x00487384: mov ax,[0x5C8A80]
                                           3U,                                     // 0x0048738F: push 3
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x00487860U))); // 0x0048738A: push handler

    // 0x0048739A: push 0x00487920 (pre-pushed third argument, and the getter's inert argument); 0x0048739F: call 0x4F1210 (event id);
    // 0x004873A4: mov cx,[0x5C8A80]; 0x004873AB: push eax; 0x004873AC: push ecx; 0x004873AD: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_5c8a80[0]), // 0x004873A4: mov cx,[0x5C8A80]
                                           static_cast<std::uint32_t>(Sys_GetGlobal32_4F1210()), // 0x004873AB: push eax
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x00487920U))); // 0x0048739A: push handler
    // 0x004873B2: add esp,0xc - 3 pushes above.

    // 0x004873B5: push 0x00487980 (pre-pushed third argument, and the getter's inert argument); 0x004873BA: call 0x4F1220 (event id);
    // 0x004873BF: mov dx,[0x5C8A80]; 0x004873C6: push eax; 0x004873C7: push edx; 0x004873C8: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_5c8a80[0]), // 0x004873BF: mov dx,[0x5C8A80]
                                           static_cast<std::uint32_t>(Sys_GetGlobal32_4F1220()), // 0x004873C6: push eax
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x00487980U))); // 0x004873B5: push handler
    // 0x004873CD: add esp,0xc - 3 pushes above.

    // 0x004873D0: push 0x004879E0 (pre-pushed third argument, and the getter's inert argument); 0x004873D5: call 0x4C5800 (event id);
    // 0x004873DA: push eax; 0x004873DB: mov ax,[0x5C8A80]; 0x004873E1: push eax; 0x004873E2: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5c8a80[0]), // 0x004873DB: mov ax,[0x5C8A80]
                                static_cast<std::uint32_t>(DynObj_GetByte_4C5800()), // 0x004873DA: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x004879E0U))); // 0x004873D0: push handler (argument 3)
    // 0x004873E7: add esp,0xc - 3 pushes above.

    // 0x004873EA: push 0x00487B20 (pre-pushed third argument, and the getter's inert argument); 0x004873EF: call 0x4CEA20 (event id);
    // 0x004873F4: push eax; 0x004873F5: mov cx,[0x5C8A80]; 0x004873FC: push ecx; 0x004873FD: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5c8a80[0]), // 0x004873F5: mov cx,[0x5C8A80]
                                static_cast<std::uint32_t>(QTask_PhaseIdGet548177_4CEA20()), // 0x004873F4: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00487B20U))); // 0x004873EA: push handler (argument 3)
    // 0x00487402: add esp,0xc - 3 pushes above.

    // 0x00487405: push 0x00487A70 (pre-pushed third argument, and the getter's inert argument); 0x0048740A: call 0x4CEA10 (event id);
    // 0x0048740F: mov dx,[0x5C8A80]; 0x00487416: push eax; 0x00487417: push edx; 0x00487418: call 0x401400 (no cleanup of its own).
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5c8a80[0]), // 0x0048740F: mov dx,[0x5C8A80]
                                static_cast<std::uint32_t>(QTask_PhaseIdGet548176_4CEA10()), // 0x00487416: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00487A70U))); // 0x00487405: push handler (argument 3)

    // 0x0048741D: push 3; 0x0048741F: call 0x4F1950; 0x00487424: add esp,0x10 (4 pushes: the link's 3 plus this 1);
    // 0x00487427: mov [0x5C8A7C],eax.
    ::g_5c8a7c = Script_TaskContainer_Alloc(3); // 0x0048741D: push 3 (the container's entry count; three descriptors follow)

    // 0x0048742C/0x0048742E: push 0; push 7 (inert arguments); 0x00487430: call 0x4F0FC0; 0x00487435: mov edx,[0x5C8A7C];
    // 0x0048743B: push eax; 0x0048743C: lea eax,[esp+0x2c] (frame+0x20); 0x00487440: lea ecx,[esp+0xc] (frame+0x00);
    // 0x00487444/0x00487445/0x00487446: the three argument pushes; 0x00487447: call 0x4F1A80; 0x0048744C: add esp,0x18 (6 pushes).
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(::g_5c8a7c),  // argument 1: the container
                                append_frame,                                 // argument 2: retail frame+0x00
                                append_frame + 0x20,                          // argument 3: retail frame+0x20 (recorded difference 0x20)
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FC0()), // argument 4: the type from 0x4F0FC0
                                7);                                           // argument 5: retail's leftover 7

    // 0x0048744F/0x00487451: push 0; push 7 (inert); 0x00487453: call 0x4F0FC0; 0x00487458: mov edx,[0x5C8A7C];
    // 0x0048745E: push eax; 0x0048745F: lea eax,[esp+0x7c] (frame+0x70); 0x00487463: lea ecx,[esp+0xc] (frame+0x00);
    // 0x00487467/0x00487468/0x00487469: the three argument pushes; 0x0048746A: call 0x4F1A80; 0x0048746F: add esp,0x18 (6 pushes).
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(::g_5c8a7c),
                                append_frame,
                                append_frame + 0x70,                          // 0x0048745F: frame+0x70 (recorded difference 0x70)
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FC0()),
                                7);

    // 0x00487472/0x00487474: push 0; push 7 (inert); 0x00487476: call 0x4F0FC0; 0x0048747B: mov edx,[0x5C8A7C];
    // 0x00487481: push eax; 0x00487482: lea eax,[esp+0xcc] (frame+0xC0); 0x00487489: lea ecx,[esp+0xc] (frame+0x00);
    // 0x0048748D/0x0048748E/0x0048748F: the three argument pushes; 0x00487490: call 0x4F1A80 (no cleanup of its own);
    // 0x00487495: add esp,0x12c - the 0x114 frame plus this group's 6 pushes; 0x0048749B: ret.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(::g_5c8a7c),
                                append_frame,
                                append_frame + 0xC0,                          // 0x00487482: frame+0xC0 (recorded difference 0xC0)
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FC0()),
                                7);
} // end of Task_DefineConditionalContainer

} // extern "C"
