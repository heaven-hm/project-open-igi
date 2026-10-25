// Native: Task_DefineGuardGenerator (behavior-derived placeholder name; retail symbol unknown); retail VA 0x00487BE0-0x00487D84.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 421 bytes, 1 block, 115 instructions.
// Tools/evidence: radare2 6.2.0 af/afbj/pdf with bin.relocs.apply over the exact image (1 block / 115 instructions / 421 bytes / 0 edges; `pd`
//   stops at the ret at 0x00487D84) plus the objdump listing build/retail-dis.txt for the same extent; `p8`/`ps` at 0x00541098 for the class name
//   (bytes 477561726447656e657261746f7200 = "GuardGenerator", byte-identical to the literal below); `pxw` at 0x00541094 and 0x005C8A88 for the
//   image values of the two globals; a full stack-pointer simulation over the 115 instructions, which returns esp to its entry value at the ret
//   and confirms every `add esp` (0x38 = 14 pushes, 0xc = 3 nine times, 0x10 = 4, 0x18 = 6, 0x98 = the 0x80 frame plus the last 6 pushes).
//   Both 0x004F1A80 calls were checked against 0x004F1A80's own bytes: it reads its second and third arguments at [esp+8]/[esp+0xC], subtracts
//   them at 0x004F1A88 and never dereferences either, so the values that reach the container are the differences 0x20 and 0x78. The nine event-id
//   callees were each re-read at their own VAs: 0x004CEA20, 0x004CEA10, 0x004C56F0, 0x004C5800 and 0x00467D90 are argument-less getters
//   (`mov al,[abs]` with a ret; 0x00467D90 branches internally to 0x004017C0 when the byte is 0xFF), and 0x004F1A60/0x004F1A70/0x004F1210/
//   0x004F1220 are argument-less getters too.
// Behavior/CFG: registers the "GuardGenerator" task type as TaskType_Register("GuardGenerator", 0x80, QTask_GetCurrentType(), 0) and stores the
//   returned 16-bit id as a word at 0x00541094 (image dword 0x00000181); links the id to handler 0x00487EB0 on event 0 through 0x00401400, then to
//   0x00487EE0 on event 1 and 0x004880F0 on event 2 through 0x00401530; then, in order, links the handlers 0x004881F0, 0x00488200, 0x00488430 and
//   0x004884D0 on the event ids from 0x004F1A60, 0x004F1A70, 0x004F1210 and 0x004F1220 (first two through 0x00401400, last two through
//   0x00401530), and the handlers 0x00487E20, 0x00487D90, 0x004E0890, 0x00488560 and 0x004879E0 on the event ids from 0x004CEA20, 0x004CEA10,
//   0x004C56F0, 0x00467D90 and 0x004C5800 (all through 0x00401400); allocates a 2-entry parameter container through 0x004F1950 into 0x005C8A88 and
//   appends two descriptors to it through 0x004F1A80, with the type values from 0x004F0FC0 and 0x004F0FA0. Straight-line: one basic block, no
//   branch, no loop, 28 call sites over 17 distinct callees. There is no 0x004B8810/0x004B8890 script-name tail in this body.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. Documented shape differences, all value-preserving: (1) every event-id callee is an
//   argument-less getter in retail that ignores the arguments the caller pushes, and the tree's transcriptions take no arguments, so those inert
//   pushes are not reproduced; each such push is simultaneously the next link call's pre-pushed handler slot, which the port keeps as the handler
//   value in the same order; (2) the same applies to the two zeros pushed before 0x004018F0; (3) retail's 0x80-byte frame is not reproduced - only
//   the two addresses whose difference 0x004F1A80 records are, via one addressable dword plus the retail offsets, because that difference is the
//   only thing the callee reads from them. The id slot 0x00541094 is declared as the two 16-bit words it holds (image dword 0x00000181), so
//   retail's 16-bit store at 0x00487C07 and its nine 16-bit re-loads are plain 16-bit accesses with no type punning. All eleven handler addresses
//   (0x00487EB0, 0x00487EE0, 0x004880F0, 0x004881F0, 0x00488200, 0x00488430, 0x004884D0, 0x00487E20, 0x00487D90, 0x00488560, 0x004879E0) and the
//   handler 0x004E0890 are raw code immediates in retail and are not transcribed yet; all except 0x004E0890 are inside 0x0046xxxx-0x0048xxxx.
//   0x00541094 and 0x005C8A88 have no other owner; 0x00541094's image value is the dword 0x00000181 and 0x005C8A88's is 0, so this file defines
//   them with those values. 0x00541094 is the fifth image slot found holding 0x00000181 (with 0x00540B08, 0x0053C46C, 0x0053F520 and
//   0x005410D0). Domain: core/, because it is one of the 0x00402930 startup registrars.
#include "igi/retail/core/task_types.hpp"

#include <cstdint>

extern "C" std::uint8_t Script_PhaseIdGetE4_4F1A60(void);               // 0x004F1A60: argument-less byte getter
extern "C" std::uint8_t Script_PhaseIdGetE5_4F1A70(void);               // 0x004F1A70: argument-less byte getter
extern "C" std::uint32_t Sys_GetGlobal32_4F1210(void);                  // 0x004F1210: argument-less dword getter
extern "C" std::uint32_t Sys_GetGlobal32_4F1220(void);                  // 0x004F1220: argument-less dword getter
extern "C" std::uint32_t Sys_GetGlobal32_4F0FC0(void);                  // 0x004F0FC0: argument-less dword getter
extern "C" std::uint32_t Core_GetRenderWordA_4F0FA0(void);              // 0x004F0FA0: argument-less dword getter
extern "C" std::uint8_t QTask_PhaseIdGet548176_4CEA10(void);            // 0x004CEA10: argument-less byte getter (script/native_4cea10.cpp)
extern "C" std::uint8_t QTask_PhaseIdGet548177_4CEA20(void);            // 0x004CEA20: argument-less byte getter (script/native_4cea20.cpp)
extern "C" std::uint8_t DynObj_GetByte_4C56F0(void);                    // 0x004C56F0: argument-less byte getter (dynobj/native_4c56f0.cpp)
extern "C" std::uint8_t DynObj_GetByte_4C5800(void);                    // 0x004C5800: argument-less byte getter (dynobj/native_4c5800.cpp)
extern "C" std::uint8_t Task_GetCachedEventId_467D90(void);             // 0x00467D90: argument-less byte getter (core/native_00467d90.cpp)
extern "C" void* Script_TaskContainer_Alloc(std::int32_t const count);  // 0x004F1950
extern "C" void* Script_TaskContainer_Append(std::int32_t* const container, std::int32_t const arg2,
                                             std::int32_t const arg3, std::int32_t const arg4,
                                             std::int32_t const arg5, std::int32_t const arg6);
// 0x004F1A80 (script/native_4f1a80.cpp). SIX arguments, corrected: the callee reads arg1..arg4, arg5 as a byte (recorded at record +0x0C)
// and arg6 as a dword (recorded at record +0x08). The two reads are both `[esp+0x20]` in retail, at different push depths, with a `pop edi`
// between them (0x004F1AB1 with twelve bytes pushed = arg5, 0x004F1ACA with eight = arg6). Earlier revisions of this file declared five
// parameters and the tree's implementation wrote arg5 into both fields; both are now corrected. Every append below passes the sixth word that
// retail pushes for it.  // 0x004F1A80 (copied from script/native_4f1a80.cpp)

extern "C" {

// Retail 0x00541094 is a 4-byte slot whose image dword is 0x00000181, and retail stores and re-loads only its low word (0x00487C07
// `mov word [0x541094],ax`, then `mov ax/cx/dx,[0x541094]` at 0x00487C12/0x00487C25/0x00487C46/0x00487C62/0x00487C7B/0x00487C96/0x00487CB2/
// 0x00487CCB/0x00487CE6/0x00487D02/0x00487D1B). It is declared as the two 16-bit words the slot actually holds, so the byte layout is identical
// and every store and load is a plain 16-bit access.
std::uint16_t g_541094[2] = {0x0181U, 0x0000U}; // retail 0x00541094 (low word: the "GuardGenerator" task type id) and 0x00541096 (high word)
void* g_5c8a88 = nullptr;                       // retail 0x005C8A88: the parameter container from 0x004F1950 (image value 0)

void Task_DefineGuardGenerator()
{
    // 0x00487BE0: sub esp,0x80 - retail's 128-byte frame. The two 0x004F1A80 calls below pass two addresses inside it: its first dword (each
    // `lea eax,[esp+0xc]` runs with 12 bytes of arguments already pushed, so the address is frame+0x00) and one later slot at retail offsets
    // 0x20 and 0x78. 0x004F1A80 reads those two arguments at [esp+8]/[esp+0xC], subtracts them at 0x004F1A88 and never dereferences them, so the
    // port keeps one addressable dword and passes that address plus the same retail offset.
    std::int32_t append_frame_base = 0; // retail frame slot at +0x00
    const std::int32_t append_frame = reinterpret_cast<std::int32_t>(&append_frame_base);

    // 0x00487BE6/0x00487BE8: push 0; push 0 (inert arguments of the getter, and TaskType_Register's fourth/fifth slots);
    // 0x00487BEA: call 0x4018F0; 0x00487BEF: push eax; 0x00487BF0: push 0x80; 0x00487BF5: push "GuardGenerator"; 0x00487BFA: call 0x401900.
    const std::uint16_t class_id = TaskType_Register("GuardGenerator", // 0x00487BF5: push 0x00541098
                                                     0x80U,             // 0x00487BF0: push 0x80 (object size)
                                                     QTask_GetCurrentType(), // 0x00487BEA: call 0x4018F0 (parent type)
                                                     0U);               // 0x00487BE8: push 0 (task list count)

    // 0x00487BFF: push 0x00487EB0 (pre-pushed third argument); 0x00487C04: push 0; 0x00487C06: push eax;
    // 0x00487C07: mov word [0x541094],ax; 0x00487C0D: call 0x401400.
    ::g_541094[0] = class_id; // 0x00487C07: the id is stored between the argument pushes and the call, exactly as retail orders it
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_541094[0]), // 0x00487C12: mov ax,[0x541094] (retail re-loads the word)
                                0U,                                     // 0x00487C04: push 0 (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00487EB0U))); // 0x00487BFF: push handler

    // 0x00487C18: push 0x00487EE0 (pre-pushed third argument); 0x00487C1D: push 1; 0x00487C1F: push eax; 0x00487C20: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_541094[0]), // 0x00487C12: mov ax,[0x541094]
                                           1U,                                     // 0x00487C1D: push 1
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x00487EE0U))); // 0x00487C18: push handler

    // 0x00487C2C: push 0x004880F0 (pre-pushed third argument); 0x00487C31: push 2; 0x00487C33: push ecx; 0x00487C34: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_541094[0]), // 0x00487C25: mov cx,[0x541094]
                                           2U,                                     // 0x00487C31: push 2
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x004880F0U))); // 0x00487C2C: push handler
    // 0x00487C39: add esp,0x38 - 14 pushes above (2 inert zeros + 3 registration arguments + three 3-argument links).

    // 0x00487C3C: push 0x004881F0 (pre-pushed third argument); 0x00487C41: call 0x4F1A60 (event id);
    // 0x00487C46: mov dx,[0x541094]; 0x00487C4D: push eax; 0x00487C4E: push edx; 0x00487C4F: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_541094[0]), // 0x00487C46: mov dx,[0x541094]
                                static_cast<std::uint32_t>(Script_PhaseIdGetE4_4F1A60()), // 0x00487C4D: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x004881F0U))); // 0x00487C3C: push handler (argument 3)
    // 0x00487C54: add esp,0xc - 3 pushes above.

    // 0x00487C57: push 0x00488200 (pre-pushed third argument); 0x00487C5C: call 0x4F1A70 (event id);
    // 0x00487C62: mov ax,[0x541094]; 0x00487C61: push eax; 0x00487C68: push eax; 0x00487C69: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_541094[0]), // 0x00487C62: mov ax,[0x541094]
                                static_cast<std::uint32_t>(Script_PhaseIdGetE5_4F1A70()), // 0x00487C61: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00488200U))); // 0x00487C57: push handler (argument 3)
    // 0x00487C6E: add esp,0xc - 3 pushes above.

    // 0x00487C71: push 0x00488430 (pre-pushed third argument); 0x00487C76: call 0x4F1210 (event id);
    // 0x00487C7B: mov cx,[0x541094]; 0x00487C82: push eax; 0x00487C83: push ecx; 0x00487C84: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_541094[0]), // 0x00487C7B: mov cx,[0x541094]
                                           static_cast<std::uint32_t>(Sys_GetGlobal32_4F1210()), // 0x00487C82: push eax
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x00488430U))); // 0x00487C71: push handler
    // 0x00487C89: add esp,0xc - 3 pushes above.

    // 0x00487C8C: push 0x004884D0 (pre-pushed third argument); 0x00487C91: call 0x4F1220 (event id);
    // 0x00487C96: mov dx,[0x541094]; 0x00487C9D: push eax; 0x00487C9E: push edx; 0x00487C9F: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_541094[0]), // 0x00487C96: mov dx,[0x541094]
                                           static_cast<std::uint32_t>(Sys_GetGlobal32_4F1220()), // 0x00487C9D: push eax
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x004884D0U))); // 0x00487C8C: push handler
    // 0x00487CA4: add esp,0xc - 3 pushes above.

    // 0x00487CA7: push 0x00487E20 (pre-pushed third argument, and the getter's inert argument); 0x00487CAC: call 0x4CEA20 (event id);
    // 0x00487CB2: mov ax,[0x541094]; 0x00487CB1: push eax; 0x00487CB8: push eax; 0x00487CB9: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_541094[0]), // 0x00487CB2: mov ax,[0x541094]
                                static_cast<std::uint32_t>(QTask_PhaseIdGet548177_4CEA20()), // 0x00487CB1: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00487E20U))); // 0x00487CA7: push handler (argument 3)
    // 0x00487CBE: add esp,0xc - 3 pushes above.

    // 0x00487CC1: push 0x00487D90 (pre-pushed third argument, and the getter's inert argument); 0x00487CC6: call 0x4CEA10 (event id);
    // 0x00487CCB: mov cx,[0x541094]; 0x00487CD2: push eax; 0x00487CD3: push ecx; 0x00487CD4: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_541094[0]), // 0x00487CCB: mov cx,[0x541094]
                                static_cast<std::uint32_t>(QTask_PhaseIdGet548176_4CEA10()), // 0x00487CD2: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00487D90U))); // 0x00487CC1: push handler (argument 3)
    // 0x00487CD9: add esp,0xc - 3 pushes above.

    // 0x00487CDC: push 0x004E0890 (pre-pushed third argument, and the getter's inert argument); 0x00487CE1: call 0x4C56F0 (event id);
    // 0x00487CE6: mov dx,[0x541094]; 0x00487CED: push eax; 0x00487CEE: push edx; 0x00487CEF: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_541094[0]), // 0x00487CE6: mov dx,[0x541094]
                                static_cast<std::uint32_t>(DynObj_GetByte_4C56F0()), // 0x00487CED: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x004E0890U))); // 0x00487CDC: push handler (argument 3)
    // 0x00487CF4: add esp,0xc - 3 pushes above.

    // 0x00487CF7: push 0x00488560 (pre-pushed third argument, and the getter's inert argument); 0x00487CFC: call 0x467D90 (event id);
    // 0x00487D02: mov ax,[0x541094]; 0x00487D01: push eax; 0x00487D08: push eax; 0x00487D09: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_541094[0]), // 0x00487D02: mov ax,[0x541094]
                                static_cast<std::uint32_t>(Task_GetCachedEventId_467D90()), // 0x00487D01: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00488560U))); // 0x00487CF7: push handler (argument 3)
    // 0x00487D0E: add esp,0xc - 3 pushes above.

    // 0x00487D11: push 0x004879E0 (pre-pushed third argument, and the getter's inert argument); 0x00487D16: call 0x4C5800 (event id);
    // 0x00487D1B: mov cx,[0x541094]; 0x00487D22: push eax; 0x00487D23: push ecx; 0x00487D24: call 0x401400 (no cleanup of its own).
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_541094[0]), // 0x00487D1B: mov cx,[0x541094]
                                static_cast<std::uint32_t>(DynObj_GetByte_4C5800()), // 0x00487D22: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x004879E0U))); // 0x00487D11: push handler (argument 3)

    // 0x00487D29: push 2; 0x00487D2B: call 0x4F1950; 0x00487D30: add esp,0x10 (4 pushes: the link's 3 plus this 1);
    // 0x00487D33: mov [0x5C8A88],eax.
    ::g_5c8a88 = Script_TaskContainer_Alloc(2); // 0x00487D29: push 2 (the container's entry count; two descriptors follow)

    // 0x00487D38/0x00487D3A: push 0; push 7 (inert arguments); 0x00487D3C: call 0x4F0FC0; 0x00487D41: mov ecx,[0x5C8A88];
    // 0x00487D47: push eax; 0x00487D48: lea edx,[esp+0x2c] (frame+0x20); 0x00487D4C: lea eax,[esp+0xc] (frame+0x00);
    // 0x00487D50/0x00487D51/0x00487D52: the three argument pushes; 0x00487D53: call 0x4F1A80; 0x00487D58: add esp,0x18 (6 pushes).
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(::g_5c8a88),  // argument 1: the container
                                append_frame,                                 // argument 2: retail frame+0x00
                                append_frame + 0x20,                          // argument 3: retail frame+0x20 (recorded difference 0x20)
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FC0()), // argument 4: the type from 0x4F0FC0
                                7, 0);                                           // argument 5: retail's leftover 7

    // 0x00487D5B/0x00487D5D: push 0; push 7 (inert); 0x00487D5F: call 0x4F0FA0; 0x00487D64: mov ecx,[0x5C8A88];
    // 0x00487D6A: push eax; 0x00487D6B: lea edx,[esp+0x84] (frame+0x78); 0x00487D72: lea eax,[esp+0xc] (frame+0x00);
    // 0x00487D76/0x00487D77/0x00487D78: the three argument pushes; 0x00487D79: call 0x4F1A80 (no cleanup of its own);
    // 0x00487D7E: add esp,0x98 - the 0x80 frame plus this group's 6 pushes; 0x00487D84: ret.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(::g_5c8a88),
                                append_frame,
                                append_frame + 0x78,                          // 0x00487D6B: frame+0x78 (recorded difference 0x78)
                                static_cast<std::int32_t>(Core_GetRenderWordA_4F0FA0()),
                                7, 0);
} // end of Task_DefineGuardGenerator

} // extern "C"
