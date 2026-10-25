// Native: Task_DefineGunDragunov (behavior-derived placeholder name; retail symbol unknown); retail VA 0x00472D00-0x00472E0A.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 267 bytes, 1 block, 68 instructions.
// Tools/evidence: radare2 6.2.0 af/afbj/pdf with bin.relocs.apply over the exact image (1 block / 68 instructions / 267 bytes / 0 edges) plus
//   the objdump listing build/retail-dis.txt for the same extent (0x00472D00-0x00472E0A; the ret is at 0x00472E0A, one nop follows, and
//   0x00472E10 is the next function, which this body uses as a handler); `ps` at 0x005403F8, 0x005403E0, 0x005403D0, 0x005403BC and 0x005403A8
//   for the five literals (byte-identical to "GunDRAGUNOV", "TASKTYPE_GUNDRAGUNOV", "snipersight.spr", "snipersight_x.spr" and
//   "snipersight_y.spr"); `pxw` at 0x005BE358, 0x005BE35C, 0x005BE360 and 0x005BE364 for the image values; a full stack-pointer simulation over
//   all 68 instructions, which returns esp to its entry value at the ret and accounts for every cleanup (0x2c = 11 pushes for the two inert
//   arguments, the registration and the first two links; 0xc four times; 0x20 = the last link's three pushes plus the 0x004B8810 tail's two
//   plus the three 0x004169D0 calls' three). Every callee was re-read at its own VA. Argument numbers were derived by counting the pushes above
//   each access, not by reading an [esp+N] offset as an argument number.
// Behavior/CFG: one straight-line body that registers the "GunDRAGUNOV" task type of size 0x1B8 with parent Sys_GetGlobal16_477BF0() -> id at
//   0x005BE358, then wires seven handler links: e0->0x00472E10 (0x00401400), e2->0x004754A0 (0x00401530), then the event ids from
//   0x00460BF0->0x00472E50, 0x00487190->0x00473040, 0x00413B40->0x004732F0, 0x00413B30->0x00427D70 and 0x004071D0->0x00473330 (all
//   0x00401400). It then registers the script name ("TASKTYPE_GUNDRAGUNOV", GunDRAGUNOV id) through 0x004B8810 and finishes with three record
//   lookups through 0x004169D0 whose results go to 0x005BE360 ("snipersight.spr"), 0x005BE364 ("snipersight_x.spr") and 0x005BE35C
//   ("snipersight_y.spr"). One basic block, no branch, no loop, no frame, no parameter container, 18 call sites over 11 distinct callees.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. The parent getter 0x00477BF0 is `mov ax, word [0x005BE3C6]` - the Gun type id that
//   0x00477E30 stores - so this type is parented onto Gun; its size 0x1B8 matches 0x004739D0 and differs from 0x00475380/0x004733F0's 0x1B0
//   with all four sharing that parent, so the size is taken from each body's own push immediate. This body registers NO event 1: its two
//   0x00401530/0x00401400 links use events 0 and 2 only, and that gap is transcribed rather than filled. Two of its handlers are shared with
//   other registrars, confirmed by searching the whole listing for the immediates rather than by reading this body alone: 0x004754A0 is
//   referenced four times in the image (0x00472D32 here, 0x00473422, 0x00473A02, 0x004753B2) and 0x004732F0 twice (0x00472D78 here and
//   0x00473A2D inside 0x004739D0). Both are transcribed as retail writes them; neither link implies ownership of the handler. The id slot
//   0x005BE358 holds image value 0, is written with a 16-bit store and is read back at the 0x004B8810 tail with a 32-bit load masked by 0xFFFF,
//   so it is declared as a plain std::uint16_t. 0x005BE35C, 0x005BE360 and 0x005BE364 are the three record pointers, all image value 0, written
//   and read as dwords, so they are declared as std::uint32_t. Documented shape differences, all value-preserving: (1) every getter callee is an
//   argument-less load in retail that ignores the arguments the caller pushes, and the tree's transcriptions take no arguments, so those inert
//   pushes are not reproduced; each is simultaneously the next 0x00401400/0x00401530 link call's pre-pushed handler, which the port keeps as
//   the handler value in the same order; (2) byte-valued event ids are zero-extended in the port, which cannot change behaviour because every
//   link fed by a byte getter here goes through 0x00401400 or 0x00401530, which mask the event word for the phase index; (3) retail stores the
//   first lookup result at 0x00472DE9, between the second argument's push and the second call, and the port performs the three lookups and their
//   stores in the same order. Handlers 0x00472E10, 0x004754A0, 0x00472E50, 0x00473040, 0x004732F0 and 0x00473330 are raw code immediates in
//   retail and are not transcribed yet (native gaps in 0x0046xxxx-0x0048xxxx); handler 0x00427D70 is BELOW 0x00460000 and is already
//   implemented by the 0x00400000-0x0045FFFF owner, not by this file. Domain: core/, because it is one of the 0x00402930 startup registrars.
#include "igi/retail/core/task_types.hpp"

#include <cstdint>

extern "C" std::uint16_t Sys_GetGlobal16_477BF0(void);            // 0x00477BF0: argument-less word getter (the Gun type id)
extern "C" std::uint8_t Task_GetCachedEventId_460BF0(void);       // 0x00460BF0: lazily allocated event id (core/native_00460bf0.cpp)
extern "C" std::uint8_t Sys_GetGlobal8_487190(void);              // 0x00487190: argument-less byte getter (core/native_487190.cpp)
extern "C" std::uint8_t Core_GetFlagByte_413B40(void);            // 0x00413B40: argument-less byte getter (core/native_413b40.cpp)
extern "C" std::uint8_t Core_GetFlagByte_413B30(void);            // 0x00413B30: argument-less byte getter (core/native_413b30.cpp)
extern "C" std::uint8_t Sys_GetSettingByte_4071D0(void);          // 0x004071D0: argument-less byte getter (sys/native_4071d0.cpp)
extern "C" void* Sys_LookupOrAppendRecord_4169D0(char const* name); // 0x004169D0: record lookup/append (core/native_4169d0.cpp)
extern "C" void Script_QHashNodeRegister(void* arg1, void* value); // 0x004B8810 (script/native_4b8810.cpp)

extern "C" {

// 0x005BE358: the "GunDRAGUNOV" task type id. Image value 0, overwritten by a 16-bit store from TaskType_Register's return value.
std::uint16_t g_5be358 = 0U;

// 0x005BE35C: the "snipersight_y.spr" record pointer. Image value 0, written and read as a dword.
std::uint32_t g_5be35c = 0U;

// 0x005BE360: the "snipersight.spr" record pointer. Image value 0, written and read as a dword.
std::uint32_t g_5be360 = 0U;

// 0x005BE364: the "snipersight_x.spr" record pointer. Image value 0, written and read as a dword.
std::uint32_t g_5be364 = 0U;

void Task_DefineGunDragunov()
{
    // 0x00472D00/0x00472D02: push 0; push 0 (both inert arguments of the parent getter); 0x00472D04: call 0x477BF0; 0x00472D09: push eax;
    // 0x00472D0A: push 0x1B8; 0x00472D0F: push "GunDRAGUNOV"; 0x00472D14: call 0x401900.
    const std::uint16_t gun_dragunov_id = TaskType_Register("GunDRAGUNOV", // 0x00472D0F: push 0x005403F8
                                                            0x1B8U,        // 0x00472D0A: push 0x1B8 (object size)
                                                            Sys_GetGlobal16_477BF0(), // 0x00472D04: call 0x477BF0 (parent type)
                                                            0U);           // 0x00472D02: push 0 (inert)

    // 0x00472D19: push 0x00472E10 (pre-pushed handler); 0x00472D1E: push 0; 0x00472D20: push eax;
    // 0x00472D21: mov word [0x5BE358],ax (the store lands between the pushes and the call); 0x00472D27: call 0x401400.
    ::g_5be358 = gun_dragunov_id;
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be358), // 0x00472D2C: mov ax,[0x5BE358]
                                0U,                                     // 0x00472D1E: push 0 (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00472E10U))); // 0x00472D19: push handler

    // 0x00472D32: push 0x004754A0 (the shared event-2 handler; the image references it at 0x00472D32, 0x00473422, 0x00473A02 and
    // 0x004753B2); 0x00472D37: push 2; 0x00472D39: push eax; 0x00472D3A: call 0x401530; 0x00472D3F: add esp,0x2c - the eleven pushes above.
    // No event 1 link exists in this body.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_5be358), // 0x00472D2C: mov ax,[0x5BE358]
                                           2U,                                     // 0x00472D37: push 2 (argument 2)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x004754A0U))); // 0x00472D32: push handler

    // 0x00472D42: push 0x00472E50; 0x00472D47: call 0x460BF0; 0x00472D4C: mov cx,[0x5BE358]; 0x00472D53: push eax; 0x00472D54: push ecx;
    // 0x00472D55: call 0x401400; 0x00472D5A: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be358), // 0x00472D4C: mov cx,[0x5BE358]
                                static_cast<std::uint32_t>(Task_GetCachedEventId_460BF0()), // 0x00472D53: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00472E50U))); // 0x00472D42: push handler

    // 0x00472D5D: push 0x00473040; 0x00472D62: call 0x487190; 0x00472D67: mov dx,[0x5BE358]; 0x00472D6E: push eax; 0x00472D6F: push edx;
    // 0x00472D70: call 0x401400; 0x00472D75: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be358), // 0x00472D67: mov dx,[0x5BE358]
                                static_cast<std::uint32_t>(Sys_GetGlobal8_487190()), // 0x00472D6E: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00473040U))); // 0x00472D5D: push handler

    // 0x00472D78: push 0x004732F0 (the handler 0x004739D0 also links, referenced twice in the image); 0x00472D7D: call 0x413B40;
    // 0x00472D82: push eax; 0x00472D83: mov ax,[0x5BE358]; 0x00472D89: push eax; 0x00472D8A: call 0x401400; 0x00472D8F: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be358), // 0x00472D83: mov ax,[0x5BE358]
                                static_cast<std::uint32_t>(Core_GetFlagByte_413B40()), // 0x00472D82: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x004732F0U))); // 0x00472D78: push handler

    // 0x00472D92: push 0x00427D70; 0x00472D97: call 0x413B30; 0x00472D9C: mov cx,[0x5BE358]; 0x00472DA3: push eax; 0x00472DA4: push ecx;
    // 0x00472DA5: call 0x401400; 0x00472DAA: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be358), // 0x00472D9C: mov cx,[0x5BE358]
                                static_cast<std::uint32_t>(Core_GetFlagByte_413B30()), // 0x00472DA3: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00427D70U))); // 0x00472D92: push handler (below 0x00460000)

    // 0x00472DAD: push 0x00473330; 0x00472DB2: call 0x4071D0; 0x00472DB7: mov dx,[0x5BE358]; 0x00472DBE: push eax; 0x00472DBF: push edx;
    // 0x00472DC0: call 0x401400. This link has no cleanup of its own.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be358), // 0x00472DB7: mov dx,[0x5BE358]
                                static_cast<std::uint32_t>(Sys_GetSettingByte_4071D0()), // 0x00472DBE: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00473330U))); // 0x00472DAD: push handler

    // 0x00472DC5: mov eax,[0x5BE358] (a 32-bit load); 0x00472DCA: and eax,0xFFFF; 0x00472DCF: push eax;
    // 0x00472DD0: push "TASKTYPE_GUNDRAGUNOV"; 0x00472DD5: call 0x4B8810.
    Script_QHashNodeRegister(const_cast<char*>("TASKTYPE_GUNDRAGUNOV"), // 0x00472DD0: push 0x005403E0 (argument 1)
                             reinterpret_cast<void*>(static_cast<std::uintptr_t>(
                                 static_cast<std::uint32_t>(::g_5be358)))); // 0x00472DCA: the id masked to 16 bits (argument 2)

    // 0x00472DDA: push "snipersight.spr"; 0x00472DDF: call 0x4169D0; 0x00472DE4: push "snipersight_x.spr";
    // 0x00472DE9: mov [0x5BE360],eax (the store of the first result lands between the second argument's push and the second call);
    // 0x00472DEE: call 0x4169D0; 0x00472DF3: push "snipersight_y.spr"; 0x00472DF8: mov [0x5BE364],eax; 0x00472DFD: call 0x4169D0;
    // 0x00472E02: add esp,0x20 - the last link's three pushes, this tail's two and the three lookups' three; 0x00472E05: mov [0x5BE35C],eax;
    // 0x00472E0A: ret.
    ::g_5be360 = reinterpret_cast<std::uint32_t>(Sys_LookupOrAppendRecord_4169D0("snipersight.spr"));   // 0x00472DDA, stored 0x00472DE9
    ::g_5be364 = reinterpret_cast<std::uint32_t>(Sys_LookupOrAppendRecord_4169D0("snipersight_x.spr")); // 0x00472DE4, stored 0x00472DF8
    ::g_5be35c = reinterpret_cast<std::uint32_t>(Sys_LookupOrAppendRecord_4169D0("snipersight_y.spr")); // 0x00472DF3, stored 0x00472E05
} // end of Task_DefineGunDragunov

} // extern "C"
