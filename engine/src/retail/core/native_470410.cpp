// Native: Task_DefineMedipack (behavior-derived placeholder name; retail symbol unknown); retail VA 0x00470410-0x004704F4.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 229 bytes, 1 block, 61 instructions.
// Tools/evidence: radare2 6.2.0 af/afbj/pdf with bin.relocs.apply over the exact image (1 block / 61 instructions / 229 bytes / 0 edges) plus
//   the objdump listing build/retail-dis.txt for the same extent (0x00470410-0x004704F4; the ret is at 0x004704F4, eleven nops follow, and
//   0x00470500 is the next function, which this body uses as a handler); `ps` at 0x0053FF04 and 0x0053FEF0 for the two literals (byte-identical
//   to "Medipack" and "TASKTYPE_MEDIPACK"); `pxw` at 0x005BE334 for the image value; a full stack-pointer simulation over all 61 instructions,
//   which returns esp to its entry value at the ret and accounts for every cleanup (0x20 = 8 pushes for the two inert arguments, the
//   registration and the single event-0 link; 0xc five times; 0x14 once = the last link's three pushes plus the tail's two). Every callee was
//   re-read at its own VA. Argument numbers were derived by counting the pushes above each access, not by reading an [esp+N] offset as an
//   argument number.
// Behavior/CFG: one straight-line body that registers the "Medipack" task type of size 0x1B0 with parent Sys_GetGlobal16_477BF0() -> id at
//   0x005BE334, then wires six handler links, ALL of them through the context variant 0x00401400: e0->0x00470500, then the event ids from
//   0x004F1FE0->0x004754C0, 0x00460BF0->0x004D3710, 0x00460C10->0x004D3710, 0x00413B40->0x00470540 and 0x00413B30->0x00427D70, and
//   0x004071D0->0x00470570. It finishes with the script-name tail ("TASKTYPE_MEDIPACK", Medipack id) through 0x004B8810. One basic block, no
//   branch, no loop, no frame, no parameter container, 16 call sites over 10 distinct callees.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. This is the only registrar in this lane so far that calls NEITHER 0x00401530 NOR any
//   no-context link: it has no e1 and no e2 link at all, and the cleanup after its first link is 0x20 rather than the 0x2c that every
//   two-phase registrar here uses - the size follows from there being one link before the shared cleanup instead of three. That gap is
//   transcribed rather than filled. The parent getter 0x00477BF0 is `mov ax, word [0x005BE3C6]` - the Gun type id that 0x00477E30 stores - so
//   this type is parented onto Gun even though it is a pickup; the parent is a transcription fact, not an inference about what the type does.
//   The id slot 0x005BE334 holds image value 0, is written with a 16-bit store and is read back at the tail with a 32-bit load masked by
//   0xFFFF, so it is declared as a plain std::uint16_t; it is one of the image-zero slots in the 0x005BE3xx cluster. Handler 0x004754C0 is now
//   linked by THREE bodies in this lane - 0x00475380, 0x00470610 and this one - and all three drive it from the same 0x004F1FE0 getter, which
//   the image-wide sweep confirms (the immediate is referenced at 0x004753C2, 0x00470652 and 0x0047043F and nowhere else). Handler 0x004D3710
//   is referenced 148 times as a pushed handler immediate across the image and is a generic filler rather than a task-specific handler. Both
//   are transcribed as retail writes them. Documented shape differences, all value-preserving: (1) every getter callee is an argument-less load
//   in retail that ignores the arguments the caller pushes, and the tree's transcriptions take no arguments, so those inert pushes are not
//   reproduced; each is simultaneously the next 0x00401400 link call's pre-pushed handler, which the port keeps as the handler value in the
//   same order; (2) byte-valued event ids are zero-extended in the port, which cannot change behaviour because every link fed by a byte getter
//   here goes through 0x00401400, which masks the event word for the phase index. Handlers 0x00470500, 0x004754C0, 0x00470540 and 0x00470570
//   are raw code immediates in retail and are not transcribed yet (native gaps in 0x0046xxxx-0x0048xxxx); handler 0x00427D70 is BELOW
//   0x00460000 and belongs to the 0x00400000-0x0045FFFF owner, and 0x004D3710 is above 0x0048FFFF and is implemented elsewhere. Domain:
//   core/, because it is one of the 0x00402930 startup registrars.
#include "igi/retail/core/task_types.hpp"

#include <cstdint>

extern "C" std::uint16_t Sys_GetGlobal16_477BF0(void);            // 0x00477BF0: argument-less word getter (the Gun type id)
extern "C" std::uint8_t Sys_GetGlobal8_4F1FE0(void);              // 0x004F1FE0: argument-less byte getter (sys/native_4f1fe0.cpp)
extern "C" std::uint8_t Task_GetCachedEventId_460BF0(void);       // 0x00460BF0: lazily allocated event id (core/native_00460bf0.cpp)
extern "C" std::uint8_t Task_GetCachedEventId_460C10(void);       // 0x00460C10: lazily allocated event id (core/native_460c10.cpp)
extern "C" std::uint8_t Core_GetFlagByte_413B40(void);            // 0x00413B40: argument-less byte getter (core/native_413b40.cpp)
extern "C" std::uint8_t Core_GetFlagByte_413B30(void);            // 0x00413B30: argument-less byte getter (core/native_413b30.cpp)
extern "C" std::uint8_t Sys_GetSettingByte_4071D0(void);          // 0x004071D0: argument-less byte getter (sys/native_4071d0.cpp)
extern "C" void Script_QHashNodeRegister(void* arg1, void* value); // 0x004B8810 (script/native_4b8810.cpp)

extern "C" {

// 0x005BE334: the "Medipack" task type id. Image value 0, overwritten by a 16-bit store from TaskType_Register's return value.
std::uint16_t g_5be334 = 0U;

void Task_DefineMedipack()
{
    // 0x00470410/0x00470412: push 0; push 0 (both inert arguments of the parent getter); 0x00470414: call 0x477BF0; 0x00470419: push eax;
    // 0x0047041A: push 0x1B0; 0x0047041F: push "Medipack"; 0x00470424: call 0x401900.
    const std::uint16_t medipack_id = TaskType_Register("Medipack", // 0x0047041F: push 0x0053FF04
                                                        0x1B0U,      // 0x0047041A: push 0x1B0 (object size)
                                                        Sys_GetGlobal16_477BF0(), // 0x00470414: call 0x477BF0 (parent type)
                                                        0U);         // 0x00470412: push 0 (inert)

    // 0x00470429: push 0x00470500 (pre-pushed handler); 0x0047042E: push 0; 0x00470430: push eax;
    // 0x00470431: mov word [0x5BE334],ax (the store lands between the pushes and the call); 0x00470437: call 0x401400;
    // 0x0047043C: add esp,0x20 - the eight pushes above (two inert, three registration, three link). There is no e1 or e2 link here, which is
    // why this cleanup is 0x20 and not the 0x2c the two-phase registrars use.
    ::g_5be334 = medipack_id;
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be334), // 0x0047043C is the cleanup; the id is read below
                                0U,                                     // 0x0047042E: push 0 (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00470500U))); // 0x00470429: push handler

    // 0x0047043F: push 0x004754C0 (the handler 0x00475380 and 0x00470610 also link for their 0x004F1FE0 event); 0x00470444: call 0x4F1FE0;
    // 0x00470449: push eax; 0x0047044A: mov ax,[0x5BE334]; 0x00470450: push eax; 0x00470451: call 0x401400; 0x00470456: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be334), // 0x0047044A: mov ax,[0x5BE334]
                                static_cast<std::uint32_t>(Sys_GetGlobal8_4F1FE0()), // 0x00470449: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x004754C0U))); // 0x0047043F: push handler

    // 0x00470459: push 0x004D3710; 0x0047045E: call 0x460BF0; 0x00470463: mov cx,[0x5BE334]; 0x0047046A: push eax; 0x0047046B: push ecx;
    // 0x0047046C: call 0x401400; 0x00470471: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be334), // 0x00470463: mov cx,[0x5BE334]
                                static_cast<std::uint32_t>(Task_GetCachedEventId_460BF0()), // 0x0047046A: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x004D3710U))); // 0x00470459: push handler

    // 0x00470474: push 0x004D3710; 0x00470479: call 0x460C10; 0x0047047E: mov dx,[0x5BE334]; 0x00470485: push eax; 0x00470486: push edx;
    // 0x00470487: call 0x401400; 0x0047048C: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be334), // 0x0047047E: mov dx,[0x5BE334]
                                static_cast<std::uint32_t>(Task_GetCachedEventId_460C10()), // 0x00470485: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x004D3710U))); // 0x00470474: push handler

    // 0x0047048F: push 0x00470540; 0x00470494: call 0x413B40; 0x00470499: push eax; 0x0047049A: mov ax,[0x5BE334]; 0x004704A0: push eax;
    // 0x004704A1: call 0x401400; 0x004704A6: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be334), // 0x0047049A: mov ax,[0x5BE334]
                                static_cast<std::uint32_t>(Core_GetFlagByte_413B40()), // 0x00470499: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00470540U))); // 0x0047048F: push handler

    // 0x004704A9: push 0x00427D70; 0x004704AE: call 0x413B30; 0x004704B3: mov cx,[0x5BE334]; 0x004704BA: push eax; 0x004704BB: push ecx;
    // 0x004704BC: call 0x401400; 0x004704C1: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be334), // 0x004704B3: mov cx,[0x5BE334]
                                static_cast<std::uint32_t>(Core_GetFlagByte_413B30()), // 0x004704BA: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00427D70U))); // 0x004704A9: push handler (below 0x00460000)

    // 0x004704C4: push 0x00470570; 0x004704C9: call 0x4071D0; 0x004704CE: mov dx,[0x5BE334]; 0x004704D5: push eax; 0x004704D6: push edx;
    // 0x004704D7: call 0x401400. This link has no cleanup of its own.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be334), // 0x004704CE: mov dx,[0x5BE334]
                                static_cast<std::uint32_t>(Sys_GetSettingByte_4071D0()), // 0x004704D5: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00470570U))); // 0x004704C4: push handler

    // 0x004704DC: mov eax,[0x5BE334] (a 32-bit load); 0x004704E1: and eax,0xFFFF; 0x004704E6: push eax;
    // 0x004704E7: push "TASKTYPE_MEDIPACK"; 0x004704EC: call 0x4B8810; 0x004704F1: add esp,0x14 - the last link's three pushes plus the
    // tail's two; 0x004704F4: ret.
    Script_QHashNodeRegister(const_cast<char*>("TASKTYPE_MEDIPACK"), // 0x004704E7: push 0x0053FEF0 (argument 1)
                             reinterpret_cast<void*>(static_cast<std::uintptr_t>(
                                 static_cast<std::uint32_t>(::g_5be334)))); // 0x004704E1: the id masked to 16 bits (argument 2)
} // end of Task_DefineMedipack

} // extern "C"
