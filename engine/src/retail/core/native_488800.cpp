// Native: Task_DefineEditVariable (behavior-derived placeholder name; retail symbol unknown); retail VA 0x00488800-0x00488977.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 376 bytes, 1 block, 109 instructions.
// Tools/evidence: radare2 6.2.0 af/afbj/pdf with bin.relocs.apply over the exact image (1 block / 109 instructions / 376 bytes / 0 edges; `pd`
//   stops at the ret at 0x00488977) plus the objdump listing build/retail-dis.txt for the same extent; `ps` at 0x005410D4 for the class name;
//   `pxw` at 0x005410D0 and 0x005C8A9C for the image values of the two globals; a full stack-pointer simulation over the 109 instructions, which
//   returns esp to its entry value at the ret and confirms every `add esp` (0x44 = 17 pushes, 0xc = 3 each, 0x10 = 4, 0x18 = 6 each, 0xf8 = the
//   0xe0 frame plus the final 6 pushes). The four 0x004F1A80 calls were checked against 0x004F1A80's own bytes: it reads its second and third
//   arguments at [esp+8]/[esp+0xC], subtracts them at 0x004F1A88 and never dereferences either, so the values that reach the container are the
//   differences 0x20, 0xD8, 0x38, 0x88.
// Behavior/CFG: registers the "EditVariable" task type as TaskType_Register("EditVariable", 0xE0, Sys_GetGlobal16_5034D0(), 0) and stores the
//   returned 16-bit id as a word at 0x005410D0 (image value 0x0181); links the id to handler 0x00488980 on event 0 through 0x00401400, then to
//   0x004889B0 on event 1, 0x00488A30 on event 2 and 0x004D3710 on event 3 through 0x00401530, then to 0x00488A90 on the event id from
//   0x004F1A60 and 0x00488AA0 on the event id from 0x004F1A70 through 0x00401400, then to 0x00488AD0 on the event id from 0x004F1210 and
//   0x00488B30 on the event id from 0x004F1220 through 0x00401530; allocates a 4-entry parameter container through 0x004F1950 into 0x005C8A9C and
//   appends four descriptors to it through 0x004F1A80, with the type values coming from 0x004F1010, 0x004F0FA0, 0x004F0FC0 and 0x004F0FC0 again.
//   Straight-line: one basic block, no branch, no loop, 23 call sites over 13 distinct callees. Unlike the sibling registrars there is no
//   0x004B8810/0x004B8890 script-name tail in this body, and no model name is loaded.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. Documented shape differences, all value-preserving: (1) the 0x004F0FA0/0x004F0FC0/0x004F1010/
//   0x004F1210/0x004F1220 callees are single-instruction getters in retail that ignore the arguments the caller pushes (verified: each body is
//   `mov eax,[global]; ret`), and the tree's transcriptions take no arguments, so those inert pushes are not reproduced and only the call order
//   and values are kept; (2) the same applies to the two zeros pushed before 0x005034D0; (3) retail's 0xe0-byte frame is not reproduced - only the
//   two addresses whose difference 0x004F1A80 records are, via one addressable dword plus the retail offsets, because that difference is the only
//   thing the callee reads from them; (4) the id slot 0x005410D0 is declared as the two 16-bit words it really holds (image dword 0x00000181), so
//   retail's 16-bit store at 0x00488827 and its seven 16-bit re-loads are plain 16-bit accesses with no type punning and identical bytes.
//   0x005410D0 is the fourth image slot found holding 0x00000181 (with 0x00540B08, 0x0053C46C and 0x0053F520), which supports the Lead's pending
//   note that 0x0181 is a retail task-type constant rather than a per-native literal. The seven handler addresses 0x00488980, 0x004889B0,
//   0x00488A30, 0x00488A90, 0x00488AA0, 0x00488AD0 and 0x00488B30 are raw code immediates in retail and are not transcribed yet (native gaps, all
//   inside 0x0046xxxx-0x0048xxxx); the event-3 handler 0x004D3710 is already transcribed in script/native_4D3710.cpp. 0x005410D0 and 0x005C8A9C
//   have no other owner; 0x005410D0's image value is the dword 0x00000181 and 0x005C8A9C's is 0, so this file defines them with those values.
//   Domain: core/, because it is one of the 0x00402930 startup registrars.
#include "igi/retail/core/task_types.hpp"

#include <cstdint>

extern "C" std::uint16_t Sys_GetGlobal16_5034D0(void);                  // 0x005034D0: argument-less word getter
extern "C" std::uint8_t Script_PhaseIdGetE4_4F1A60(void);               // 0x004F1A60: argument-less byte getter
extern "C" std::uint8_t Script_PhaseIdGetE5_4F1A70(void);               // 0x004F1A70: argument-less byte getter
extern "C" std::uint32_t Sys_GetGlobal32_4F1210(void);                  // 0x004F1210: argument-less dword getter
extern "C" std::uint32_t Sys_GetGlobal32_4F1220(void);                  // 0x004F1220: argument-less dword getter
extern "C" std::uint32_t Sys_GetGlobal32_4F1010(void);                  // 0x004F1010: argument-less dword getter
extern "C" std::uint32_t Sys_GetGlobal32_4F0FC0(void);                  // 0x004F0FC0: argument-less dword getter
extern "C" std::uint32_t Core_GetRenderWordA_4F0FA0(void);              // 0x004F0FA0: argument-less dword getter
extern "C" void* Script_TaskContainer_Alloc(std::int32_t const count);  // 0x004F1950
extern "C" void* Script_TaskContainer_Append(std::int32_t* const container, std::int32_t const arg2,
                                             std::int32_t const arg3, std::int32_t const arg4,
                                             std::int32_t const arg5);  // 0x004F1A80 (copied from script/native_4f1a80.cpp)

extern "C" {

// Retail 0x005410D0 is a 4-byte aligned slot whose image dword is 0x00000181, and retail stores and re-loads only its low word (0x00488827
// `mov word [0x5410D0],ax`, then `mov ax/cx/dx,[0x5410D0]` at 0x00488832/0x00488845/0x00488859/0x0048887B/0x00488894/0x004888AF/0x004888CB).
// It is declared as the two 16-bit words the slot actually holds, so the byte layout is identical and every store and load is a plain 16-bit access.
std::uint16_t g_5410d0[2] = {0x0181U, 0x0000U}; // retail 0x005410D0 (low word, image 0x0181) and 0x005410D2 (high word, zero in the image)
void* g_5c8a9c = nullptr;         // retail 0x005C8A9C: the parameter container from 0x004F1950 (image value 0)

void Task_DefineEditVariable()
{
    // 0x00488800: sub esp,0xe0 - retail's 224-byte frame. The four 0x004F1A80 calls below pass two addresses inside it: its first dword (each
    // `lea edx,[esp+0xc]` runs with 12 bytes of arguments already pushed, so the address is frame+0x00) and one later slot at retail offsets
    // 0x20, 0xD8, 0x38 and 0x88. 0x004F1A80 reads those two arguments at [esp+8]/[esp+0xC], subtracts them at 0x004F1A88 and never dereferences
    // them, so the port keeps one addressable dword and passes that address plus the same retail offset.
    std::int32_t append_frame_base = 0; // retail frame slot at +0x00
    const std::int32_t append_frame = reinterpret_cast<std::int32_t>(&append_frame_base);

    // 0x00488806/0x00488808: push 0; push 0 (inert arguments of the getter, and TaskType_Register's fourth/fifth slots);
    // 0x0048880A: call 0x5034D0; 0x0048880F: push eax; 0x00488810: push 0xE0; 0x00488815: push "EditVariable"; 0x0048881A: call 0x401900.
    const std::uint16_t class_id = TaskType_Register("EditVariable", // 0x00488815: push 0x005410D4
                                                     0xE0U,          // 0x00488810: push 0xE0 (object size)
                                                     Sys_GetGlobal16_5034D0(), // 0x0048880A: call 0x5034D0 (parent type)
                                                     0U);           // 0x00488808: push 0 (task list count)

    // 0x0048881F: push 0x00488980 (pre-pushed third argument); 0x00488824: push 0; 0x00488826: push eax;
    // 0x00488827: mov word [0x5410D0],ax; 0x0048882D: call 0x401400.
    ::g_5410d0[0] = class_id; // 0x00488827: mov word [0x5410D0],ax - a plain 16-bit store into the low word of the slot. Retail stores the id
                              // between the argument pushes and the call, exactly as this statement is ordered here.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5410d0[0]), // 0x00488832: mov ax,[0x5410D0] (retail re-loads the word)
                                0U,                                     // 0x00488824: push 0 (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00488980U))); // 0x0048881F: push handler

    // 0x00488838: push 0x004889B0 (pre-pushed third argument); 0x0048883D: push 1; 0x0048883F: push eax; 0x00488840: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_5410d0[0]), // 0x00488832: mov ax,[0x5410D0]
                                           1U,                                     // 0x0048883D: push 1
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x004889B0U))); // 0x00488838: push handler

    // 0x0048884C: push 0x00488A30 (pre-pushed third argument); 0x00488851: push 2; 0x00488853: push ecx; 0x00488854: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_5410d0[0]), // 0x00488845: mov cx,[0x5410D0]
                                           2U,                                     // 0x00488851: push 2
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x00488A30U))); // 0x0048884C: push handler

    // 0x00488860: push 0x004D3710 (pre-pushed third argument); 0x00488865: push 3; 0x00488867: push edx; 0x00488868: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_5410d0[0]), // 0x00488859: mov dx,[0x5410D0]
                                           3U,                                     // 0x00488865: push 3
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x004D3710U))); // 0x00488860: push handler
    // 0x0048886D: add esp,0x44 - 17 pushes above (2 inert zeros + 3 registration arguments + three 3-argument links + this link's 3).

    // 0x00488870: push 0x00488A90 (pre-pushed third argument); 0x00488875: call 0x4F1A60 (event id);
    // 0x0048887A: push eax; 0x0048887B: mov ax,[0x5410D0]; 0x00488881: push eax; 0x00488882: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5410d0[0]),      // 0x0048887B: mov ax,[0x5410D0]
                                static_cast<std::uint32_t>(Script_PhaseIdGetE4_4F1A60()), // 0x0048887A: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00488A90U))); // 0x00488870: push handler (argument 3)
    // 0x00488887: add esp,0xc - 3 pushes above.

    // 0x0048888A: push 0x00488AA0 (pre-pushed third argument); 0x0048888F: call 0x4F1A70 (event id);
    // 0x00488894: mov cx,[0x5410D0]; 0x0048889B: push eax; 0x0048889C: push ecx; 0x0048889D: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5410d0[0]),      // 0x00488894: mov cx,[0x5410D0]
                                static_cast<std::uint32_t>(Script_PhaseIdGetE5_4F1A70()), // 0x0048889B: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00488AA0U))); // 0x0048888A: push handler (argument 3)
    // 0x004888A2: add esp,0xc - 3 pushes above.

    // 0x004888A5: push 0x00488AD0 (pre-pushed third argument); 0x004888AA: call 0x4F1210 (event id);
    // 0x004888AF: mov dx,[0x5410D0]; 0x004888B6: push eax; 0x004888B7: push edx; 0x004888B8: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_5410d0[0]), // 0x004888AF: mov dx,[0x5410D0]
                                           static_cast<std::uint32_t>(Sys_GetGlobal32_4F1210()), // 0x004888B6: push eax
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x00488AD0U))); // 0x004888A5: push handler
    // 0x004888BD: add esp,0xc - 3 pushes above.

    // 0x004888C0: push 0x00488B30 (pre-pushed third argument); 0x004888C5: call 0x4F1220 (event id);
    // 0x004888CA: push eax; 0x004888CB: mov ax,[0x5410D0]; 0x004888D1: push eax; 0x004888D2: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_5410d0[0]), // 0x004888CB: mov ax,[0x5410D0]
                                           static_cast<std::uint32_t>(Sys_GetGlobal32_4F1220()), // 0x004888CA: push eax
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x00488B30U))); // 0x004888C0: push handler

    // 0x004888D7: push 4; 0x004888D9: call 0x4F1950; 0x004888DE: add esp,0x10 (4 pushes); 0x004888E1: mov [0x5C8A9C],eax.
    ::g_5c8a9c = Script_TaskContainer_Alloc(4); // 0x004888D7: push 4 (the container's entry count; four descriptors follow)

    // 0x004888E6/0x004888E8: push 0; push 7 (inert arguments); 0x004888EA: call 0x4F1010; 0x004888EF: lea ecx,[esp+0x28] (frame+0x20);
    // 0x004888F3: push eax; 0x004888F9: lea edx,[esp+0xc] (frame+0x00); 0x004888FD/0x004888FE/0x004888FF: the three argument pushes;
    // 0x00488900: call 0x4F1A80; 0x00488905: add esp,0x18 (6 pushes).
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(::g_5c8a9c),  // argument 1: the container
                                append_frame,                                 // argument 2: retail frame+0x00
                                append_frame + 0x20,                          // argument 3: retail frame+0x20 (recorded difference 0x20)
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F1010()), // argument 4: the type from 0x4F1010
                                7);                                           // argument 5: retail's leftover 7

    // 0x00488908/0x0048890A: push 0; push 7 (inert); 0x0048890C: call 0x4F0FA0; 0x00488911: lea ecx,[esp+0xe0] (frame+0xD8);
    // 0x00488918: push eax; 0x0048891E: lea edx,[esp+0xc]; 0x00488925: call 0x4F1A80; 0x0048892A: add esp,0x18.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(::g_5c8a9c),
                                append_frame,
                                append_frame + 0xD8,                          // 0x00488911: frame+0xD8 (recorded difference 0xD8)
                                static_cast<std::int32_t>(Core_GetRenderWordA_4F0FA0()),
                                7);

    // 0x0048892D/0x0048892F: push 0; push 7 (inert); 0x00488931: call 0x4F0FC0; 0x00488936: lea ecx,[esp+0x40] (frame+0x38);
    // 0x0048893A: push eax; 0x00488940: lea edx,[esp+0xc]; 0x00488947: call 0x4F1A80; 0x0048894C: add esp,0x18.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(::g_5c8a9c),
                                append_frame,
                                append_frame + 0x38,                          // 0x00488936: frame+0x38 (recorded difference 0x38)
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FC0()),
                                7);

    // 0x0048894F/0x00488951: push 0; push 7 (inert); 0x00488953: call 0x4F0FC0; 0x00488958: lea ecx,[esp+0x90] (frame+0x88);
    // 0x0048895F: push eax; 0x00488965: lea edx,[esp+0xc]; 0x0048896C: call 0x4F1A80 (no cleanup of its own);
    // 0x00488971: add esp,0xf8 - the 0xe0 frame plus this group's 6 pushes; 0x00488977: ret.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(::g_5c8a9c),
                                append_frame,
                                append_frame + 0x88,                          // 0x00488958: frame+0x88 (recorded difference 0x88)
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FC0()), // second use of the same getter, exactly as retail
                                7);
} // end of Task_DefineEditVariable

} // extern "C"
