// Native: Task_DefineExplodeObject (behavior-derived placeholder name; retail symbol unknown); retail VA 0x004813E0-0x0048177A.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 923 bytes, 1 block, 256 instructions.
// Tools/evidence: radare2 6.2.0 af/afbj/pdf with bin.relocs.apply over the exact image (1 block / 256 instructions / 923 bytes / 0 edges; `pd`
//   stops at the ret at 0x0048177A, followed by the nop pad 0x0048177B-0x0048177F) plus the objdump listing build/retail-dis.txt, which I read
//   over the whole extent in four ranges (0x004813E0-0x004814F0, 0x004814F0-0x004815EF, 0x004815EF-0x004816DF, 0x004816DF-0x00481780) so no
//   part of the body is transcribed from a summary; `ps` at 0x00540980 for the class name (byte-identical to the literal below); `pxw` at
//   0x0054097C, 0x005C1170, 0x005C1174, 0x005C1178, 0x005C117C and 0x005C1184 for the image values; a full stack-pointer simulation over the 256
//   instructions, which returns esp to its entry value at the ret and confirms every cleanup (0x38 = 14 pushes, 0xc = 3 eight times, 0x18 = 6
//   twelve times, 0x10 = 4, 0x250 = the 0x238 frame plus the last append's 6 pushes). All thirteen 0x004F1A80 calls were checked against
//   0x004F1A80's own bytes: it reads its second and third arguments at [esp+8]/[esp+0xC], subtracts them at 0x004F1A88 and never dereferences
//   either, so the values that reach the container are the differences 0xF0, 0x108, 0x170, 0x180, 0x1F0, 0x1F8, 0x1FC, 0x204, 0x208, 0x20C,
//   0x210, 0x194 and 0x214 in that retail order (the 0x194 entry is the twelfth and is deliberately out of ascending order). Every getter was
//   re-read at its own VA and is a single-instruction argument-less load ending in a ret: 0x004E0830, 0x004F1A60, 0x004F1A70, 0x00416840,
//   0x00416860, 0x004F1210, 0x004F1220, 0x004C5800, 0x004CEA20, 0x004CEA10, 0x004F1010, 0x004F0F70, 0x004F0FD0, 0x004F0F50, 0x004F0FA0 and
//   0x004F0FC0, so every push that precedes them is inert (and, for those followed by a link call, is that call's pre-pushed handler slot).
// Behavior/CFG: registers the "ExplodeObject" task type as TaskType_Register("ExplodeObject", 0x238, Sys_GetGlobal16_4E0830(), 0) and stores
//   the returned 16-bit id as a word at 0x0054097C; links the id to handler 0x00481780 on event 0 through 0x00401400, to 0x00481970 on event 1 and
//   0x00481BC0 on event 2 through 0x00401530, then to handlers 0x00481C30, 0x00481C40, 0x00481E60, 0x00481EA0, 0x00481F90, 0x00481FF0,
//   0x00482050, 0x004D3710 (event 6), 0x004824C0 and 0x00482390 on the event ids from 0x004F1A60, 0x004F1A70, 0x00416840, 0x00416860,
//   0x004F1210, 0x004F1220, 0x004C5800, the constant 6, 0x004CEA20 and 0x004CEA10 (the 0x00401400 calls carry the 0x004F1A60, 0x004F1A70,
//   0x00416840, 0x00416860, 0x004C5800, 6, 0x004CEA20 and 0x004CEA10 ids, the 0x00401530 calls the 0x004F1210 and 0x004F1220 ids); then writes
//   the dwords 0, 0x42C80000, 0 and 0x3E8 to 0x005C1170, 0x005C1174, 0x005C1178 and 0x005C117C and allocates a 13-entry parameter container
//   through 0x004F1950 into 0x005C1184; finally appends thirteen descriptors through 0x004F1A80 with the recorded differences listed above and the
//   type values from 0x004F1010, 0x004F0F70, 0x004F0FD0, 0x004F0FD0, 0x004F0F50, 0x004F0F50, 0x004F0F50, 0x004F0F50, 0x004F0F50, 0x004F0FA0,
//   0x004F0FA0, 0x004F0FC0 and 0x004F0FD0 in that order. Straight-line: one basic block, no branch, no loop, 51 call sites over 21 distinct
//   callees. There is no 0x004B8810/0x004B8890 script-name tail in this body.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. Documented shape differences, all value-preserving: (1) every getter callee is an
//   argument-less load in retail that ignores the arguments the caller pushes, and the tree's transcriptions take no arguments, so those inert
//   pushes are not reproduced; each such push is simultaneously the next link call's pre-pushed handler slot, which the port keeps as the handler
//   value in the same order; (2) the same applies to the two zeros pushed before 0x004E0830; (3) retail's 0x238-byte frame is not reproduced -
//   only the two addresses whose difference 0x004F1A80 records are, via one addressable dword plus the retail offsets, because that difference is
//   the only thing the callee reads from them; (4) retail pushes the container's entry count (`push 0xd`) BEFORE the four dword stores and only
//   then calls 0x004F1950; the port stores first and passes 13 at the call, because nothing reads that stack slot in between, so the pushed
//   argument is dead until the call and no observable order changes. The frame size equals the object size registered here (0x238), consistent
//   with the six-registrar pattern recorded in docs/fidelity/evidence/registrar-frame-is-task-object-20261025.md, so the recorded differences are
//   field offsets inside the task object. 0x0054097C is owned by sys/native_481340.cpp as a 16-bit slot and 0x005C1184 by sys/native_481350.cpp as
//   a 32-bit slot, so this file declares them extern with those types and defines only what nobody owns: 0x005C1170, 0x005C1174, 0x005C1178 and
//   0x005C117C, all written as dwords and declared as raw 32-bit values, not floats - 0x42C80000 and 0x3E8 are the image bit patterns retail
//   stores and their float/integer meaning is NOT established here (this is the third site writing a 0/0x42C80000 pair). Handlers 0x00481780,
//   0x00481970, 0x00481BC0, 0x00481C30, 0x00481C40, 0x00481E60, 0x00481EA0, 0x00481F90, 0x00481FF0, 0x00482050, 0x004824C0 and 0x00482390 are
//   raw code immediates in retail and are not transcribed yet (native gaps, all inside 0x0046xxxx-0x0048xxxx); 0x004D3710 is already
//   transcribed in script/native_4D3710.cpp. Domain: core/, because it is one of the 0x00402930 startup registrars.
#include "igi/retail/core/task_types.hpp"

#include <cstdint>

extern "C" std::uint16_t Sys_GetGlobal16_4E0830(void);                  // 0x004E0830: argument-less word getter (sys/native_4e0830.cpp)
extern "C" std::uint8_t Script_PhaseIdGetE4_4F1A60(void);               // 0x004F1A60: argument-less byte getter
extern "C" std::uint8_t Script_PhaseIdGetE5_4F1A70(void);               // 0x004F1A70: argument-less byte getter
extern "C" std::uint8_t Sys_GetSettingByte_416840(void);                // 0x00416840: argument-less byte getter (config/native_416840.cpp)
extern "C" std::uint8_t Sys_GetSettingByte_416860(void);                // 0x00416860: argument-less byte getter (config/native_416860.cpp)
extern "C" std::uint32_t Sys_GetGlobal32_4F1010(void);                  // 0x004F1010: argument-less dword getter (sys/native_4f1010.cpp)
extern "C" std::uint32_t Sys_GetGlobal32_4F0F50(void);                  // 0x004F0F50: argument-less dword getter (sys/native_4f0f50.cpp)
extern "C" std::uint32_t Sys_GetGlobal32_4F0F70(void);                  // 0x004F0F70: argument-less dword getter (sys/native_4f0f70.cpp)
extern "C" std::uint32_t Sys_GetGlobal32_4F0FC0(void);                  // 0x004F0FC0: argument-less dword getter
extern "C" std::uint32_t Sys_GetGlobal32_4F1210(void);                  // 0x004F1210: argument-less dword getter
extern "C" std::uint32_t Sys_GetGlobal32_4F1220(void);                  // 0x004F1220: argument-less dword getter
extern "C" std::uint32_t Core_GetRenderWordA_4F0FA0(void);              // 0x004F0FA0: argument-less dword getter
extern "C" std::uint32_t Core_GetRenderWordB_4F0FD0(void);              // 0x004F0FD0: argument-less dword getter (core/native_4f0fd0.cpp)
extern "C" std::uint8_t QTask_PhaseIdGet548176_4CEA10(void);            // 0x004CEA10: argument-less byte getter (script/native_4cea10.cpp)
extern "C" std::uint8_t QTask_PhaseIdGet548177_4CEA20(void);            // 0x004CEA20: argument-less byte getter (script/native_4cea20.cpp)
extern "C" std::uint8_t DynObj_GetByte_4C5800(void);                    // 0x004C5800: argument-less byte getter (dynobj/native_4c5800.cpp)
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

// 0x0054097C: 16 bits, owned and defined by sys/native_481340.cpp; this body is its writer.
extern std::uint16_t g_54097c;

// 0x005C1184: 32 bits, owned and defined by sys/native_481350.cpp; this body is its writer.
extern std::uint32_t g_5c1184;

std::uint32_t g_5c1170 = 0U; // retail 0x005C1170: written 0 at 0x0048153F (image value 0)
std::uint32_t g_5c1174 = 0U; // retail 0x005C1174: written 0x42C80000 at 0x00481549 (image value 0)
std::uint32_t g_5c1178 = 0U; // retail 0x005C1178: written 0 at 0x00481553 (image value 0)
std::uint32_t g_5c117c = 0U; // retail 0x005C117C: written 0x3E8 at 0x0048155D (image value 0)

void Task_DefineExplodeObject()
{
    // 0x004813E0: sub esp,0x238 - retail's 568-byte frame, the same size it passes to TaskType_Register as the object size. The thirteen
    // 0x004F1A80 calls below pass two addresses inside it: its first dword (each `lea ecx,[esp+0xc]` runs with 12 bytes of arguments already
    // pushed, so the address is frame+0x00) and one later slot each. 0x004F1A80 reads those two arguments at [esp+8]/[esp+0xC], subtracts them at
    // 0x004F1A88 and never dereferences them, so the port keeps one addressable dword and passes that address plus the same retail offset.
    std::int32_t append_frame_base = 0; // retail frame slot at +0x00
    const std::int32_t append_frame = reinterpret_cast<std::int32_t>(&append_frame_base);

    // 0x004813E6/0x004813E8: push 0; push 0 (inert arguments of the getter, and TaskType_Register's fourth/fifth slots);
    // 0x004813EA: call 0x4E0830; 0x004813EF: push eax; 0x004813F0: push 0x238; 0x004813F5: push "ExplodeObject"; 0x004813FA: call 0x401900.
    const std::uint16_t class_id = TaskType_Register("ExplodeObject", // 0x004813F5: push 0x00540980
                                                     0x238U,          // 0x004813F0: push 0x238 (object size)
                                                     Sys_GetGlobal16_4E0830(), // 0x004813EA: call 0x4E0830 (parent type)
                                                     0U);            // 0x004813E8: push 0 (task list count)

    // 0x004813FF: push 0x00481780 (pre-pushed third argument); 0x00481404: push 0; 0x00481406: push eax;
    // 0x00481407: mov word [0x54097C],ax; 0x0048140D: call 0x401400.
    ::g_54097c = class_id; // 0x00481407: a plain 16-bit store, matching the 16-bit slot the owner declares
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_54097c), // 0x00481412: mov ax,[0x54097C] (retail re-loads the word)
                                0U,                                    // 0x00481404: push 0 (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00481780U))); // 0x004813FF: push handler

    // 0x00481418: push 0x00481970 (pre-pushed third argument); 0x0048141D: push 1; 0x0048141F: push eax; 0x00481420: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_54097c), // 0x00481412: mov ax,[0x54097C]
                                           1U,                                    // 0x0048141D: push 1
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x00481970U))); // 0x00481418: push handler

    // 0x0048142C: push 0x00481BC0 (pre-pushed third argument); 0x00481431: push 2; 0x00481433: push ecx; 0x00481434: call 0x401530;
    // 0x00481439: add esp,0x38 - 14 pushes above (2 inert zeros + 3 registration arguments + three 3-argument links).
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_54097c), // 0x00481425: mov cx,[0x54097C]
                                           2U,                                    // 0x00481431: push 2
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x00481BC0U))); // 0x0048142C: push handler

    // 0x0048143C: push 0x00481C30 (pre-pushed third argument, and the getter's inert argument); 0x00481441: call 0x4F1A60 (event id);
    // 0x00481446: mov dx,[0x54097C]; 0x0048144D: push eax; 0x0048144E: push edx; 0x0048144F: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_54097c), // 0x00481446: mov dx,[0x54097C]
                                static_cast<std::uint32_t>(Script_PhaseIdGetE4_4F1A60()), // 0x0048144D: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00481C30U))); // 0x0048143C: push handler (argument 3)
    // 0x00481454: add esp,0xc - 3 pushes above.

    // 0x00481457: push 0x00481C40 (pre-pushed third argument, and the getter's inert argument); 0x0048145C: call 0x4F1A70 (event id);
    // 0x00481461: push eax; 0x00481462: mov ax,[0x54097C]; 0x00481468: push eax; 0x00481469: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_54097c), // 0x00481462: mov ax,[0x54097C]
                                static_cast<std::uint32_t>(Script_PhaseIdGetE5_4F1A70()), // 0x00481461: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00481C40U))); // 0x00481457: push handler (argument 3)
    // 0x0048146E: add esp,0xc - 3 pushes above.

    // 0x00481471: push 0x00481E60 (pre-pushed third argument, and the getter's inert argument); 0x00481476: call 0x416840 (event id);
    // 0x0048147B: mov cx,[0x54097C]; 0x00481482: push eax; 0x00481483: push ecx; 0x00481484: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_54097c), // 0x0048147B: mov cx,[0x54097C]
                                static_cast<std::uint32_t>(Sys_GetSettingByte_416840()), // 0x00481482: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00481E60U))); // 0x00481471: push handler (argument 3)
    // 0x00481489: add esp,0xc - 3 pushes above.

    // 0x0048148C: push 0x00481EA0 (pre-pushed third argument, and the getter's inert argument); 0x00481491: call 0x416860 (event id);
    // 0x00481496: mov dx,[0x54097C]; 0x0048149D: push eax; 0x0048149E: push edx; 0x0048149F: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_54097c), // 0x00481496: mov dx,[0x54097C]
                                static_cast<std::uint32_t>(Sys_GetSettingByte_416860()), // 0x0048149D: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00481EA0U))); // 0x0048148C: push handler (argument 3)
    // 0x004814A4: add esp,0xc - 3 pushes above.

    // 0x004814A7: push 0x00481F90 (pre-pushed third argument, and the getter's inert argument); 0x004814AC: call 0x4F1210 (event id);
    // 0x004814B1: push eax; 0x004814B2: mov ax,[0x54097C]; 0x004814B8: push eax; 0x004814B9: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_54097c), // 0x004814B2: mov ax,[0x54097C]
                                           static_cast<std::uint32_t>(Sys_GetGlobal32_4F1210()), // 0x004814B1: push eax
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x00481F90U))); // 0x004814A7: push handler
    // 0x004814BE: add esp,0xc - 3 pushes above.

    // 0x004814C1: push 0x00481FF0 (pre-pushed third argument, and the getter's inert argument); 0x004814C6: call 0x4F1220 (event id);
    // 0x004814CB: mov cx,[0x54097C]; 0x004814D2: push eax; 0x004814D3: push ecx; 0x004814D4: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_54097c), // 0x004814CB: mov cx,[0x54097C]
                                           static_cast<std::uint32_t>(Sys_GetGlobal32_4F1220()), // 0x004814D2: push eax
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x00481FF0U))); // 0x004814C1: push handler
    // 0x004814D9: add esp,0xc - 3 pushes above.

    // 0x004814DC: push 0x00482050 (pre-pushed third argument, and the getter's inert argument); 0x004814E1: call 0x4C5800 (event id);
    // 0x004814E6: mov dx,[0x54097C]; 0x004814ED: push eax; 0x004814EE: push edx; 0x004814EF: call 0x401400 (no cleanup of its own).
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_54097c), // 0x004814E6: mov dx,[0x54097C]
                                static_cast<std::uint32_t>(DynObj_GetByte_4C5800()), // 0x004814ED: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00482050U))); // 0x004814DC: push handler (argument 3)

    // 0x004814FA: push 0x004D3710 (pre-pushed third argument); 0x004814FF: push 6 (event id, a constant here); 0x00481501: push eax;
    // 0x00481502: call 0x401400; 0x00481507: add esp,0x18 (this link's 3 pushes plus the previous link's 3).
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_54097c), // 0x004814F4: mov ax,[0x54097C]
                                6U,                                     // 0x004814FF: push 6 (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x004D3710U))); // 0x004814FA: push handler (argument 3)

    // 0x0048150A: push 0x004824C0 (pre-pushed third argument, and the getter's inert argument); 0x0048150F: call 0x4CEA20 (event id);
    // 0x00481514: push eax; 0x00481515: mov cx,[0x54097C]; 0x0048151C: push ecx; 0x0048151D: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_54097c), // 0x00481515: mov cx,[0x54097C]
                                static_cast<std::uint32_t>(QTask_PhaseIdGet548177_4CEA20()), // 0x00481514: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x004824C0U))); // 0x0048150A: push handler (argument 3)
    // 0x00481522: add esp,0xc - 3 pushes above.

    // 0x00481525: push 0x00482390 (pre-pushed third argument, and the getter's inert argument); 0x0048152A: call 0x4CEA10 (event id);
    // 0x0048152F: mov dx,[0x54097C]; 0x00481536: push eax; 0x00481537: push edx; 0x00481538: call 0x401400 (no cleanup of its own).
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_54097c), // 0x0048152F: mov dx,[0x54097C]
                                static_cast<std::uint32_t>(QTask_PhaseIdGet548176_4CEA10()), // 0x00481536: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00482390U))); // 0x00481525: push handler (argument 3)

    // 0x0048153F: mov dword [0x5C1170],0; 0x00481549: mov dword [0x5C1174],0x42C80000; 0x00481553: mov dword [0x5C1178],0;
    // 0x0048155D: mov dword [0x5C117C],0x3E8. Retail pushes the entry count at 0x0048153D first, then performs these four stores, then calls
    // 0x004F1950 at 0x00481567; the pushed value is dead until the call, so the port stores first and passes the count at the call.
    ::g_5c1170 = 0U;          // 0x0048153F: written as a dword
    ::g_5c1174 = 0x42C80000U; // 0x00481549: the raw image bit pattern, written as a dword
    ::g_5c1178 = 0U;          // 0x00481553: written as a dword
    ::g_5c117c = 0x3E8U;      // 0x0048155D: written as a dword

    // 0x0048153D: push 0xd (the container's entry count; thirteen descriptors follow); 0x00481567: call 0x4F1950;
    // 0x0048156C: add esp,0x10 (4 pushes: the last link's 3 plus this 1); 0x0048156F: mov [0x5C1184],eax.
    ::g_5c1184 = reinterpret_cast<std::uint32_t>(Script_TaskContainer_Alloc(13));

    // Append 1: 0x00481574/0x00481576 push 0/7 (inert); 0x00481578: call 0x4F1010; 0x0048157D: mov edx,[0x5C1184]; 0x00481584: lea
    // eax,[esp+0xfc] (frame+0xF0); 0x0048158B: lea ecx,[esp+0xc] (frame+0x00); 0x00481592: call 0x4F1A80; 0x00481597: add esp,0x18.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(::g_5c1184),  // argument 1: the container
                                append_frame,                                 // argument 2: retail frame+0x00
                                append_frame + 0xF0,                          // argument 3: retail frame+0xF0 (recorded difference 0xF0)
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F1010()), // argument 4: the type from 0x4F1010
                                7, 0);                                           // argument 5: retail's leftover 7

    // Append 2: 0x0048159A/0x0048159C push 0/7; 0x0048159E: call 0x4F0F70; 0x004815AA: lea eax,[esp+0x114] (frame+0x108); call at 0x004815B8.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(::g_5c1184),
                                append_frame,
                                append_frame + 0x108,                         // 0x004815AA: frame+0x108 (recorded difference 0x108)
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F70()),
                                7, 0);

    // Append 3: 0x004815C0/0x004815C2 push 0/7; 0x004815C4: call 0x4F0FD0; 0x004815D0: lea eax,[esp+0x17c] (frame+0x170); call at 0x004815DE.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(::g_5c1184),
                                append_frame,
                                append_frame + 0x170,                         // 0x004815D0: frame+0x170 (recorded difference 0x170)
                                static_cast<std::int32_t>(Core_GetRenderWordB_4F0FD0()),
                                7, 0);

    // Append 4: 0x004815E6/0x004815E8 push 0/7; 0x004815EA: call 0x4F0FD0; 0x004815F6: lea eax,[esp+0x18c] (frame+0x180); call at 0x00481604.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(::g_5c1184),
                                append_frame,
                                append_frame + 0x180,                         // 0x004815F6: frame+0x180 (recorded difference 0x180)
                                static_cast<std::int32_t>(Core_GetRenderWordB_4F0FD0()),
                                7, 0);

    // Append 5: 0x0048160C: push 0x5C1170 (the getter's inert argument - the address of a global this body itself writes at 0x0048153F);
    // 0x00481611: push 7; 0x00481613: call 0x4F0F50; 0x0048161F: lea eax,[esp+0x1fc] (frame+0x1F0); call at 0x0048162D.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(::g_5c1184),
                                append_frame,
                                append_frame + 0x1F0,                         // 0x0048161F: frame+0x1F0 (recorded difference 0x1F0)
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F50()),
                                7, static_cast<std::int32_t>(static_cast<std::uintptr_t>(0x005C1170U)));

    // Append 6: 0x00481635: push 0x5C1170; 0x0048163A: push 7; 0x0048163C: call 0x4F0F50; 0x00481648: lea eax,[esp+0x204] (frame+0x1F8);
    // call at 0x00481656.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(::g_5c1184),
                                append_frame,
                                append_frame + 0x1F8,                         // 0x00481648: frame+0x1F8 (recorded difference 0x1F8)
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F50()),
                                7, static_cast<std::int32_t>(static_cast<std::uintptr_t>(0x005C1170U)));

    // Append 7: 0x0048165E: push 0x5C1170; 0x00481663: push 7; 0x00481665: call 0x4F0F50; 0x00481671: lea eax,[esp+0x208] (frame+0x1FC);
    // call at 0x0048167F.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(::g_5c1184),
                                append_frame,
                                append_frame + 0x1FC,                         // 0x00481671: frame+0x1FC (recorded difference 0x1FC)
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F50()),
                                7, static_cast<std::int32_t>(static_cast<std::uintptr_t>(0x005C1170U)));

    // Append 8: 0x00481687: push 0x5C1170; 0x0048168C: push 7; 0x0048168E: call 0x4F0F50; 0x0048169A: lea eax,[esp+0x210] (frame+0x204);
    // call at 0x004816A8.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(::g_5c1184),
                                append_frame,
                                append_frame + 0x204,                         // 0x0048169A: frame+0x204 (recorded difference 0x204)
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F50()),
                                7, static_cast<std::int32_t>(static_cast<std::uintptr_t>(0x005C1170U)));

    // Append 9: 0x004816B0: push 0x5C1170; 0x004816B5: push 7; 0x004816B7: call 0x4F0F50; 0x004816C3: lea eax,[esp+0x214] (frame+0x208);
    // call at 0x004816D1.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(::g_5c1184),
                                append_frame,
                                append_frame + 0x208,                         // 0x004816C3: frame+0x208 (recorded difference 0x208)
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F50()),
                                7, static_cast<std::int32_t>(static_cast<std::uintptr_t>(0x005C1170U)));

    // Append 10: 0x004816D9: push 0x5C1178 (the getter's inert argument - the address of a global this body writes at 0x00481553);
    // 0x004816DE: push 7; 0x004816E0: call 0x4F0FA0; 0x004816EC: lea eax,[esp+0x218] (frame+0x20C); call at 0x004816FA.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(::g_5c1184),
                                append_frame,
                                append_frame + 0x20C,                         // 0x004816EC: frame+0x20C (recorded difference 0x20C)
                                static_cast<std::int32_t>(Core_GetRenderWordA_4F0FA0()),
                                7, static_cast<std::int32_t>(static_cast<std::uintptr_t>(0x005C1178U)));

    // Append 11: 0x00481702: push 0x5C1178; 0x00481707: push 7; 0x00481709: call 0x4F0FA0; 0x00481715: lea eax,[esp+0x21c] (frame+0x210);
    // call at 0x00481723.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(::g_5c1184),
                                append_frame,
                                append_frame + 0x210,                         // 0x00481715: frame+0x210 (recorded difference 0x210)
                                static_cast<std::int32_t>(Core_GetRenderWordA_4F0FA0()),
                                7, static_cast<std::int32_t>(static_cast<std::uintptr_t>(0x005C1178U)));

    // Append 12: 0x0048172B/0x0048172D push 0/7; 0x0048172F: call 0x4F0FC0; 0x0048173B: lea eax,[esp+0x1a0] (frame+0x194); call at 0x00481749.
    // This entry's offset is out of ascending order in retail and is kept in retail's position.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(::g_5c1184),
                                append_frame,
                                append_frame + 0x194,                         // 0x0048173B: frame+0x194 (recorded difference 0x194)
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FC0()),
                                7, 0);

    // Append 13: 0x00481751/0x00481753 push 0/7; 0x00481755: call 0x4F0FD0; 0x00481761: lea eax,[esp+0x220] (frame+0x214); call at 0x0048176F;
    // 0x00481774: add esp,0x250 - the 0x238 frame plus this group's 6 pushes; 0x0048177A: ret.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(::g_5c1184),
                                append_frame,
                                append_frame + 0x214,                         // 0x00481761: frame+0x214 (recorded difference 0x214)
                                static_cast<std::int32_t>(Core_GetRenderWordB_4F0FD0()),
                                7, 0);
} // end of Task_DefineExplodeObject

} // extern "C"
