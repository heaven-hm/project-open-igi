// Native: Task_DefineBinocularOverlayAndHolder (behavior-derived placeholder name; retail symbol unknown); retail VA 0x00470FA0-0x00471113.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 372 bytes, 1 block, 101 instructions.
// Tools/evidence: radare2 6.2.0 af/afbj/pdf with bin.relocs.apply over the exact image (1 block / 101 instructions / 372 bytes / 0 edges) plus
//   the objdump listing build/retail-dis.txt for the same extent (0x00470FA0-0x00471113; the ret is at 0x00471113, eleven nops follow, and
//   0x00471120 is the next function, which this body uses as a handler); `ps` at 0x00540294, 0x0054026C and 0x00540284 for the three literals
//   (byte-identical to "BinocularOverlay", "BinocularOverlayHolder" and "binoculars.spr"); `pxw` at 0x005401F8, which reads 0x01810181 - i.e.
//   BOTH id slots hold image value 0x0181 (0x005401F8 and 0x005401FA); `pxw` at 0x005BE344 and 0x005BE348 for the two image-zero dwords; a full
//   stack-pointer simulation over all 101 instructions, which returns esp to its entry value at the ret and accounts for every cleanup (0x38
//   twice = 14 pushes each for one registration block's two inert arguments, registration and first two links; 0xc five times; 0x10 twice, each
//   covering a preceding link's three pushes plus one more; 0x90 once = the 0x78 frame plus the final append's six pushes). Every callee was
//   re-read at its own VA. Argument numbers were derived by counting the pushes above each access, not by reading an [esp+N] offset as an
//   argument number.
// Behavior/CFG: ONE body with a 0x78-byte frame that performs TWO complete registrations. First "BinocularOverlay" (0x00540294) of size 0x2E00
//   with parent QTask_GetCurrentType() -> id at 0x005401F8, linking e0->0x00471120 (0x00401400), e1->0x004D3710 and e2->0x004713C0
//   (0x00401530), then 0x00487190->0x00471440, 0x004E7530->0x00472400 (0x00401530) and 0x00460BF0->0x00472440 (0x00401400), and stores the
//   "binoculars.spr" record from 0x004169D0 at 0x005BE344. Second "BinocularOverlayHolder" (0x0054026C) of size 0x78 - the frame size - with the
//   same parent -> id at 0x005401FA, linking e0->0x00471310 (0x00401400), e1->0x00471330 and e2->0x00471400 (0x00401530), then
//   0x004F1A60->0x004723D0 and 0x004F1A70->0x004723E0 (both 0x00401400). It then allocates a ONE-entry parameter container into 0x005BE348
//   through 0x004F1950 and appends a single parameter record through 0x004F1A80 (type from 0x004F0FC0, recorded delta 0x24 against frame+0x00).
//   One basic block, no branch, no loop, 24 call sites over 13 distinct callees. There is NO 0x004B8810 script-name tail in this body - the
//   container append is the last thing it does.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. Both id slots are in the 0x005401xx cluster and both hold image 0x0181, which are two more
//   sightings of that value and the first in this cluster; they are written with 16-bit stores and read back with 16-bit loads, so they are
//   declared as std::uint16_t. Because their image value is 0x0181 rather than 0 they are expected to land in .data rather than .bss, which is
//   the falsifiable prediction recorded for the 0x0181 pattern; the object was checked and reports both as `D`. The two other globals are
//   image-zero dwords: 0x005BE344 (the "binoculars.spr" record pointer, written at 0x00471053) and 0x005BE348 (the one-entry container, written
//   at 0x004710E8), both declared as std::uint32_t. The parent getter 0x004018F0 is the argument-less QTask_GetCurrentType(), which
//   engine/include/igi/retail/core/task_types.hpp already declares; its two pushed zeros are inert. The frame is 0x78, which is the SECOND
//   registration's object size and not the first's 0x2E00, so the frame belongs to the holder type whose container this body fills - the frame
//   is not a property of the body. 0x2E00 is the largest registered object size seen in this lane so far - the others are 0x250, 0x220, 0x1C8,
//   0x1B8, 0x1B0, 0x140, 0x138, 0x120, 0x80, 0x78, 0x54 and 0x38 - and "BinocularOverlay" is a gameplay-visible type rather than a
//   bookkeeping one, so the size is worth keeping in view when the type is eventually exercised. Handler 0x004D3710 appears as e1 of the first registration and is also the immediate other registrars link;
//   the shared-handler sweep over this body's twelve handler immediates is what confirmed that. Documented shape differences, all
//   value-preserving: (1) every getter callee is an argument-less load in retail that ignores the arguments the caller pushes, and the tree's
//   transcriptions take no arguments, so those inert pushes are not reproduced; each is simultaneously the next 0x00401400/0x00401530 link
//   call's pre-pushed handler, which the port keeps as the handler value in the same order, and for the single 0x004F0FC0 call they are
//   `push 0` and `push 7`; (2) byte-valued event ids are zero-extended in the port, which cannot change behaviour because every link fed by a
//   byte getter here goes through 0x00401400 or 0x00401530, which mask the event word for the phase index; (3) retail's 0x78-byte frame is
//   reproduced as a single local slot whose address stands in for the frame base - the one 0x004F1A80 call passes frame+0x00 and frame+0x24,
//   exactly the addresses retail computes, and nothing reads the intervening bytes. Handlers 0x00471120, 0x004713C0, 0x00471440, 0x00472400,
//   0x00472440, 0x00471310, 0x00471330, 0x00471400, 0x004723D0 and 0x004723E0 are raw code immediates in retail and are not transcribed yet
//   (native gaps in 0x0046xxxx-0x0048xxxx); handler 0x004D3710 is above 0x0048FFFF and is already implemented elsewhere. Domain: core/,
//   because it is one of the 0x00402930 startup registrars.
#include "igi/retail/core/task_types.hpp"

#include <cstdint>

extern "C" std::uint8_t Sys_GetGlobal8_487190(void);              // 0x00487190: argument-less byte getter (core/native_487190.cpp)
extern "C" std::uint32_t Core_GetEventWord_4E7530(void);          // 0x004E7530: argument-less dword getter (core/native_4e7530.cpp)
extern "C" std::uint8_t Task_GetCachedEventId_460BF0(void);       // 0x00460BF0: lazily allocated event id (core/native_00460bf0.cpp)
extern "C" void* Sys_LookupOrAppendRecord_4169D0(char const* name); // 0x004169D0: record lookup/append (core/native_4169d0.cpp)
extern "C" std::uint32_t Sys_GetGlobal32_4F0FC0(void);            // 0x004F0FC0: argument-less dword getter (the appended type)
extern "C" std::uint8_t Script_PhaseIdGetE4_4F1A60(void);         // 0x004F1A60: argument-less byte getter (script_state.hpp)
extern "C" std::uint8_t Script_PhaseIdGetE5_4F1A70(void);         // 0x004F1A70: argument-less byte getter (script_state.hpp)
extern "C" void* Script_TaskContainer_Alloc(std::int32_t count);  // 0x004F1950 (script/native_4f1950.cpp)
extern "C" void* Script_TaskContainer_Append(std::int32_t* const container, std::int32_t const arg2,
                                             std::int32_t const arg3, std::int32_t const arg4,
                                             std::int32_t const arg5, std::int32_t const arg6);
// 0x004F1A80 (script/native_4f1a80.cpp). SIX arguments, corrected: the callee reads arg1..arg4, arg5 as a byte (recorded at record +0x0C)
// and arg6 as a dword (recorded at record +0x08). The two reads are both `[esp+0x20]` in retail, at different push depths, with a `pop edi`
// between them (0x004F1AB1 with twelve bytes pushed = arg5, 0x004F1ACA with eight = arg6). Earlier revisions of this file declared five
// parameters and the tree's implementation wrote arg5 into both fields; both are now corrected. Every append below passes the sixth word that
// retail pushes for it. // 0x004F1A80 (script/native_4f1a80.cpp)

extern "C" {

// 0x005401F8: the "BinocularOverlay" task type id. Image value 0x0181, overwritten by a 16-bit store from TaskType_Register's return value.
std::uint16_t g_5401f8 = 0x0181U;

// 0x005401FA: the "BinocularOverlayHolder" task type id. Image value 0x0181, overwritten by a 16-bit store.
std::uint16_t g_5401fa = 0x0181U;

// 0x005BE344: the "binoculars.spr" record pointer. Image value 0, written and read as a dword.
std::uint32_t g_5be344 = 0U;

// 0x005BE348: the one-entry parameter container. Image value 0, written and read as a dword.
std::uint32_t g_5be348 = 0U;

void Task_DefineBinocularOverlayAndHolder()
{
    // 0x00470FA0: sub esp,0x78 - retail's 120-byte frame, which is the SECOND registration's object size. The single 0x004F1A80 call below
    // passes two addresses inside it: its second argument (every `lea ecx,[esp+0xc]` runs with 12 bytes of arguments already pushed, so the
    // address is frame+0x00) and frame+0x24 (0x004F1A80 subtracts them at 0x004F1A9D and stores 0x24).
    std::int32_t append_frame_base = 0; // retail frame slot at +0x00
    const std::int32_t append_frame = reinterpret_cast<std::int32_t>(&append_frame_base);

    // ===== Registration 1: BinocularOverlay =====
    // 0x00470FA3/0x00470FA5: push 0; push 0 (both inert arguments of the parent getter); 0x00470FA7: call 0x4018F0; 0x00470FAC: push eax;
    // 0x00470FAD: push 0x2E00; 0x00470FB2: push "BinocularOverlay"; 0x00470FB7: call 0x401900.
    const std::uint16_t binocular_overlay_id = TaskType_Register("BinocularOverlay", // 0x00470FB2: push 0x00540294
                                                                 0x2E00U, // 0x00470FAD: push 0x2E00 (object size)
                                                                 QTask_GetCurrentType(), // 0x00470FA7: call 0x4018F0 (parent type)
                                                                 0U);     // 0x00470FA5: push 0 (inert)

    // 0x00470FBC: push 0x00471120 (pre-pushed handler); 0x00470FC1: push 0; 0x00470FC3: push eax;
    // 0x00470FC4: mov word [0x5401F8],ax (the store lands between the pushes and the call); 0x00470FCA: call 0x401400.
    ::g_5401f8 = binocular_overlay_id;
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5401f8), // 0x00470FCF: mov ax,[0x5401F8]
                                0U,                                     // 0x00470FC1: push 0 (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00471120U))); // 0x00470FBC: push handler

    // 0x00470FD5: push 0x004D3710; 0x00470FDA: push 1; 0x00470FDC: push eax; 0x00470FDD: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_5401f8), // 0x00470FCF: mov ax,[0x5401F8]
                                           1U,                                     // 0x00470FDA: push 1 (argument 2)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x004D3710U))); // 0x00470FD5: push handler

    // 0x00470FE2: mov cx,[0x5401F8]; 0x00470FE9: push 0x004713C0; 0x00470FEE: push 2; 0x00470FF0: push ecx; 0x00470FF1: call 0x401530;
    // 0x00470FF6: add esp,0x38 - the fourteen pushes above (two inert, three registration, three per link).
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_5401f8), // 0x00470FE2: mov cx,[0x5401F8]
                                           2U,                                     // 0x00470FEE: push 2 (argument 2)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x004713C0U))); // 0x00470FE9: push handler

    // 0x00470FF9: push 0x00471440; 0x00470FFE: call 0x487190; 0x00471003: mov dx,[0x5401F8]; 0x0047100A: push eax; 0x0047100B: push edx;
    // 0x0047100C: call 0x401400; 0x00471011: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5401f8), // 0x00471003: mov dx,[0x5401F8]
                                static_cast<std::uint32_t>(Sys_GetGlobal8_487190()), // 0x0047100A: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00471440U))); // 0x00470FF9: push handler

    // 0x00471014: push 0x00472400; 0x00471019: call 0x4E7530; 0x0047101E: push eax; 0x0047101F: mov ax,[0x5401F8]; 0x00471025: push eax;
    // 0x00471026: call 0x401530; 0x0047102B: add esp,0xc.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_5401f8), // 0x0047101F: mov ax,[0x5401F8]
                                           static_cast<std::uint32_t>(Core_GetEventWord_4E7530()), // 0x0047101E: push eax (argument 2)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x00472400U))); // 0x00471014: push handler

    // 0x0047102E: push 0x00472440; 0x00471033: call 0x460BF0; 0x00471038: mov cx,[0x5401F8]; 0x0047103F: push eax; 0x00471040: push ecx;
    // 0x00471041: call 0x401400. This link has no cleanup of its own.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5401f8), // 0x00471038: mov cx,[0x5401F8]
                                static_cast<std::uint32_t>(Task_GetCachedEventId_460BF0()), // 0x0047103F: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00472440U))); // 0x0047102E: push handler

    // 0x00471046: push "binoculars.spr"; 0x0047104B: call 0x4169D0; 0x00471050: add esp,0x10 - the last link's three pushes plus this one;
    // 0x00471053: mov [0x5BE344],eax.
    ::g_5be344 = reinterpret_cast<std::uint32_t>(Sys_LookupOrAppendRecord_4169D0("binoculars.spr")); // 0x00471046, stored 0x00471053

    // ===== Registration 2: BinocularOverlayHolder =====
    // 0x00471058/0x0047105A: push 0; push 0 (inert); 0x0047105C: call 0x4018F0; 0x00471061: push eax; 0x00471062: push 0x78;
    // 0x00471064: push "BinocularOverlayHolder"; 0x00471069: call 0x401900.
    const std::uint16_t holder_id = TaskType_Register("BinocularOverlayHolder", // 0x00471064: push 0x0054026C
                                                      0x78U,      // 0x00471062: push 0x78 (object size, equal to the frame)
                                                      QTask_GetCurrentType(), // 0x0047105C: call 0x4018F0
                                                      0U);        // 0x0047105A: push 0 (inert)

    // 0x0047106E: push 0x00471310; 0x00471073: push 0; 0x00471075: push eax; 0x00471076: mov word [0x5401FA],ax;
    // 0x0047107C: call 0x401400.
    ::g_5401fa = holder_id;
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5401fa), // 0x00471081: mov dx,[0x5401FA]
                                0U,                                     // 0x00471073: push 0 (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00471310U))); // 0x0047106E: push handler

    // 0x00471088: push 0x00471330; 0x0047108D: push 1; 0x0047108F: push edx; 0x00471090: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_5401fa), // 0x00471081: mov dx,[0x5401FA]
                                           1U,                                     // 0x0047108D: push 1 (argument 2)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x00471330U))); // 0x00471088: push handler

    // 0x00471095: mov ax,[0x5401FA]; 0x0047109B: push 0x00471400; 0x004710A0: push 2; 0x004710A2: push eax; 0x004710A3: call 0x401530;
    // 0x004710A8: add esp,0x38.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_5401fa), // 0x00471095: mov ax,[0x5401FA]
                                           2U,                                     // 0x004710A0: push 2 (argument 2)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x00471400U))); // 0x0047109B: push handler

    // 0x004710AB: push 0x004723D0; 0x004710B0: call 0x4F1A60; 0x004710B5: mov cx,[0x5401FA]; 0x004710BC: push eax; 0x004710BD: push ecx;
    // 0x004710BE: call 0x401400; 0x004710C3: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5401fa), // 0x004710B5: mov cx,[0x5401FA]
                                static_cast<std::uint32_t>(Script_PhaseIdGetE4_4F1A60()), // 0x004710BC: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x004723D0U))); // 0x004710AB: push handler

    // 0x004710C6: push 0x004723E0; 0x004710CB: call 0x4F1A70; 0x004710D0: push eax; 0x004710D1: mov dx,[0x5401FA]; 0x004710D8: push edx;
    // 0x004710D9: call 0x401400. This link has no cleanup of its own.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5401fa), // 0x004710D1: mov dx,[0x5401FA]
                                static_cast<std::uint32_t>(Script_PhaseIdGetE5_4F1A70()), // 0x004710D0: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x004723E0U))); // 0x004710C6: push handler

    // 0x004710DE: push 1 (Script_TaskContainer_Alloc's argument); 0x004710E0: call 0x4F1950;
    // 0x004710E5: add esp,0x10 - the last link's three pushes plus this argument; 0x004710E8: mov [0x5BE348],eax.
    ::g_5be348 = reinterpret_cast<std::uint32_t>(Script_TaskContainer_Alloc(1));

    // 0x004710ED/0x004710EF: push 0/7 (inert arguments of the type getter); 0x004710F1: call 0x4F0FC0;
    // 0x004710F6: mov edx,[0x5BE348]; 0x004710FC: push eax; 0x004710FD: lea eax,[esp+0x30] (frame+0x24); 0x00471101: lea ecx,[esp+0xC]
    // (frame+0x00); 0x00471105: push eax; 0x00471106: push ecx; 0x00471107: push edx; 0x00471108: call 0x4F1A80;
    // 0x0047110D: add esp,0x90 - the 0x78 frame plus this append's six pushes; 0x00471113: ret.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5be348)),
                                append_frame,                                 // argument 2: retail frame+0x00
                                append_frame + 0x24,                          // argument 3: retail frame+0x24 (recorded difference 0x24)
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FC0()), // argument 4: the type from 0x4F0FC0
                                7, 0);                                           // argument 5: retail's leftover 7
} // end of Task_DefineBinocularOverlayAndHolder

} // extern "C"
