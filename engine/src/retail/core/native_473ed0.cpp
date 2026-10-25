// Native: Task_DefineStationaryGun (behavior-derived placeholder name; retail symbol unknown); retail VA 0x00473ED0-0x00474186.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 695 bytes, 1 block, 194 instructions.
// Tools/evidence: radare2 6.2.0 af/afbj/pdf with bin.relocs.apply over the exact image (1 block / 194 instructions / 695 bytes / 0 edges) plus
//   the objdump listing build/retail-dis.txt for the same extent (0x00473ED0-0x00474186; the ret is at 0x00474186, three nops follow, and
//   0x00474190 is the next function, which this body uses as a handler); `ps` at 0x0054052C for the type name (byte-identical to
//   "StationaryGun"); `pxw` at 0x005BE380, 0x005BE384, 0x005BE388 and 0x005BE38C for the image values (all zero; 0x005BE384 is written
//   0x186A0 by this body); a full stack-pointer simulation over all 194 instructions, which returns esp to its entry value at the ret and
//   accounts for every cleanup (the 0x220 frame, 0x38 = 14 pushes for the two inert arguments plus the registration and the first three links,
//   0xc eleven times, 0x18 twice - once for the two links that share a cleanup and once per append - 0x10 for one link plus the allocator
//   argument, and 0x238 = the 0x220 frame plus the last append's six pushes). Every callee was re-read at its own VA; 0x004F0FA0 and
//   0x004F0FD0 were disassembled in full to confirm they are argument-less (`mov eax,[abs]` and a ret, zero esp references), which is what
//   makes the two pushes in front of each of those calls inert. Argument numbers were derived by counting the pushes above each access.
// Behavior/CFG: one straight-line body with a 0x220-byte frame - the same size it registers - that registers the "StationaryGun" task type with
//   parent Sys_GetGlobal16_4C48C0() -> id at 0x005BE388, then wires twelve handler links: e0->0x00473D30 (0x00401400), e1->0x00474190 and
//   e2->0x00474960 (0x00401530), then the event ids from 0x004F1A70->0x00474980, 0x004167F0->0x00474E30, 0x00416810->0x00474ED0,
//   0x00416800->0x00474EE0, 0x00416820->0x00474F10, 0x00416890->0x00474F30, 0x004F1A60->0x00474F50 and 0x004C5800->0x00474F60, then the
//   literal pair (handler 0x004D3710, event 6), then 0x00489610->0x004752B0. It then zeroes 0x005BE380, stores 0x186A0 in 0x005BE384,
//   allocates an 8-entry parameter container into 0x005BE38C through 0x004F1950, and appends EIGHT parameter records to it through
//   0x004F1A80 (types from 0x004F1010, 0x004F0F70, 0x004F0FD0, 0x004F0FE0, 0x004F0F30, 0x004F0F30, 0x004F0F30 and 0x004F0FA0; recorded
//   deltas 0xF0, 0x108, 0x1B0, 0x1C0, 0x1E0, 0x1E4, 0x1E8 and 0x1EC against frame+0x00). One basic block, no branch, no loop, 41 call
//   sites over 21 distinct callees. There is NO 0x004B8810 script-name tail in this body - it ends at the eighth append.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. Documented shape differences, all value-preserving: (1) retail's 0x220-byte frame is
//   reproduced as a single local slot whose address stands in for the frame base; the eight 0x004F1A80 calls pass frame+0x00 and
//   0x004F1A80's second argument, exactly the addresses retail computes, and nothing reads the intervening bytes; (2) every getter callee is an
//   argument-less load in retail that ignores the arguments the caller pushes, and the tree's transcriptions take no arguments, so those inert
//   pushes are not reproduced - for the seven link getters the inert argument is simultaneously the next 0x00401400 link's pre-pushed handler,
//   which the port keeps as the handler value in the same order, and for the eighth append's 0x004F0FA0 call the two inert pushes are
//   `push 0x005BE380` and `push 7`; (3) byte-valued event ids are zero-extended in the port, which cannot change behaviour because every link
//   fed by a byte getter here goes through 0x00401400, which masks the event word to its low byte for the phase index; (4) the eighth append's
//   sixth argument is retail's address 0x005BE380, which the port passes as the port address of the global that mirrors it (`&::g_5be380`)
//   instead of the retail constant, because 0x004F1A80 stores that argument as a value in the parameter record and a retail address there would
//   leave a pointer into unmapped retail .data inside the record. The id slot 0x005BE388
//   holds image value 0, is written with a 16-bit store and is read back as a word; the two bytes at 0x005BE389-0x005BE38B are referenced
//   nowhere in build/retail-dis.txt and are NOT modelled here. 0x005BE380, 0x005BE384 and 0x005BE38C are written and read as dwords, so they
//   are declared as std::uint32_t. The order of the 0x005BE380/0x005BE384 stores relative to the 0x004F1950 call is retail's
//   (0x0047402F and 0x00474039 precede 0x00474043) and is preserved. Handlers 0x00473D30, 0x00474190, 0x00474960, 0x00474980, 0x00474E30,
//   0x00474ED0, 0x00474EE0, 0x00474F10, 0x00474F30, 0x00474F50, 0x00474F60 and 0x004752B0 are raw code immediates in retail and are not
//   transcribed yet (native gaps in 0x0046xxxx-0x0048xxxx); handler 0x004D3710 is above 0x0048FFFF and is already implemented elsewhere.
//   Domain: core/, because it is one of the 0x00402930 startup registrars.
#include "igi/retail/core/task_types.hpp"

#include <cstdint>

extern "C" std::uint16_t Sys_GetGlobal16_4C48C0(void);             // 0x004C48C0: argument-less word getter (parent type)
extern "C" std::uint8_t Sys_GetSettingByte_4167F0(void);           // 0x004167F0: argument-less byte getter
extern "C" std::uint8_t Sys_GetSettingByte_416810(void);           // 0x00416810: argument-less byte getter
extern "C" std::uint8_t Sys_GetSettingByte_416800(void);           // 0x00416800: argument-less byte getter
extern "C" std::uint8_t Sys_GetSettingByte_416820(void);           // 0x00416820: argument-less byte getter
extern "C" std::uint8_t Sys_GetSettingByte_416890(void);           // 0x00416890: argument-less byte getter
extern "C" std::uint8_t DynObj_GetByte_4C5800(void);               // 0x004C5800: argument-less byte getter (dynobj/native_4c5800.cpp)
extern "C" std::uint8_t Sys_GetGlobal8_489610(void);               // 0x00489610: argument-less byte getter (core/native_489610.cpp)
extern "C" std::uint32_t Sys_GetGlobal32_4F1010(void);             // 0x004F1010: argument-less dword getter (the appended type)
extern "C" std::uint32_t Sys_GetGlobal32_4F0F70(void);             // 0x004F0F70: argument-less dword getter (the appended type)
extern "C" std::uint32_t Core_GetRenderWordB_4F0FD0(void);         // 0x004F0FD0: argument-less dword getter (core/native_4f0fd0.cpp)
extern "C" std::uint32_t Sys_GetGlobal32_4F0FE0(void);             // 0x004F0FE0: argument-less dword getter (the appended type)
extern "C" std::uint32_t Sys_GetGlobal32_4F0F30(void);             // 0x004F0F30: argument-less dword getter (the appended type)
extern "C" std::uint32_t Core_GetRenderWordA_4F0FA0(void);         // 0x004F0FA0: argument-less dword getter (core/native_4f0fa0.cpp)
extern "C" std::uint8_t Script_PhaseIdGetE4_4F1A60(void);          // 0x004F1A60: argument-less byte getter (script_state.hpp)
extern "C" std::uint8_t Script_PhaseIdGetE5_4F1A70(void);          // 0x004F1A70: argument-less byte getter (script_state.hpp)
extern "C" void* Script_TaskContainer_Alloc(std::int32_t count);   // 0x004F1950 (script/native_4f1950.cpp)
extern "C" void* Script_TaskContainer_Append(std::int32_t* const container, std::int32_t const arg2,
                                             std::int32_t const arg3, std::int32_t const arg4,
                                             std::int32_t const arg5, std::int32_t const arg6);
// 0x004F1A80 (script/native_4f1a80.cpp). SIX arguments, corrected: the callee reads arg1..arg4, arg5 as a byte (recorded at record +0x0C)
// and arg6 as a dword (recorded at record +0x08). The two reads are both `[esp+0x20]` in retail, at different push depths, with a `pop edi`
// between them (0x004F1AB1 with twelve bytes pushed = arg5, 0x004F1ACA with eight = arg6). Earlier revisions of this file declared five
// parameters and the tree's implementation wrote arg5 into both fields; both are now corrected. Every append below passes the sixth word that
// retail pushes for it. // 0x004F1A80 (script/native_4f1a80.cpp)

extern "C" {

// 0x005BE380: the first word pair handed to 0x004F0FA0 by the eighth append. Image value 0, written and read as a dword.
std::uint32_t g_5be380 = 0U;

// 0x005BE384: the second word of that pair. Image value 0, written 0x186A0 by this body.
std::uint32_t g_5be384 = 0U;

// 0x005BE388: the "StationaryGun" task type id. Image value 0, overwritten by a 16-bit store from TaskType_Register's return value.
std::uint16_t g_5be388 = 0U;

// 0x005BE38C: the 8-entry parameter container. Image value 0, written and read as a dword.
std::uint32_t g_5be38c = 0U;

void Task_DefineStationaryGun()
{
    // 0x00473ED0: sub esp,0x220 - retail's 544-byte frame, the same size it passes to TaskType_Register as the object size. Each of the
    // eight 0x004F1A80 calls below passes two addresses inside it: its first dword (every `lea ecx,[esp+0xc]` runs with 12 bytes of arguments
    // already pushed, so the address is frame+0x00) and one later slot. 0x004F1A80 reads those two arguments at [esp+8]/[esp+0xC],
    // subtracts them at 0x004F1A9D and stores the difference, so the eight recorded deltas are 0xF0, 0x108, 0x1B0, 0x1C0, 0x1E0, 0x1E4,
    // 0x1E8 and 0x1EC.
    std::int32_t append_frame_base = 0; // retail frame slot at +0x00
    const std::int32_t append_frame = reinterpret_cast<std::int32_t>(&append_frame_base);

    // 0x00473ED6/0x00473ED8: push 0; push 0 (both inert arguments of the parent getter); 0x00473EDA: call 0x4C48C0; 0x00473EDF: push eax;
    // 0x00473EE0: push 0x220; 0x00473EE5: push "StationaryGun"; 0x00473EEA: call 0x401900.
    const std::uint16_t stationary_gun_id = TaskType_Register("StationaryGun", // 0x00473EE5: push 0x0054052C
                                                              0x220U,           // 0x00473EE0: push 0x220 (object size)
                                                              Sys_GetGlobal16_4C48C0(), // 0x00473EDA: call 0x4C48C0 (parent type)
                                                              0U);              // 0x00473ED8: push 0 (inert)

    // 0x00473EEF: push 0x00473D30 (pre-pushed handler); 0x00473EF4: push 0; 0x00473EF6: push eax;
    // 0x00473EF7: mov word [0x5BE388],ax (the store lands between the pushes and the call); 0x00473EFD: call 0x401400.
    ::g_5be388 = stationary_gun_id;
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be388), // 0x00473F02: mov ax,[0x5BE388]
                                0U,                                     // 0x00473EF4: push 0 (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00473D30U))); // 0x00473EEF: push handler

    // 0x00473F08: push 0x00474190; 0x00473F0D: push 1; 0x00473F0F: push eax; 0x00473F10: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_5be388), // 0x00473F02: mov ax,[0x5BE388]
                                           1U,                                     // 0x00473F0D: push 1 (argument 2)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x00474190U))); // 0x00473F08: push handler

    // 0x00473F15: mov cx,[0x5BE388]; 0x00473F1C: push 0x00474960; 0x00473F21: push 2; 0x00473F23: push ecx; 0x00473F24: call 0x401530;
    // 0x00473F29: add esp,0x38 - the fourteen pushes above (two inert, three registration, three per link).
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_5be388), // 0x00473F15: mov cx,[0x5BE388]
                                           2U,                                     // 0x00473F21: push 2 (argument 2)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x00474960U))); // 0x00473F1C: push handler

    // 0x00473F2C: push 0x00474980; 0x00473F31: call 0x4F1A70; 0x00473F36: mov dx,[0x5BE388]; 0x00473F3D: push eax; 0x00473F3E: push edx;
    // 0x00473F3F: call 0x401400; 0x00473F44: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be388), // 0x00473F36: mov dx,[0x5BE388]
                                static_cast<std::uint32_t>(Script_PhaseIdGetE5_4F1A70()), // 0x00473F3D: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00474980U))); // 0x00473F2C: push handler

    // 0x00473F47: push 0x00474E30; 0x00473F4C: call 0x4167F0; 0x00473F51: push eax; 0x00473F52: mov ax,[0x5BE388]; 0x00473F58: push eax;
    // 0x00473F59: call 0x401400; 0x00473F5E: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be388), // 0x00473F52: mov ax,[0x5BE388]
                                static_cast<std::uint32_t>(Sys_GetSettingByte_4167F0()), // 0x00473F51: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00474E30U))); // 0x00473F47: push handler

    // 0x00473F61: push 0x00474ED0; 0x00473F66: call 0x416810; 0x00473F6B: mov cx,[0x5BE388]; 0x00473F72: push eax; 0x00473F73: push ecx;
    // 0x00473F74: call 0x401400; 0x00473F79: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be388), // 0x00473F6B: mov cx,[0x5BE388]
                                static_cast<std::uint32_t>(Sys_GetSettingByte_416810()), // 0x00473F72: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00474ED0U))); // 0x00473F61: push handler

    // 0x00473F7C: push 0x00474EE0; 0x00473F81: call 0x416800; 0x00473F86: mov dx,[0x5BE388]; 0x00473F8D: push eax; 0x00473F8E: push edx;
    // 0x00473F8F: call 0x401400; 0x00473F94: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be388), // 0x00473F86: mov dx,[0x5BE388]
                                static_cast<std::uint32_t>(Sys_GetSettingByte_416800()), // 0x00473F8D: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00474EE0U))); // 0x00473F7C: push handler

    // 0x00473F97: push 0x00474F10; 0x00473F9C: call 0x416820; 0x00473FA1: push eax; 0x00473FA2: mov ax,[0x5BE388]; 0x00473FA8: push eax;
    // 0x00473FA9: call 0x401400; 0x00473FAE: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be388), // 0x00473FA2: mov ax,[0x5BE388]
                                static_cast<std::uint32_t>(Sys_GetSettingByte_416820()), // 0x00473FA1: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00474F10U))); // 0x00473F97: push handler

    // 0x00473FB1: push 0x00474F30; 0x00473FB6: call 0x416890; 0x00473FBB: mov cx,[0x5BE388]; 0x00473FC2: push eax; 0x00473FC3: push ecx;
    // 0x00473FC4: call 0x401400; 0x00473FC9: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be388), // 0x00473FBB: mov cx,[0x5BE388]
                                static_cast<std::uint32_t>(Sys_GetSettingByte_416890()), // 0x00473FC2: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00474F30U))); // 0x00473FB1: push handler

    // 0x00473FCC: push 0x00474F50; 0x00473FD1: call 0x4F1A60; 0x00473FD6: mov dx,[0x5BE388]; 0x00473FDD: push eax; 0x00473FDE: push edx;
    // 0x00473FDF: call 0x401400; 0x00473FE4: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be388), // 0x00473FD6: mov dx,[0x5BE388]
                                static_cast<std::uint32_t>(Script_PhaseIdGetE4_4F1A60()), // 0x00473FDD: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00474F50U))); // 0x00473FCC: push handler

    // 0x00473FE7: push 0x00474F60; 0x00473FEC: call 0x4C5800; 0x00473FF1: push eax; 0x00473FF2: mov ax,[0x5BE388]; 0x00473FF8: push eax;
    // 0x00473FF9: call 0x401400. This link has no cleanup of its own.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be388), // 0x00473FF2: mov ax,[0x5BE388]
                                static_cast<std::uint32_t>(DynObj_GetByte_4C5800()), // 0x00473FF1: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00474F60U))); // 0x00473FE7: push handler

    // 0x00473FFE: push 0x004D3710; 0x00474003: push 6; 0x00474005: mov cx,[0x5BE388]; 0x0047400C: push ecx; 0x0047400D: call 0x401400;
    // 0x00474012: add esp,0x18 - the two links' six pushes share this single cleanup.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be388), // 0x00474005: mov cx,[0x5BE388]
                                6U,                                     // 0x00474003: push 6 (argument 2: a literal event id)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x004D3710U))); // 0x00473FFE: push handler

    // 0x00474015: push 0x004752B0; 0x0047401A: call 0x489610; 0x0047401F: mov dx,[0x5BE388]; 0x00474026: push eax; 0x00474027: push edx;
    // 0x00474028: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be388), // 0x0047401F: mov dx,[0x5BE388]
                                static_cast<std::uint32_t>(Sys_GetGlobal8_489610()), // 0x00474026: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x004752B0U))); // 0x00474015: push handler

    // 0x0047402D: push 8 (Script_TaskContainer_Alloc's argument); 0x0047402F: mov dword [0x5BE380],0;
    // 0x00474039: mov dword [0x5BE384],0x186A0; 0x00474043: call 0x4F1950; 0x00474048: add esp,0x10 - the link's three pushes plus the
    // allocator argument; 0x0047404B: mov [0x5BE38C],eax. The two global stores precede the call in retail and keep that order here.
    ::g_5be380 = 0U;                                                       // 0x0047402F: mov dword [0x5BE380],0
    ::g_5be384 = 0x186A0U;                                                 // 0x00474039: mov dword [0x5BE384],0x186A0
    ::g_5be38c = reinterpret_cast<std::uint32_t>(Script_TaskContainer_Alloc(8)); // 0x00474043: call 0x4F1950

    // Append 1: 0x00474050/0x00474052: push 0/7 (inert); 0x00474054: call 0x4F1010; 0x00474059: mov edx,[0x5BE38C]; 0x0047405F: push eax;
    // 0x00474060: lea eax,[esp+0xFC] (frame+0xF0); 0x00474067: lea ecx,[esp+0xC] (frame+0x00); 0x0047406B: push eax; 0x0047406C: push ecx;
    // 0x0047406D: push edx; 0x0047406E: call 0x4F1A80; 0x00474073: add esp,0x18.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5be38c)),
                                append_frame,                                 // argument 2: retail frame+0x00
                                append_frame + 0xF0,                          // argument 3: retail frame+0xF0 (recorded difference 0xF0)
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F1010()), // argument 4: the type from 0x4F1010
                                7, 0);                                           // argument 5: retail's leftover 7

    // Append 2: 0x00474076/0x00474078: push 0/7; 0x0047407A: call 0x4F0F70; 0x0047407F: mov edx,[0x5BE38C]; 0x00474085: push eax;
    // 0x00474086: lea eax,[esp+0x114] (frame+0x108); 0x0047408D: lea ecx,[esp+0xC]; 0x00474094: call 0x4F1A80; 0x00474099: add esp,0x18.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5be38c)),
                                append_frame,
                                append_frame + 0x108,                         // 0x00474086: frame+0x108
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F70()),
                                7, 0);

    // Append 3: 0x0047409C/0x0047409E: push 0/7; 0x004740A0: call 0x4F0FD0; 0x004740A5: mov edx,[0x5BE38C]; 0x004740AB: push eax;
    // 0x004740AC: lea eax,[esp+0x1BC] (frame+0x1B0); 0x004740B3: lea ecx,[esp+0xC]; 0x004740BA: call 0x4F1A80; 0x004740BF: add esp,0x18.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5be38c)),
                                append_frame,
                                append_frame + 0x1B0,                         // 0x004740AC: frame+0x1B0
                                static_cast<std::int32_t>(Core_GetRenderWordB_4F0FD0()),
                                7, 0);

    // Append 4: 0x004740C2/0x004740C4: push 0/7; 0x004740C6: call 0x4F0FE0; 0x004740CB: mov edx,[0x5BE38C]; 0x004740D1: push eax;
    // 0x004740D2: lea eax,[esp+0x1CC] (frame+0x1C0); 0x004740D9: lea ecx,[esp+0xC]; 0x004740E0: call 0x4F1A80; 0x004740E5: add esp,0x18.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5be38c)),
                                append_frame,
                                append_frame + 0x1C0,                         // 0x004740D2: frame+0x1C0
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FE0()),
                                7, 0);

    // Append 5: 0x004740E8/0x004740EA: push 0/7; 0x004740EC: call 0x4F0F30; 0x004740F1: mov edx,[0x5BE38C]; 0x004740F7: push eax;
    // 0x004740F8: lea eax,[esp+0x1EC] (frame+0x1E0); 0x004740FF: lea ecx,[esp+0xC]; 0x00474106: call 0x4F1A80; 0x0047410B: add esp,0x18.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5be38c)),
                                append_frame,
                                append_frame + 0x1E0,                         // 0x004740F8: frame+0x1E0
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F30()),
                                7, 0);

    // Append 6: 0x0047410E/0x00474110: push 0/7; 0x00474112: call 0x4F0F30 (the same getter as append 5); 0x00474117: push eax;
    // 0x00474118: lea eax,[esp+0x1F0] (frame+0x1E4); 0x0047411F: mov edx,[0x5BE38C]; 0x00474125: lea ecx,[esp+0xC]; 0x0047412C: call
    // 0x4F1A80; 0x00474131: add esp,0x18. Retail loads the container between the two leas here; that ordering is unobservable.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5be38c)),
                                append_frame,
                                append_frame + 0x1E4,                         // 0x00474118: frame+0x1E4
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F30()),
                                7, 0);

    // Append 7: 0x00474134/0x00474136: push 0/7; 0x00474138: call 0x4F0F30 (the same getter again); 0x0047413D: mov edx,[0x5BE38C];
    // 0x00474143: push eax; 0x00474144: lea eax,[esp+0x1F4] (frame+0x1E8); 0x0047414B: lea ecx,[esp+0xC]; 0x00474152: call 0x4F1A80;
    // 0x00474157: add esp,0x18.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5be38c)),
                                append_frame,
                                append_frame + 0x1E8,                         // 0x00474144: frame+0x1E8
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F30()),
                                7, 0);

    // Append 8: 0x0047415A: push 0x005BE380 (inert argument of the getter); 0x0047415F: push 7; 0x00474161: call 0x4F0FA0 (which reads
    // neither); 0x00474166: mov edx,[0x5BE38C]; 0x0047416C: push eax; 0x0047416D: lea eax,[esp+0x1F8] (frame+0x1EC); 0x00474174: lea
    // ecx,[esp+0xC]; 0x0047417B: call 0x4F1A80. This append has no cleanup of its own: 0x00474180's add esp,0x238 covers its six pushes and
    // the frame.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5be38c)),
                                append_frame,
                                append_frame + 0x1EC,                         // 0x0047416D: frame+0x1EC
                                static_cast<std::int32_t>(Core_GetRenderWordA_4F0FA0()),
                                7, static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(&::g_5be380))); // port address of retail 0x005BE380

    // 0x00474180: add esp,0x238; 0x00474186: ret. No 0x004B8810 script-name tail exists in this body.
} // end of Task_DefineStationaryGun

} // extern "C"
