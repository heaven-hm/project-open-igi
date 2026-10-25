// Native: Task_DefineComputerObjectives (behavior-derived placeholder name; retail symbol unknown); retail VA 0x00468E20-0x00468E8E.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 111 bytes, 1 block, 31 instructions.
// Tools/evidence: radare2 6.2.0 af/afbj/pdf with bin.relocs.apply over the exact image (1 block / 31 instructions / 111 bytes); both cleanups are
//   accounted for independently: `add esp,0x38` at 0x00468E70 is 56 bytes = 14 pushes (2 inert zeros + 3 TaskType_Register arguments + three
//   3-argument links), and `add esp,0xc` at 0x00468E8B is 12 bytes = the pre-pushed handler 0x00469110 plus 2 link arguments. 0x004018F0 is the
//   argument-less QTask_GetCurrentType() getter, so the two zeros pushed at 0x00468E20/0x00468E22 are inert and survive as TaskType_Register's
//   fourth and fifth slots. 0x004E7530 is the transcribed getter Core_GetEventWord_4E7530 (`mov eax,[0x005488C8]; ret`), called with one ignored
//   argument, so the pre-pushed 0x00469110 is the fourth link's handler and the getter's return is that link's event id. 0x00401400 and 0x00401530
//   read [esp+4]/[esp+8]/[esp+0xC] as (class id, event id, handler). 0x005BDD70 is written as a word at 0x00468E3E and read back as a word at
//   0x00468E49, 0x00468E5C and 0x00468E7D.
// Behavior/CFG: registers the "ComputerObjectives" task type as TaskType_Register("ComputerObjectives", 0x38, QTask_GetCurrentType(), 0) and stores
//   the returned 16-bit id at 0x005BDD70; links the id to handler 0x00468E90 on event 0 through 0x00401400, then to handler 0x00488700 on event 1 and
//   handler 0x004D3710 on event 2 through 0x00401530, then to handler 0x00469110 on the event id returned by 0x004E7530 through 0x00401530 again.
//   Like the ComputerLog registrar it does NOT call 0x004B8810 (no "TASKTYPE_..." script name). Straight-line, no branch, no loop. This is the
//   ComputerLog registrar's byte-for-byte twin apart from the string, the object size, the global and the three handler addresses.
// Status/limits: BUILD_VERIFIED; runtime UNOBSERVED. Handlers 0x00468E90, 0x00488700, 0x004D3710 and 0x00469110 are raw code immediates in retail and
//   are not transcribed yet (native gaps); 0x004D3710 is also the address the Lead has an open cosmetic symbol-swap note about, which this file does
//   not touch. 0x005BDD70 has no other owner and image value 0, so this file defines it. The class id pushed at 0x00468E3D is TaskType_Register's
//   return value in EAX, whose meaningful part is the 16-bit id; the port passes the zero-extended word, matching every later read of 0x005BDD70.
//   Domain: core/, because it is one of the 0x00402930 startup registrars.
#include "igi/retail/core/task_types.hpp"

#include <cstdint>

extern "C" std::uint32_t Core_GetEventWord_4E7530(); // 0x004E7530: argument-less event-word getter

extern "C" {

std::uint16_t g_5bdd70 = 0U; // retail 0x005BDD70: the "ComputerObjectives" task type id written here (image value 0)

void Task_DefineComputerObjectives()
{
    // 0x00468E20: push 0 (inert fifth slot); 0x00468E22: push 0 (the fourth TaskType_Register argument);
    // 0x00468E24: call 0x4018F0; 0x00468E29: push eax; 0x00468E2A: push 0x38; 0x00468E2C: push "ComputerObjectives"; 0x00468E31: call 0x401900.
    g_5bdd70 = TaskType_Register("ComputerObjectives",       // 0x00468E2C: push 0x0053F700
                                 0x38U,                      // 0x00468E2A: push 0x38 (object size)
                                 QTask_GetCurrentType(),     // 0x00468E24: call 0x4018F0 (parent type)
                                 0U);                        // 0x00468E22: push 0 (task list count)
    // 0x00468E3E: mov word [0x5BDD70],ax.

    const std::uint32_t class_id = static_cast<std::uint32_t>(::g_5bdd70); // 0x00468E49/0x00468E5C/0x00468E7D: the id read back from 0x5BDD70

    // 0x00468E36: push 0x00468E90 (pre-pushed third argument); 0x00468E3B: push 0; 0x00468E3D: push eax; 0x00468E44: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(class_id),      // argument 1
                                0U,                                        // 0x00468E3B: push 0 (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00468E90U))); // 0x00468E36: push handler (argument 3)

    // 0x00468E4F: push 0x00488700 (pre-pushed third argument); 0x00468E54: push 1; 0x00468E56: push eax; 0x00468E57: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(class_id), // argument 1
                                           1U,                                 // 0x00468E54: push 1 (argument 2)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x00488700U))); // 0x00468E4F: push handler

    // 0x00468E63: push 0x004D3710 (pre-pushed third argument); 0x00468E68: push 2; 0x00468E6A: push ecx; 0x00468E6B: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(class_id), // argument 1
                                           2U,                                 // 0x00468E68: push 2 (argument 2)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x004D3710U))); // 0x00468E63: push handler
    // 0x00468E70: add esp,0x38 - 14 pushes above.

    // 0x00468E73: push 0x00469110 (pre-pushed third argument); 0x00468E78: call 0x4E7530; 0x00468E84: push eax; 0x00468E85: push edx; 0x00468E86: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(class_id), // argument 1
                                           Core_GetEventWord_4E7530(),          // 0x00468E84: push eax (argument 2)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x00469110U))); // 0x00468E73: push handler
    // 0x00468E8B: add esp,0xc - 3 pushes above.
} // 0x00468E8E: ret.

} // extern "C"
