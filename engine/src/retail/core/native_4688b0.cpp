// Native: Task_DefineComputerLog (behavior-derived placeholder name; retail symbol unknown); retail VA 0x004688B0-0x0046891E.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 111 bytes, 1 block, 31 instructions.
// Tools/evidence: radare2 6.2.0 af/afbj/pdf with bin.relocs.apply over the exact image (1 block / 31 instructions / 111 bytes); both cleanups are
//   accounted for independently: `add esp,0x38` at 0x00468900 is 56 bytes = 14 pushes (2 inert zeros + 3 TaskType_Register arguments + three
//   3-argument links), and `add esp,0xc` at 0x0046891B is 12 bytes = the pre-pushed handler 0x00468BC0 plus 2 link arguments. 0x004018F0 is the
//   argument-less QTask_GetCurrentType() getter, so the two zeros pushed at 0x004688B0/0x004688B2 are inert and survive as TaskType_Register's
//   fourth and fifth slots. 0x004E7530 is the transcribed getter Core_GetEventWord_4E7530 (`mov eax,[0x005488C8]; ret`), called with one ignored
//   argument, so the pre-pushed 0x00468BC0 is the fourth link's handler and the getter's return is that link's event id. 0x00401400 and 0x00401530
//   read [esp+4]/[esp+8]/[esp+0xC] as (class id, event id, handler). 0x005BDC98 is written as a word at 0x004688CE and read back as a word at
//   0x004688D9, 0x004688EC and 0x0046890D.
// Behavior/CFG: registers the "ComputerLog" task type as TaskType_Register("ComputerLog", 0x24, QTask_GetCurrentType(), 0) and stores the returned
//   16-bit id at 0x005BDC98; links the id to handler 0x00468920 on event 0 through 0x00401400, then to handler 0x00488700 on event 1 and handler
//   0x00468BB0 on event 2 through 0x00401530, then to handler 0x00468BC0 on the event id returned by 0x004E7530 through 0x00401530 again. This
//   registrar does NOT call 0x004B8810: like the FlashBang registrar, it registers no "TASKTYPE_..." script name. Straight-line, no branch, no loop.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. Handlers 0x00468920, 0x00488700, 0x00468BB0 and 0x00468BC0 are raw code immediates in retail and
//   are not transcribed yet (native gaps). 0x005BDC98 has no other owner and image value 0, so this file defines it. The class id pushed at
//   0x004688CD is TaskType_Register's return value in EAX, whose meaningful part is the 16-bit id; the port passes the zero-extended word, matching
//   what every later read of 0x005BDC98 sees. Domain: core/, because it is one of the 0x00402930 startup registrars.
#include "igi/retail/core/task_types.hpp"

#include <cstdint>

extern "C" std::uint32_t NATIVE_DECL Core_GetEventWord_4E7530(); // 0x004E7530: argument-less event-word getter

extern "C" {

std::uint16_t g_5bdc98 = 0U; // retail 0x005BDC98: the "ComputerLog" task type id written here (image value 0)

void Task_DefineComputerLog()
{
    // 0x004688B0: push 0 (inert fifth slot); 0x004688B2: push 0 (the fourth TaskType_Register argument);
    // 0x004688B4: call 0x4018F0; 0x004688B9: push eax; 0x004688BA: push 0x24; 0x004688BC: push "ComputerLog"; 0x004688C1: call 0x401900.
    g_5bdc98 = TaskType_Register("ComputerLog",              // 0x004688BC: push 0x0053F6E4
                                 0x24U,                      // 0x004688BA: push 0x24 (object size)
                                 QTask_GetCurrentType(),     // 0x004688B4: call 0x4018F0 (parent type)
                                 0U);                        // 0x004688B2: push 0 (task list count)
    // 0x004688CE: mov word [0x5BDC98],ax.

    const std::uint32_t class_id = static_cast<std::uint32_t>(::g_5bdc98); // 0x004688D9/0x004688EC/0x0046890D: the id read back from 0x5BDC98

    // 0x004688C6: push 0x00468920 (pre-pushed third argument); 0x004688CB: push 0; 0x004688CD: push eax; 0x004688D4: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(class_id),      // argument 1
                                0U,                                        // 0x004688CB: push 0 (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00468920U))); // 0x004688C6: push handler (argument 3)

    // 0x004688DF: push 0x00488700 (pre-pushed third argument); 0x004688E4: push 1; 0x004688E6: push eax; 0x004688E7: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(class_id), // argument 1
                                           1U,                                 // 0x004688E4: push 1 (argument 2)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x00488700U))); // 0x004688DF: push handler

    // 0x004688F3: push 0x00468BB0 (pre-pushed third argument); 0x004688F8: push 2; 0x004688FA: push ecx; 0x004688FB: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(class_id), // argument 1
                                           2U,                                 // 0x004688F8: push 2 (argument 2)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x00468BB0U))); // 0x004688F3: push handler
    // 0x00468900: add esp,0x38 - 14 pushes above.

    // 0x00468903: push 0x00468BC0 (pre-pushed third argument); 0x00468908: call 0x4E7530; 0x00468914: push eax; 0x00468915: push edx; 0x00468916: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(class_id), // argument 1
                                           Core_GetEventWord_4E7530(),          // 0x00468914: push eax (argument 2)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x00468BC0U))); // 0x00468903: push handler
    // 0x0046891B: add esp,0xc - 3 pushes above.
} // 0x0046891E: ret.

} // extern "C"
