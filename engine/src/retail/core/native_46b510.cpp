// Native: Task_DefineComputerButtonItem (behavior-derived placeholder name; retail symbol unknown); retail VA 0x0046B510-0x0046B59B.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 140 bytes, 1 block, 38 instructions.
// Tools/evidence: radare2 6.2.0 af/afbj/pdf with bin.relocs.apply over the exact image (1 block / 38 instructions / 140 bytes); all three cleanups
//   are accounted for independently: `add esp,0x38` at 0x0046B563 is 56 bytes = 14 pushes (2 inert zeros + 3 TaskType_Register arguments + three
//   3-argument links), `add esp,0xc` at 0x0046B57E is the pre-pushed handler 0x0046B5C0 plus 2 link arguments, and `add esp,0xc` at 0x0046B598 is
//   the pre-pushed handler 0x0046B940 plus 2 link arguments. 0x00505C80 is the transcribed argument-less word getter Sys_GetGlobal16_505C80, so the
//   two zeros pushed at 0x0046B510/0x0046B512 are inert and survive as TaskType_Register's fourth and fifth slots. 0x004E7530 is the transcribed
//   getter Core_GetEventWord_4E7530 (`mov eax,[0x005488C8]; ret`) and 0x00504DF0 the transcribed byte getter Sys_GetGlobal8_504DF0, both called
//   with one ignored argument, so the pre-pushed 0x0046B5C0 and 0x0046B940 are the handlers of the two links that take their event ids from those
//   getters. 0x00401400 and 0x00401530 read [esp+4]/[esp+8]/[esp+0xC] as (class id, event id, handler). 0x005BE2B4 is written as a word at
//   0x0046B531 and read back as a word at 0x0046B53C, 0x0046B54F, 0x0046B570 and 0x0046B58C; the image holds 0 there.
// Behavior/CFG: registers the "ComputerButtonItem" task type as TaskType_Register("ComputerButtonItem", 0x94, Sys_GetGlobal16_505C80(), 0) and stores
//   the returned 16-bit id at 0x005BE2B4; links the id to handler 0x004D3710 on event 0 through 0x00401400, to the SAME handler 0x004D3710 on event 1
//   and handler 0x0046B5A0 on event 2 through 0x00401530, then to handler 0x0046B5C0 on the event id from 0x004E7530 through 0x00401530, and finally
//   to handler 0x0046B940 on the event id from 0x00504DF0 through 0x00401400 - five links, and the last one returns to the full-context variant after
//   three no-context links. No 0x004B8810 call (no "TASKTYPE_..." name). Straight-line, no branch, no loop.
// Status/limits: BUILD_VERIFIED; runtime UNOBSERVED. Handlers 0x004D3710, 0x0046B5A0, 0x0046B5C0 and 0x0046B940 are raw code immediates in retail and are
//   not transcribed yet (native gaps; 0x004D3710 is the address of the Lead's open cosmetic symbol-swap note, untouched here). 0x005BE2B4 has no other
//   owner and image value 0, so this file defines it. The class id pushed at 0x0046B530 is TaskType_Register's return value in EAX, whose meaningful
//   part is the 16-bit id; the port passes the zero-extended word, matching every later read of 0x005BE2B4. Note that the same handler address
//   0x004D3710 is linked twice, on events 0 and 1; that is retail's bytes, not a copy-paste slip. Domain: core/, because it is one of the 0x00402930
//   startup registrars and belongs to the Computer family.
#include "igi/retail/core/task_types.hpp"

#include <cstdint>

extern "C" std::uint16_t Sys_GetGlobal16_505C80(void);  // 0x00505C80: argument-less word getter
extern "C" std::uint32_t Core_GetEventWord_4E7530();    // 0x004E7530: argument-less event-word getter
extern "C" std::uint8_t Sys_GetGlobal8_504DF0(void);    // 0x00504DF0: argument-less byte getter

extern "C" {

std::uint16_t g_5be2b4 = 0U; // retail 0x005BE2B4: the "ComputerButtonItem" task type id written here (image value 0)

void Task_DefineComputerButtonItem()
{
    // 0x0046B510: push 0 (inert fifth slot); 0x0046B512: push 0 (the fourth TaskType_Register argument);
    // 0x0046B514: call 0x505C80; 0x0046B519: push eax; 0x0046B51A: push 0x94; 0x0046B51F: push "ComputerButtonItem"; 0x0046B524: call 0x401900.
    g_5be2b4 = TaskType_Register("ComputerButtonItem",       // 0x0046B51F: push 0x0053FD70
                                 0x94U,                      // 0x0046B51A: push 0x94 (object size)
                                 Sys_GetGlobal16_505C80(),   // 0x0046B514: call 0x505C80 (parent type)
                                 0U);                        // 0x0046B512: push 0 (task list count)
    // 0x0046B531: mov word [0x5BE2B4],ax.

    const std::uint32_t class_id = static_cast<std::uint32_t>(::g_5be2b4); // 0x0046B53C/0x0046B54F/0x0046B570/0x0046B58C: read back from 0x5BE2B4

    // 0x0046B529: push 0x004D3710 (pre-pushed third argument); 0x0046B52E: push 0; 0x0046B530: push eax; 0x0046B537: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(class_id),      // argument 1
                                0U,                                        // 0x0046B52E: push 0 (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x004D3710U))); // 0x0046B529: push handler (argument 3)

    // 0x0046B542: push 0x004D3710 (pre-pushed third argument, same handler as event 0); 0x0046B547: push 1; 0x0046B549: push eax; 0x0046B54A: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(class_id), // argument 1
                                           1U,                                 // 0x0046B547: push 1 (argument 2)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x004D3710U))); // 0x0046B542: push handler

    // 0x0046B556: push 0x0046B5A0 (pre-pushed third argument); 0x0046B55B: push 2; 0x0046B55D: push ecx; 0x0046B55E: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(class_id), // argument 1
                                           2U,                                 // 0x0046B55B: push 2 (argument 2)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x0046B5A0U))); // 0x0046B556: push handler
    // 0x0046B563: add esp,0x38 - 14 pushes above.

    // 0x0046B566: push 0x0046B5C0 (pre-pushed third argument); 0x0046B56B: call 0x4E7530; 0x0046B577: push eax; 0x0046B578: push edx; 0x0046B579: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(class_id), // argument 1
                                           Core_GetEventWord_4E7530(),          // 0x0046B577: push eax (argument 2)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x0046B5C0U))); // 0x0046B566: push handler
    // 0x0046B57E: add esp,0xc - 3 pushes above.

    // 0x0046B581: push 0x0046B940 (pre-pushed third argument); 0x0046B586: call 0x504DF0; 0x0046B58B: push eax; 0x0046B592: push eax; 0x0046B593: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(class_id),      // argument 1
                                static_cast<std::uint32_t>(Sys_GetGlobal8_504DF0()), // 0x0046B58B: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x0046B940U))); // 0x0046B581: push handler (argument 3)
    // 0x0046B598: add esp,0xc - 3 pushes above.
} // 0x0046B59B: ret.

} // extern "C"
