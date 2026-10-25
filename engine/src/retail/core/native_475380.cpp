// Native: Task_DefineProximityMine (behavior-derived placeholder name; retail symbol unknown); retail VA 0x00475380-0x0047545D.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 222 bytes, 1 block, 59 instructions.
// Tools/evidence: radare2 6.2.0 af/afbj/pdf with bin.relocs.apply over the exact image (1 block / 59 instructions / 222 bytes / 0 edges) plus
//   the objdump listing build/retail-dis.txt for the same extent (0x00475380-0x0047545D; the ret is at 0x0047545D, two nops follow, and
//   0x00475460 is the next function, which this body uses as a handler); `ps` at 0x00540554 and 0x0054053C for the two literals (byte-identical
//   to "ProximityMine" and "TASKTYPE_PROXIMITYMINE"); `pxw` at 0x005BE394 for the image value; a full stack-pointer simulation over all 59
//   instructions, which returns esp to its entry value at the ret and accounts for every cleanup (0x2c = 11 pushes for the two inert
//   arguments, the registration and the first two links; 0xc five times; 0x14 = the last link's three plus the tail's two). Every callee was
//   re-read at its own VA and each was compiled in isolation to confirm it is defined with C linkage (all six are unmangled `_Name`).
//   Argument numbers were derived by counting the pushes above each access, not by reading an [esp+N] offset as an argument number.
// Behavior/CFG: one straight-line body that registers the "ProximityMine" task type of size 0x1B0 with parent Sys_GetGlobal16_477BF0() -> id at
//   0x005BE394, then wires seven handler links: e0->0x00475460 (0x00401400), e2->0x004754A0 (0x00401530), then the event ids from
//   0x004F1FE0->0x004754C0, 0x00460BF0->0x004755A0, 0x00413B40->0x00475900, 0x00413B30->0x00429910 and 0x004071D0->0x00475960 (all
//   0x00401400), and finishes with the script-name tail ("TASKTYPE_PROXIMITYMINE", ProximityMine id) through 0x004B8810. One basic block, no
//   branch, no loop, no frame, no parameter container, 15 call sites over 10 distinct callees.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. The registered size 0x1B0 equals the size 0x00477E30 registers for "Gun", and this body's
//   parent getter 0x00477BF0 is `mov ax, word [0x005BE3C6]` - the Gun type id that 0x00477E30 stores - so this type is parented onto Gun and
//   shares its object size. This body registers NO event 1: the two 0x00401530/0x00401400 links it makes use events 0 and 2 only, and that
//   gap is transcribed rather than filled. The id slot 0x005BE394 holds image value 0, is written with a 16-bit store and is read back at the
//   tail with a 32-bit load masked by 0xFFFF, so it is declared as a plain std::uint16_t; it is one of the seven image-zero slots in the
//   0x005BE3xx cluster (0x005BE394, 0x005BE39C, 0x005BE3A0, 0x005BE3A8, 0x005BE3B8, 0x005BE3C0, 0x005BE3C4, 0x005BE3C6), which is the
//   counter-witness family to the 0x0181 values seen in the 0x005406xx/0x005407xx/0x005486xx/0x00547Fxx slots. Documented shape differences,
//   all value-preserving: (1) every getter callee is an argument-less load in retail that ignores the arguments the caller pushes, and the
//   tree's transcriptions take no arguments, so those inert pushes are not reproduced; each is simultaneously the next 0x00401400 link call's
//   pre-pushed handler, which the port keeps as the handler value in the same order; (2) byte-valued event ids are zero-extended in the port,
//   which cannot change behaviour because every link fed by a byte getter here goes through 0x00401400, which masks the event word to its low
//   byte for the phase index. Handlers 0x00475460, 0x004754A0, 0x004754C0, 0x004755A0, 0x00475900 and 0x00475960 are raw code immediates in
//   retail and are not transcribed yet (native gaps in 0x0046xxxx-0x0048xxxx). Handler 0x00429910 is BELOW 0x00460000 and therefore belongs
//   to the 0x00400000-0x0045FFFF owner, not to this file. Domain: core/, because it is one of the 0x00402930 startup registrars.
#include "igi/retail/core/task_types.hpp"

#include <cstdint>

extern "C" std::uint16_t Sys_GetGlobal16_477BF0(void);             // 0x00477BF0: argument-less word getter (the Gun type id)
extern "C" std::uint8_t Sys_GetGlobal8_4F1FE0(void);               // 0x004F1FE0: argument-less byte getter (sys/native_4f1fe0.cpp)
extern "C" std::uint8_t Task_GetCachedEventId_460BF0(void);        // 0x00460BF0: lazily allocated event id (core/native_00460bf0.cpp)
extern "C" std::uint8_t Core_GetFlagByte_413B40(void);             // 0x00413B40: argument-less byte getter (core/native_413b40.cpp)
extern "C" std::uint8_t Core_GetFlagByte_413B30(void);             // 0x00413B30: argument-less byte getter (core/native_413b30.cpp)
extern "C" std::uint8_t Sys_GetSettingByte_4071D0(void);           // 0x004071D0: argument-less byte getter (sys/native_4071d0.cpp)
extern "C" void Script_QHashNodeRegister(void* arg1, void* value); // 0x004B8810 (script/native_4b8810.cpp)

extern "C" {

// 0x005BE394: the "ProximityMine" task type id. Image value 0, overwritten by a 16-bit store from TaskType_Register's return value.
std::uint16_t g_5be394 = 0U;

void Task_DefineProximityMine()
{
    // 0x00475380/0x00475382: push 0; push 0 (both inert arguments of the parent getter); 0x00475384: call 0x477BF0; 0x00475389: push eax;
    // 0x0047538A: push 0x1B0; 0x0047538F: push "ProximityMine"; 0x00475394: call 0x401900.
    const std::uint16_t proximity_mine_id = TaskType_Register("ProximityMine", // 0x0047538F: push 0x00540554
                                                              0x1B0U,           // 0x0047538A: push 0x1B0 (object size, the Gun size)
                                                              Sys_GetGlobal16_477BF0(), // 0x00475384: call 0x477BF0 (parent type)
                                                              0U);              // 0x00475382: push 0 (inert)

    // 0x00475399: push 0x00475460 (pre-pushed handler); 0x0047539E: push 0; 0x004753A0: push eax;
    // 0x004753A1: mov word [0x5BE394],ax (the store lands between the pushes and the call); 0x004753A7: call 0x401400.
    ::g_5be394 = proximity_mine_id;
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be394), // 0x004753AC: mov ax,[0x5BE394]
                                0U,                                     // 0x0047539E: push 0 (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00475460U))); // 0x00475399: push handler

    // 0x004753B2: push 0x004754A0; 0x004753B7: push 2; 0x004753B9: push eax; 0x004753BA: call 0x401530; 0x004753BF: add esp,0x2c - the
    // eleven pushes above (two inert, three registration, three per link). No event 1 link exists in this body.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_5be394), // 0x004753AC: mov ax,[0x5BE394]
                                           2U,                                     // 0x004753B7: push 2 (argument 2)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x004754A0U))); // 0x004753B2: push handler

    // 0x004753C2: push 0x004754C0; 0x004753C7: call 0x4F1FE0; 0x004753CC: mov cx,[0x5BE394]; 0x004753D3: push eax; 0x004753D4: push ecx;
    // 0x004753D5: call 0x401400; 0x004753DA: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be394), // 0x004753CC: mov cx,[0x5BE394]
                                static_cast<std::uint32_t>(Sys_GetGlobal8_4F1FE0()), // 0x004753D3: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x004754C0U))); // 0x004753C2: push handler

    // 0x004753DD: push 0x004755A0; 0x004753E2: call 0x460BF0; 0x004753E7: mov dx,[0x5BE394]; 0x004753EE: push eax; 0x004753EF: push edx;
    // 0x004753F0: call 0x401400; 0x004753F5: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be394), // 0x004753E7: mov dx,[0x5BE394]
                                static_cast<std::uint32_t>(Task_GetCachedEventId_460BF0()), // 0x004753EE: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x004755A0U))); // 0x004753DD: push handler

    // 0x004753F8: push 0x00475900; 0x004753FD: call 0x413B40; 0x00475402: push eax; 0x00475403: mov ax,[0x5BE394]; 0x00475409: push eax;
    // 0x0047540A: call 0x401400; 0x0047540F: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be394), // 0x00475403: mov ax,[0x5BE394]
                                static_cast<std::uint32_t>(Core_GetFlagByte_413B40()), // 0x00475402: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00475900U))); // 0x004753F8: push handler

    // 0x00475412: push 0x00429910; 0x00475417: call 0x413B30; 0x0047541C: mov cx,[0x5BE394]; 0x00475423: push eax; 0x00475424: push ecx;
    // 0x00475425: call 0x401400; 0x0047542A: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be394), // 0x0047541C: mov cx,[0x5BE394]
                                static_cast<std::uint32_t>(Core_GetFlagByte_413B30()), // 0x00475423: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00429910U))); // 0x00475412: push handler (below 0x00460000)

    // 0x0047542D: push 0x00475960; 0x00475432: call 0x4071D0; 0x00475437: mov dx,[0x5BE394]; 0x0047543E: push eax; 0x0047543F: push edx;
    // 0x00475440: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be394), // 0x00475437: mov dx,[0x5BE394]
                                static_cast<std::uint32_t>(Sys_GetSettingByte_4071D0()), // 0x0047543E: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00475960U))); // 0x0047542D: push handler

    // 0x00475445: mov eax,[0x5BE394] (a 32-bit load); 0x0047544A: and eax,0xFFFF; 0x0047544F: push eax;
    // 0x00475450: push "TASKTYPE_PROXIMITYMINE"; 0x00475455: call 0x4B8810; 0x0047545A: add esp,0x14 - the last link's three pushes plus
    // the tail's two; 0x0047545D: ret.
    Script_QHashNodeRegister(const_cast<char*>("TASKTYPE_PROXIMITYMINE"), // 0x00475450: push 0x0054053C (argument 1)
                             reinterpret_cast<void*>(static_cast<std::uintptr_t>(
                                 static_cast<std::uint32_t>(::g_5be394)))); // 0x0047544A: the id masked to 16 bits (argument 2)
} // end of Task_DefineProximityMine

} // extern "C"
