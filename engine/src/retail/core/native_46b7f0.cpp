// Native: Task_DefineComputerTextItem (behavior-derived placeholder name; retail symbol unknown); retail VA 0x0046B7F0-0x0046B87B.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 140 bytes, 1 block, 38 instructions.
// Tools/evidence: radare2 6.2.0 af/afbj/pdf with bin.relocs.apply over the exact image (1 block / 38 instructions / 140 bytes); all three cleanups
//   are accounted for independently: `add esp,0x38` at 0x0046B843 is 56 bytes = 14 pushes (2 inert zeros + 3 TaskType_Register arguments + three
//   3-argument links), `add esp,0xc` at 0x0046B85E is the pre-pushed handler 0x0046B890 plus 2 link arguments, and `add esp,0xc` at 0x0046B878 is
//   the pre-pushed handler 0x0046B940 plus 2 link arguments. 0x00505C80 is the transcribed argument-less word getter Sys_GetGlobal16_505C80, so the
//   two zeros pushed at 0x0046B7F0/0x0046B7F2 are inert and survive as TaskType_Register's fourth and fifth slots. 0x004E7530 is the transcribed
//   getter Core_GetEventWord_4E7530 and 0x00504DF0 the transcribed byte getter Sys_GetGlobal8_504DF0, both called with one ignored argument, so the
//   pre-pushed 0x0046B890 and 0x0046B940 are the handlers of the two links that take their event ids from those getters. 0x00401400 and 0x00401530
//   read [esp+4]/[esp+8]/[esp+0xC] as (class id, event id, handler). 0x005BE2BC is written as a word at 0x0046B811 and read back as a word at
//   0x0046B81C, 0x0046B82F, 0x0046B850 and 0x0046B86C; the image holds 0 there.
// Behavior/CFG: registers the "ComputerTextItem" task type as TaskType_Register("ComputerTextItem", 0x90, Sys_GetGlobal16_505C80(), 0) and stores the
//   returned 16-bit id at 0x005BE2BC; links the id to handler 0x004D3710 on event 0 through 0x00401400, to the SAME handler 0x004D3710 on event 1 and
//   handler 0x0046B880 on event 2 through 0x00401530, then to handler 0x0046B890 on the event id from 0x004E7530 through 0x00401530, and finally to
//   handler 0x0046B940 on the event id from 0x00504DF0 through 0x00401400. No 0x004B8810 call (no "TASKTYPE_..." name). Straight-line, no branch,
//   no loop. This is the ComputerButtonItem registrar's twin apart from the string, the object size, the global and handlers 0x0046B5A0/0x0046B5C0;
//   both share the double-linked 0x004D3710 and the final 0x0046B940 link.
// Status/limits: BUILD_VERIFIED; runtime UNOBSERVED. Handlers 0x004D3710, 0x0046B880, 0x0046B890 and 0x0046B940 are raw code immediates in retail and are
//   not transcribed yet (native gaps; 0x004D3710 is the address of the Lead's open cosmetic symbol-swap note, untouched here). 0x005BE2BC has no other
//   owner and image value 0, so this file defines it. The class id pushed at 0x0046B810 is TaskType_Register's return value in EAX, whose meaningful
//   part is the 16-bit id; the port passes the zero-extended word, matching every later read of 0x005BE2BC. Domain: core/, because it is one of the
//   0x00402930 startup registrars and belongs to the Computer family.
#include "igi/retail/core/task_types.hpp"

#include <cstdint>

extern "C" std::uint16_t Sys_GetGlobal16_505C80(void);  // 0x00505C80: argument-less word getter
extern "C" std::uint32_t Core_GetEventWord_4E7530();    // 0x004E7530: argument-less event-word getter
extern "C" std::uint8_t Sys_GetGlobal8_504DF0(void);    // 0x00504DF0: argument-less byte getter

extern "C" {

std::uint16_t g_5be2bc = 0U; // retail 0x005BE2BC: the "ComputerTextItem" task type id written here (image value 0)

void Task_DefineComputerTextItem()
{
    // 0x0046B7F0: push 0 (inert fifth slot); 0x0046B7F2: push 0 (the fourth TaskType_Register argument);
    // 0x0046B7F4: call 0x505C80; 0x0046B7F9: push eax; 0x0046B7FA: push 0x90; 0x0046B7FF: push "ComputerTextItem"; 0x0046B804: call 0x401900.
    g_5be2bc = TaskType_Register("ComputerTextItem",         // 0x0046B7FF: push 0x0053FD84
                                 0x90U,                      // 0x0046B7FA: push 0x90 (object size)
                                 Sys_GetGlobal16_505C80(),   // 0x0046B7F4: call 0x505C80 (parent type)
                                 0U);                        // 0x0046B7F2: push 0 (task list count)
    // 0x0046B811: mov word [0x5BE2BC],ax.

    const std::uint32_t class_id = static_cast<std::uint32_t>(::g_5be2bc); // 0x0046B81C/0x0046B82F/0x0046B850/0x0046B86C: read back from 0x5BE2BC

    // 0x0046B809: push 0x004D3710 (pre-pushed third argument); 0x0046B80E: push 0; 0x0046B810: push eax; 0x0046B817: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(class_id),      // argument 1
                                0U,                                        // 0x0046B80E: push 0 (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x004D3710U))); // 0x0046B809: push handler (argument 3)

    // 0x0046B822: push 0x004D3710 (pre-pushed third argument, same handler as event 0); 0x0046B827: push 1; 0x0046B829: push eax; 0x0046B82A: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(class_id), // argument 1
                                           1U,                                 // 0x0046B827: push 1 (argument 2)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x004D3710U))); // 0x0046B822: push handler

    // 0x0046B836: push 0x0046B880 (pre-pushed third argument); 0x0046B83B: push 2; 0x0046B83D: push ecx; 0x0046B83E: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(class_id), // argument 1
                                           2U,                                 // 0x0046B83B: push 2 (argument 2)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x0046B880U))); // 0x0046B836: push handler
    // 0x0046B843: add esp,0x38 - 14 pushes above.

    // 0x0046B846: push 0x0046B890 (pre-pushed third argument); 0x0046B84B: call 0x4E7530; 0x0046B857: push eax; 0x0046B858: push edx; 0x0046B859: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(class_id), // argument 1
                                           Core_GetEventWord_4E7530(),          // 0x0046B857: push eax (argument 2)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x0046B890U))); // 0x0046B846: push handler
    // 0x0046B85E: add esp,0xc - 3 pushes above.

    // 0x0046B861: push 0x0046B940 (pre-pushed third argument); 0x0046B866: call 0x504DF0; 0x0046B86B: push eax; 0x0046B872: push eax; 0x0046B873: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(class_id),      // argument 1
                                static_cast<std::uint32_t>(Sys_GetGlobal8_504DF0()), // 0x0046B86B: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x0046B940U))); // 0x0046B861: push handler (argument 3)
    // 0x0046B878: add esp,0xc - 3 pushes above.
} // 0x0046B87B: ret.

} // extern "C"
