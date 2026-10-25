// Native: Task_DefineGunM16A2 (behavior-derived placeholder name; retail symbol unknown); retail VA 0x004733F0-0x004734CD.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 222 bytes, 1 block, 59 instructions.
// Tools/evidence: radare2 6.2.0 af/afbj/pdf with bin.relocs.apply over the exact image (1 block / 59 instructions / 222 bytes / 0 edges) plus
//   the objdump listing build/retail-dis.txt for the same extent (0x004733F0-0x004734CD; the ret is at 0x004734CD, two nops follow, and
//   0x004734D0 is the next function, which this body uses as a handler); `ps` at 0x00540448 and 0x00540434 for the two literals (byte-identical
//   to "GunM16A2" and "TASKTYPE_GUNM16A2"); `pxw` at 0x005BE36C for the image value; a full stack-pointer simulation over all 59 instructions,
//   which returns esp to its entry value at the ret and accounts for every cleanup (0x2c = 11 pushes for the two inert arguments, the
//   registration and the first two links; 0xc four times; 0x14 = the last link's three plus the tail's two). Every callee was re-read at its own
//   VA. Argument numbers were derived by counting the pushes above each access, not by reading an [esp+N] offset as an argument number.
// Behavior/CFG: one straight-line body that registers the "GunM16A2" task type of size 0x1B0 with parent Sys_GetGlobal16_477BF0() -> id at
//   0x005BE36C, then wires seven handler links: e0->0x004734D0 (0x00401400), e2->0x004754A0 (0x00401530), then the event ids from
//   0x00460BF0->0x00473510, 0x00487190->0x00473620, 0x00413B40->0x00473790, 0x00413B30->0x004737E0 and 0x004071D0->0x004737F0 (all
//   0x00401400), and finishes with the script-name tail ("TASKTYPE_GUNM16A2", GunM16A2 id) through 0x004B8810. One basic block, no branch,
//   no loop, no frame, no parameter container, 15 call sites over 10 distinct callees.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. The parent getter 0x00477BF0 is `mov ax, word [0x005BE3C6]` - the Gun type id that
//   0x00477E30 stores - so this type is parented onto Gun; its size 0x1B0 matches 0x00475380's ProximityMine and differs from 0x004739D0's
//   0x1B8, with all three sharing that parent, so the size is taken from each body's own push immediate. This body registers NO event 1: its
//   two 0x00401530/0x00401400 links use events 0 and 2 only, and that gap is transcribed rather than filled. Handler 0x004754A0 is now linked
//   as event 2 by THREE registrars seen so far - 0x00475380, 0x004739D0 and this body - so the same code serves several task types and a
//   handler identity does not identify an owner; all three call sites are transcribed as retail writes them. All six event-id links here go
//   through 0x00401400, unlike 0x004739D0 whose last link goes through 0x00401530 - the choice is per call site and is preserved. The id slot
//   0x005BE36C holds image value 0, is written with a 16-bit store and is read back at the tail with a 32-bit load masked by 0xFFFF, so it is
//   declared as a plain std::uint16_t; it is one of the image-zero slots in the 0x005BE3xx cluster. Documented shape differences, all
//   value-preserving: (1) every getter callee is an argument-less load in retail that ignores the arguments the caller pushes, and the tree's
//   transcriptions take no arguments, so those inert pushes are not reproduced; each is simultaneously the next 0x00401400/0x00401530 link
//   call's pre-pushed handler, which the port keeps as the handler value in the same order; (2) byte-valued event ids are zero-extended in the
//   port, which cannot change behaviour because every link fed by a byte getter here goes through 0x00401400 or 0x00401530, which mask the
//   event word for the phase index. Handlers 0x004734D0, 0x004754A0, 0x00473510, 0x00473620, 0x00473790, 0x004737E0 and 0x004737F0 are raw
//   code immediates in retail and are not transcribed yet (native gaps in 0x0046xxxx-0x0048xxxx); all seven are in this file's own range.
//   Domain: core/, because it is one of the 0x00402930 startup registrars.
#include "igi/retail/core/task_types.hpp"

#include <cstdint>

extern "C" std::uint16_t Sys_GetGlobal16_477BF0(void);            // 0x00477BF0: argument-less word getter (the Gun type id)
extern "C" std::uint8_t Task_GetCachedEventId_460BF0(void);       // 0x00460BF0: lazily allocated event id (core/native_00460bf0.cpp)
extern "C" std::uint8_t Sys_GetGlobal8_487190(void);              // 0x00487190: argument-less byte getter (core/native_487190.cpp)
extern "C" std::uint8_t Core_GetFlagByte_413B40(void);            // 0x00413B40: argument-less byte getter (core/native_413b40.cpp)
extern "C" std::uint8_t Core_GetFlagByte_413B30(void);            // 0x00413B30: argument-less byte getter (core/native_413b30.cpp)
extern "C" std::uint8_t Sys_GetSettingByte_4071D0(void);          // 0x004071D0: argument-less byte getter (sys/native_4071d0.cpp)
extern "C" void Script_QHashNodeRegister(void* arg1, void* value); // 0x004B8810 (script/native_4b8810.cpp)

extern "C" {

// 0x005BE36C: the "GunM16A2" task type id. Image value 0, overwritten by a 16-bit store from TaskType_Register's return value.
std::uint16_t g_5be36c = 0U;

void Task_DefineGunM16A2()
{
    // 0x004733F0/0x004733F2: push 0; push 0 (both inert arguments of the parent getter); 0x004733F4: call 0x477BF0; 0x004733F9: push eax;
    // 0x004733FA: push 0x1B0; 0x004733FF: push "GunM16A2"; 0x00473404: call 0x401900.
    const std::uint16_t gun_m16a2_id = TaskType_Register("GunM16A2", // 0x004733FF: push 0x00540448
                                                         0x1B0U,      // 0x004733FA: push 0x1B0 (object size)
                                                         Sys_GetGlobal16_477BF0(), // 0x004733F4: call 0x477BF0 (parent type)
                                                         0U);         // 0x004733F2: push 0 (inert)

    // 0x00473409: push 0x004734D0 (pre-pushed handler); 0x0047340E: push 0; 0x00473410: push eax;
    // 0x00473411: mov word [0x5BE36C],ax (the store lands between the pushes and the call); 0x00473417: call 0x401400.
    ::g_5be36c = gun_m16a2_id;
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be36c), // 0x0047341C: mov ax,[0x5BE36C]
                                0U,                                     // 0x0047340E: push 0 (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x004734D0U))); // 0x00473409: push handler

    // 0x00473422: push 0x004754A0 (the same handler 0x00475380 and 0x004739D0 link for their event 2); 0x00473427: push 2;
    // 0x00473429: push eax; 0x0047342A: call 0x401530; 0x0047342F: add esp,0x2c - the eleven pushes above. No event 1 link exists here.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_5be36c), // 0x0047341C: mov ax,[0x5BE36C]
                                           2U,                                     // 0x00473427: push 2 (argument 2)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x004754A0U))); // 0x00473422: push handler

    // 0x00473432: push 0x00473510; 0x00473437: call 0x460BF0; 0x0047343C: mov cx,[0x5BE36C]; 0x00473443: push eax; 0x00473444: push ecx;
    // 0x00473445: call 0x401400; 0x0047344A: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be36c), // 0x0047343C: mov cx,[0x5BE36C]
                                static_cast<std::uint32_t>(Task_GetCachedEventId_460BF0()), // 0x00473443: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00473510U))); // 0x00473432: push handler

    // 0x0047344D: push 0x00473620; 0x00473452: call 0x487190; 0x00473457: mov dx,[0x5BE36C]; 0x0047345E: push eax; 0x0047345F: push edx;
    // 0x00473460: call 0x401400; 0x00473465: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be36c), // 0x00473457: mov dx,[0x5BE36C]
                                static_cast<std::uint32_t>(Sys_GetGlobal8_487190()), // 0x0047345E: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00473620U))); // 0x0047344D: push handler

    // 0x00473468: push 0x00473790; 0x0047346D: call 0x413B40; 0x00473472: push eax; 0x00473473: mov ax,[0x5BE36C]; 0x00473479: push eax;
    // 0x0047347A: call 0x401400; 0x0047347F: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be36c), // 0x00473473: mov ax,[0x5BE36C]
                                static_cast<std::uint32_t>(Core_GetFlagByte_413B40()), // 0x00473472: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00473790U))); // 0x00473468: push handler

    // 0x00473482: push 0x004737E0; 0x00473487: call 0x413B30; 0x0047348C: mov cx,[0x5BE36C]; 0x00473493: push eax; 0x00473494: push ecx;
    // 0x00473495: call 0x401400; 0x0047349A: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be36c), // 0x0047348C: mov cx,[0x5BE36C]
                                static_cast<std::uint32_t>(Core_GetFlagByte_413B30()), // 0x00473493: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x004737E0U))); // 0x00473482: push handler

    // 0x0047349D: push 0x004737F0; 0x004734A2: call 0x4071D0; 0x004734A7: mov dx,[0x5BE36C]; 0x004734AE: push eax; 0x004734AF: push edx;
    // 0x004734B0: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be36c), // 0x004734A7: mov dx,[0x5BE36C]
                                static_cast<std::uint32_t>(Sys_GetSettingByte_4071D0()), // 0x004734AE: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x004737F0U))); // 0x0047349D: push handler

    // 0x004734B5: mov eax,[0x5BE36C] (a 32-bit load); 0x004734BA: and eax,0xFFFF; 0x004734BF: push eax;
    // 0x004734C0: push "TASKTYPE_GUNM16A2"; 0x004734C5: call 0x4B8810; 0x004734CA: add esp,0x14 - the last link's three pushes plus the
    // tail's two; 0x004734CD: ret.
    Script_QHashNodeRegister(const_cast<char*>("TASKTYPE_GUNM16A2"), // 0x004734C0: push 0x00540434 (argument 1)
                             reinterpret_cast<void*>(static_cast<std::uintptr_t>(
                                 static_cast<std::uint32_t>(::g_5be36c)))); // 0x004734BA: the id masked to 16 bits (argument 2)
} // end of Task_DefineGunM16A2

} // extern "C"
