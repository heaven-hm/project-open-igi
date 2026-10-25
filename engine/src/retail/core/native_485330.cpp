// Native: Task_DefineStatusMessage (behavior-derived placeholder name; retail symbol unknown); retail VA 0x00485330-0x0048556A.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 571 bytes, 1 block, 166 instructions.
// Tools/evidence: radare2 6.2.0 af/afbj/pdf with bin.relocs.apply over the exact image (1 block / 166 instructions / 571 bytes / 0 edges; `pd`
//   stops at the ret at 0x0048556A, followed by the nop pad 0x0048556B-0x0048556F) plus the objdump listing build/retail-dis.txt for the same
//   extent, read in two ranges (0x00485330-0x00485476 from the r2 dump, 0x00485476-0x0048556B from the listing) so no part of the body is
//   transcribed from a summary; `p8`/`ps` at 0x00540A90 for the class name (bytes 5374617475734d65737361676500 = "StatusMessage",
//   byte-identical to the literal below); `pxw` at 0x00540A8C, 0x005C8A30, 0x005C8A34 and 0x005C8A38 for the image values; a full stack-pointer
//   simulation over the 166 instructions, which returns esp to its entry value at the ret and confirms every cleanup (0x38 = 14 pushes, 0xc = 3
//   seven times, 0x10 = 4, 0x18 = 6 nine times, 0x1f8 = the 0x1e0 frame plus the last append's 6 pushes). All nine 0x004F1A80 calls were checked
//   against 0x004F1A80's own bytes: it reads its second and third arguments at [esp+8]/[esp+0xC], subtracts them at 0x004F1A88 and never
//   dereferences either, so the values that reach the container are the differences 0x20, 0x38, 0x60, 0xB0, 0xB4, 0x1B4, 0x1C4, 0x1C6 and 0x1D8.
//   Every getter was re-read at its own VA and is a single-instruction argument-less load: 0x005034D0, 0x004F1A60, 0x004F1A70, 0x004F1210,
//   0x004F1220, 0x004F1010, 0x004F0F70, 0x004F0FC0, 0x004F0FF0, 0x004F0FD0, 0x004F1000 and 0x004F0F50 all end in a ret and take no argument, so
//   every push that precedes them is inert (and, for the eight that are followed by a link call, is that call's pre-pushed handler slot).
// Behavior/CFG: registers the "StatusMessage" task type as TaskType_Register("StatusMessage", 0x1E0, Sys_GetGlobal16_5034D0(), 0) and stores
//   the returned 16-bit id as a word at 0x00540A8C (image dword 0x00000181); links the id to handler 0x00485570 on event 0 through 0x00401400,
//   then to 0x004855F0 on event 1 and 0x004856D0 on event 2 through 0x00401530, then to handlers 0x00485740, 0x00485750, 0x00485770 and
//   0x00485870 on the event ids from 0x004F1A60, 0x004F1A70, 0x004F1210 and 0x004F1220 (first two through 0x00401400, last two through
//   0x00401530); writes the dword 0x42C80000 to 0x005C8A34 and 0 to 0x005C8A30 immediately before allocating a 9-entry parameter container
//   through 0x004F1950 into 0x005C8A38; then appends nine descriptors to that container through 0x004F1A80, whose recorded differences are 0x20,
//   0x38, 0x60, 0xB0, 0xB4, 0x1B4, 0x1C4, 0x1C6 and 0x1D8 and whose type values come from 0x004F1010, 0x004F0F70, 0x004F0FC0, 0x004F0FC0,
//   0x004F0FF0, 0x004F0FD0, 0x004F1000, 0x004F1000 and 0x004F0F50 in that order. Straight-line: one basic block, no branch, no loop, 32 call
//   sites over 17 distinct callees. There is no 0x004B8810/0x004B8890 script-name tail in this body.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. Documented shape differences, all value-preserving: (1) every getter callee is an
//   argument-less load in retail that ignores the arguments the caller pushes, and the tree's transcriptions take no arguments, so those inert
//   pushes are not reproduced; each such push is simultaneously the next link call's pre-pushed handler slot, which the port keeps as the handler
//   value in the same order; (2) the same applies to the two zeros pushed before 0x005034D0; (3) retail's 0x1E0-byte frame is not reproduced -
//   only the two addresses whose difference 0x004F1A80 records are, via one addressable dword plus the retail offsets, because that difference is
//   the only thing the callee reads from them; (4) retail pushes the container's entry count (`push 9`) BEFORE the two dword stores and only then
//   calls 0x004F1950; the port stores first and passes 9 at the call, because nothing reads that stack slot between the push and the call, so the
//   pushed argument is dead until the call and no observable order changes; (5) the ninth append's sixth argument is retail's address 0x005C8A30,
//   which the port passes as the port address of the global that mirrors it (`&::g_5c8a30`) instead of the retail constant, because 0x004F1A80
//   stores that argument as a value in the parameter record and a retail address there would leave a pointer into unmapped retail .data inside
//   the record. The id slot 0x00540A8C is declared as the two 16-bit words it holds
//   (image dword 0x00000181), so retail's 16-bit store at 0x00485357 and its seven 16-bit re-loads are plain 16-bit accesses with no type
//   punning. 0x005C8A30 and 0x005C8A34 are written as dwords and are declared as raw 32-bit values, not floats: 0x42C80000 is the image-adjacent
//   bit pattern retail stores and its meaning as a float is NOT established here (the earlier native 0x004F1CB0 writes the same two values 0 and
//   0x42C80000 as a pair, which is consistent with a float pair but does not prove it). Handlers 0x00485570, 0x004855F0, 0x004856D0, 0x00485740,
//   0x00485750, 0x00485770 and 0x00485870 are raw code immediates in retail and are not transcribed yet (native gaps, all inside
//   0x0046xxxx-0x0048xxxx). 0x00540A8C, 0x005C8A30, 0x005C8A34 and 0x005C8A38 have no other owner and all have image value 0 except the id slot,
//   so this file defines them. Domain: core/, because it is one of the 0x00402930 startup registrars.
#include "igi/retail/core/task_types.hpp"

#include <cstdint>

extern "C" std::uint16_t Sys_GetGlobal16_5034D0(void);                  // 0x005034D0: argument-less word getter (sys/native_5034d0.cpp)
extern "C" std::uint8_t Script_PhaseIdGetE4_4F1A60(void);               // 0x004F1A60: argument-less byte getter
extern "C" std::uint8_t Script_PhaseIdGetE5_4F1A70(void);               // 0x004F1A70: argument-less byte getter
extern "C" std::uint32_t Sys_GetGlobal32_4F1010(void);                  // 0x004F1010: argument-less dword getter (sys/native_4f1010.cpp)
extern "C" std::uint32_t Sys_GetGlobal32_4F0F50(void);                  // 0x004F0F50: argument-less dword getter (sys/native_4f0f50.cpp)
extern "C" std::uint32_t Sys_GetGlobal32_4F0F70(void);                  // 0x004F0F70: argument-less dword getter (sys/native_4f0f70.cpp)
extern "C" std::uint32_t Sys_GetGlobal32_4F0FC0(void);                  // 0x004F0FC0: argument-less dword getter
extern "C" std::uint32_t Sys_GetGlobal32_4F0FF0(void);                  // 0x004F0FF0: argument-less dword getter
extern "C" std::uint32_t Sys_GetGlobal32_4F1210(void);                  // 0x004F1210: argument-less dword getter
extern "C" std::uint32_t Sys_GetGlobal32_4F1220(void);                  // 0x004F1220: argument-less dword getter
extern "C" std::uint32_t Core_GetRenderWordB_4F0FD0(void);              // 0x004F0FD0: argument-less dword getter (core/native_4f0fd0.cpp)
extern "C" std::uint32_t Camera_Get_4F1000(void);                       // 0x004F1000: argument-less dword getter (render/native_4f1000.cpp)
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

// Retail 0x00540A8C is a 4-byte slot whose image dword is 0x00000181, and retail stores and re-loads only its low word (0x00485357
// `mov word [0x540A8C],ax`, then `mov ax/cx/dx,[0x540A8C]` at 0x00485362/0x00485375/0x00485396/0x004853B2/0x004853CB/0x004853E6). It is declared
// as the two 16-bit words the slot actually holds, so the byte layout is identical and every store and load is a plain 16-bit access.
std::uint16_t g_540a8c[2] = {0x0181U, 0x0000U}; // retail 0x00540A8C (low word: the "StatusMessage" task type id) and 0x00540A8E (high word)
std::uint32_t g_5c8a30 = 0U;                    // retail 0x005C8A30: written 0 at 0x00485400 (image value 0)
std::uint32_t g_5c8a34 = 0U;                    // retail 0x005C8A34: written 0x42C80000 at 0x004853F6 (image value 0)
void* g_5c8a38 = nullptr;                       // retail 0x005C8A38: the parameter container from 0x004F1950 (image value 0)

void Task_DefineStatusMessage()
{
    // 0x00485330: sub esp,0x1e0 - retail's 480-byte frame, the same size it passes to TaskType_Register as the object size. The nine 0x004F1A80
    // calls below pass two addresses inside it: its first dword (each `lea ecx,[esp+0xc]` runs with 12 bytes of arguments already pushed, so the
    // address is frame+0x00) and one later slot each. 0x004F1A80 reads those two arguments at [esp+8]/[esp+0xC], subtracts them at 0x004F1A88 and
    // never dereferences them, so the port keeps one addressable dword and passes that address plus the same retail offset.
    std::int32_t append_frame_base = 0; // retail frame slot at +0x00
    const std::int32_t append_frame = reinterpret_cast<std::int32_t>(&append_frame_base);

    // 0x00485336/0x00485338: push 0; push 0 (inert arguments of the getter, and TaskType_Register's fourth/fifth slots);
    // 0x0048533A: call 0x5034D0; 0x0048533F: push eax; 0x00485340: push 0x1E0; 0x00485345: push "StatusMessage"; 0x0048534A: call 0x401900.
    const std::uint16_t class_id = TaskType_Register("StatusMessage", // 0x00485345: push 0x00540A90
                                                     0x1E0U,          // 0x00485340: push 0x1E0 (object size)
                                                     Sys_GetGlobal16_5034D0(), // 0x0048533A: call 0x5034D0 (parent type)
                                                     0U);            // 0x00485338: push 0 (task list count)

    // 0x0048534F: push 0x00485570 (pre-pushed third argument); 0x00485354: push 0; 0x00485356: push eax;
    // 0x00485357: mov word [0x540A8C],ax; 0x0048535D: call 0x401400.
    ::g_540a8c[0] = class_id; // 0x00485357: the id is stored between the argument pushes and the call, exactly as retail orders it
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_540a8c[0]), // 0x00485362: mov ax,[0x540A8C] (retail re-loads the word)
                                0U,                                     // 0x00485354: push 0 (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00485570U))); // 0x0048534F: push handler

    // 0x00485368: push 0x004855F0 (pre-pushed third argument); 0x0048536D: push 1; 0x0048536F: push eax; 0x00485370: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_540a8c[0]), // 0x00485362: mov ax,[0x540A8C]
                                           1U,                                     // 0x0048536D: push 1
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x004855F0U))); // 0x00485368: push handler

    // 0x0048537C: push 0x004856D0 (pre-pushed third argument); 0x00485381: push 2; 0x00485383: push ecx; 0x00485384: call 0x401530;
    // 0x00485389: add esp,0x38 - 14 pushes above (2 inert zeros + 3 registration arguments + three 3-argument links).
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_540a8c[0]), // 0x00485375: mov cx,[0x540A8C]
                                           2U,                                     // 0x00485381: push 2
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x004856D0U))); // 0x0048537C: push handler

    // 0x0048538C: push 0x00485740 (pre-pushed third argument, and the getter's inert argument); 0x00485391: call 0x4F1A60 (event id);
    // 0x00485396: mov dx,[0x540A8C]; 0x0048539D: push eax; 0x0048539E: push edx; 0x0048539F: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_540a8c[0]), // 0x00485396: mov dx,[0x540A8C]
                                static_cast<std::uint32_t>(Script_PhaseIdGetE4_4F1A60()), // 0x0048539D: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00485740U))); // 0x0048538C: push handler (argument 3)
    // 0x004853A4: add esp,0xc - 3 pushes above.

    // 0x004853A7: push 0x00485750 (pre-pushed third argument, and the getter's inert argument); 0x004853AC: call 0x4F1A70 (event id);
    // 0x004853B2: mov ax,[0x540A8C]; 0x004853B1: push eax; 0x004853B8: push eax; 0x004853B9: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_540a8c[0]), // 0x004853B2: mov ax,[0x540A8C]
                                static_cast<std::uint32_t>(Script_PhaseIdGetE5_4F1A70()), // 0x004853B1: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00485750U))); // 0x004853A7: push handler (argument 3)
    // 0x004853BE: add esp,0xc - 3 pushes above.

    // 0x004853C1: push 0x00485770 (pre-pushed third argument, and the getter's inert argument); 0x004853C6: call 0x4F1210 (event id);
    // 0x004853CB: mov cx,[0x540A8C]; 0x004853D2: push eax; 0x004853D3: push ecx; 0x004853D4: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_540a8c[0]), // 0x004853CB: mov cx,[0x540A8C]
                                           static_cast<std::uint32_t>(Sys_GetGlobal32_4F1210()), // 0x004853D2: push eax
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x00485770U))); // 0x004853C1: push handler
    // 0x004853D9: add esp,0xc - 3 pushes above.

    // 0x004853DC: push 0x00485870 (pre-pushed third argument, and the getter's inert argument); 0x004853E1: call 0x4F1220 (event id);
    // 0x004853E6: mov dx,[0x540A8C]; 0x004853ED: push eax; 0x004853EE: push edx; 0x004853EF: call 0x401530 (no cleanup of its own).
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_540a8c[0]), // 0x004853E6: mov dx,[0x540A8C]
                                           static_cast<std::uint32_t>(Sys_GetGlobal32_4F1220()), // 0x004853ED: push eax
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x00485870U))); // 0x004853DC: push handler

    // 0x004853F6: mov dword [0x5C8A34],0x42C80000; 0x00485400: mov dword [0x5C8A30],0. Retail pushes the entry count at 0x004853F4 first,
    // then performs these two stores, then calls 0x004F1950 at 0x0048540A; the pushed value is dead until the call, so the port stores first and
    // passes the count at the call.
    ::g_5c8a34 = 0x42C80000U; // 0x004853F6: the raw image bit pattern, written as a dword
    ::g_5c8a30 = 0U;          // 0x00485400: written as a dword

    // 0x004853F4: push 9 (the container's entry count; nine descriptors follow); 0x0048540A: call 0x4F1950;
    // 0x0048540F: add esp,0x10 (4 pushes: the event-3 link's 3 plus this 1); 0x00485412: mov [0x5C8A38],eax.
    ::g_5c8a38 = Script_TaskContainer_Alloc(9);

    // Append 1: 0x00485417/0x00485419: push 0; push 7 (inert); 0x0048541B: call 0x4F1010; 0x00485420: mov edx,[0x5C8A38]; 0x00485426: push eax;
    // 0x00485427: lea eax,[esp+0x2c] (frame+0x20); 0x0048542B: lea ecx,[esp+0xc] (frame+0x00); 0x00485432: call 0x4F1A80; 0x00485437: add esp,0x18.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(::g_5c8a38),  // argument 1: the container
                                append_frame,                                 // argument 2: retail frame+0x00
                                append_frame + 0x20,                          // argument 3: retail frame+0x20 (recorded difference 0x20)
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F1010()), // argument 4: the type from 0x4F1010
                                7, 0);                                           // argument 5: retail's leftover 7

    // Append 2: 0x0048543A/0x0048543C push 0/7; 0x0048543E: call 0x4F0F70; 0x0048544A: lea eax,[esp+0x44] (frame+0x38); 0x00485455: call 0x4F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(::g_5c8a38),
                                append_frame,
                                append_frame + 0x38,                          // 0x0048544A: frame+0x38 (recorded difference 0x38)
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F70()),
                                7, 0);

    // Append 3: 0x0048545D/0x0048545F push 0/7; 0x00485461: call 0x4F0FC0; 0x0048546D: lea eax,[esp+0x6c] (frame+0x60); 0x00485478: call 0x4F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(::g_5c8a38),
                                append_frame,
                                append_frame + 0x60,                          // 0x0048546D: frame+0x60 (recorded difference 0x60)
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FC0()),
                                7, 0);

    // Append 4: 0x00485480/0x00485482 push 0/7; 0x00485484: call 0x4F0FC0; 0x00485490: lea eax,[esp+0xbc] (frame+0xB0); 0x0048549E: call 0x4F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(::g_5c8a38),
                                append_frame,
                                append_frame + 0xB0,                          // 0x00485490: frame+0xB0 (recorded difference 0xB0)
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FC0()),
                                7, 0);

    // Append 5: 0x004854A6/0x004854A8 push 0/7; 0x004854AA: call 0x4F0FF0; 0x004854B6: lea eax,[esp+0xc0] (frame+0xB4); 0x004854C4: call 0x4F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(::g_5c8a38),
                                append_frame,
                                append_frame + 0xB4,                          // 0x004854B6: frame+0xB4 (recorded difference 0xB4)
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FF0()),
                                7, 0);

    // Append 6: 0x004854CC/0x004854CE push 0/7; 0x004854D0: call 0x4F0FD0; 0x004854DC: lea eax,[esp+0x1c0] (frame+0x1B4); 0x004854EA: call 0x4F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(::g_5c8a38),
                                append_frame,
                                append_frame + 0x1B4,                         // 0x004854DC: frame+0x1B4 (recorded difference 0x1B4)
                                static_cast<std::int32_t>(Core_GetRenderWordB_4F0FD0()),
                                7, 0);

    // Append 7: 0x004854F2/0x004854F4 push 0/7; 0x004854F6: call 0x4F1000; 0x00485502: lea eax,[esp+0x1d0] (frame+0x1C4); 0x00485510: call 0x4F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(::g_5c8a38),
                                append_frame,
                                append_frame + 0x1C4,                         // 0x00485502: frame+0x1C4 (recorded difference 0x1C4)
                                static_cast<std::int32_t>(Camera_Get_4F1000()),
                                7, 0);

    // Append 8: 0x00485518/0x0048551A push 0/7; 0x0048551C: call 0x4F1000 (the same getter as append 7); 0x00485528: lea eax,[esp+0x1d2]
    // (frame+0x1C6); 0x00485536: call 0x4F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(::g_5c8a38),
                                append_frame,
                                append_frame + 0x1C6,                         // 0x00485528: frame+0x1C6 (recorded difference 0x1C6)
                                static_cast<std::int32_t>(Camera_Get_4F1000()),
                                7, 0);

    // Append 9: 0x0048553E: push 0x5C8A30 (the getter's inert argument - the address of a global this body itself writes at 0x00485400);
    // 0x00485543: push 7; 0x00485545: call 0x4F0F50; 0x00485551: lea eax,[esp+0x1e4] (frame+0x1D8); 0x0048555F: call 0x4F1A80;
    // 0x00485564: add esp,0x1f8 - the 0x1e0 frame plus this group's 6 pushes; 0x0048556A: ret.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(::g_5c8a38),
                                append_frame,
                                append_frame + 0x1D8,                         // 0x00485551: frame+0x1D8 (recorded difference 0x1D8)
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F50()),
                                7, static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(&::g_5c8a30))); // port address of retail 0x005C8A30
} // end of Task_DefineStatusMessage

} // extern "C"
