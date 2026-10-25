// Native: Task_DefineGameScreen (behavior-derived placeholder name; retail symbol unknown); retail VA 0x004851B0-0x0048521E.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 111 bytes, 1 block, 31 instructions.
// Tools/evidence: radare2 6.2.0 af/afbj/pdf with bin.relocs.apply over the exact image (1 block / 31 instructions / 111 bytes); both cleanups are
//   accounted for independently: `add esp,0x38` at 0x00485200 is 56 bytes = 14 pushes (2 inert zeros + 3 TaskType_Register arguments + three
//   3-argument links), and `add esp,0xc` at 0x0048521B is 12 bytes = the pre-pushed handler 0x00424280 plus 2 link arguments. 0x004018F0 is the
//   argument-less QTask_GetCurrentType() getter, so the two zeros pushed at 0x004851B0/0x004851B2 are inert and survive as TaskType_Register's
//   fourth and fifth slots. 0x004E7530 is the transcribed getter Core_GetEventWord_4E7530 (`mov eax,[0x005488C8]; ret`), called with one ignored
//   argument, so the pre-pushed 0x00424280 is the fourth link's handler and the getter's return is that link's event id. 0x00401400 and 0x00401530
//   read [esp+4]/[esp+8]/[esp+0xC] as (class id, event id, handler). 0x005C8A28 is written as a word at 0x004851CE and read back as a word at
//   0x004851D9, 0x004851EC and 0x0048520D.
// Behavior/CFG: registers the "GameScreen" task type as TaskType_Register("GameScreen", 0x28, QTask_GetCurrentType(), 0) and stores the returned
//   16-bit id at 0x005C8A28; links the id to handler 0x00485220 on event 0 through 0x00401400, then to handler 0x00488700 on event 1 and handler
//   0x004D3710 on event 2 through 0x00401530, then to handler 0x00424280 on the event id returned by 0x004E7530 through 0x00401530 again. Like the
//   ComputerLog and ComputerObjectives registrars it does NOT call 0x004B8810 (no "TASKTYPE_..." script name). Straight-line, no branch, no loop.
// Status/limits: BUILD_VERIFIED; runtime UNOBSERVED. Handlers 0x00485220, 0x00488700, 0x004D3710 and 0x00424280 are raw code immediates in retail and
//   are not transcribed yet (native gaps; 0x004D3710 is also the address of the Lead's open cosmetic symbol-swap note, which this file does not
//   touch). 0x005C8A28 has no other owner and image value 0, so this file defines it. The class id pushed at 0x004851CD is TaskType_Register's
//   return value in EAX, whose meaningful part is the 16-bit id; the port passes the zero-extended word, matching every later read of 0x005C8A28.
//   Domain: core/, because it is one of the 0x00402930 startup registrars.
#include "igi/retail/core/task_types.hpp"

#include <cstdint>

extern "C" std::uint32_t Core_GetEventWord_4E7530(); // 0x004E7530: argument-less event-word getter

extern "C" {

std::uint16_t g_5c8a28 = 0U; // retail 0x005C8A28: the "GameScreen" task type id written here (image value 0)

void Task_DefineGameScreen()
{
    // 0x004851B0: push 0 (inert fifth slot); 0x004851B2: push 0 (the fourth TaskType_Register argument);
    // 0x004851B4: call 0x4018F0; 0x004851B9: push eax; 0x004851BA: push 0x28; 0x004851BC: push "GameScreen"; 0x004851C1: call 0x401900.
    g_5c8a28 = TaskType_Register("GameScreen",               // 0x004851BC: push 0x00540A80
                                 0x28U,                      // 0x004851BA: push 0x28 (object size)
                                 QTask_GetCurrentType(),     // 0x004851B4: call 0x4018F0 (parent type)
                                 0U);                        // 0x004851B2: push 0 (task list count)
    // 0x004851CE: mov word [0x5C8A28],ax.

    const std::uint32_t class_id = static_cast<std::uint32_t>(::g_5c8a28); // 0x004851D9/0x004851EC/0x0048520D: the id read back from 0x5C8A28

    // 0x004851C6: push 0x00485220 (pre-pushed third argument); 0x004851CB: push 0; 0x004851CD: push eax; 0x004851D4: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(class_id),      // argument 1
                                0U,                                        // 0x004851CB: push 0 (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00485220U))); // 0x004851C6: push handler (argument 3)

    // 0x004851DF: push 0x00488700 (pre-pushed third argument); 0x004851E4: push 1; 0x004851E6: push eax; 0x004851E7: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(class_id), // argument 1
                                           1U,                                 // 0x004851E4: push 1 (argument 2)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x00488700U))); // 0x004851DF: push handler

    // 0x004851F3: push 0x004D3710 (pre-pushed third argument); 0x004851F8: push 2; 0x004851FA: push ecx; 0x004851FB: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(class_id), // argument 1
                                           2U,                                 // 0x004851F8: push 2 (argument 2)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x004D3710U))); // 0x004851F3: push handler
    // 0x00485200: add esp,0x38 - 14 pushes above.

    // 0x00485203: push 0x00424280 (pre-pushed third argument); 0x00485208: call 0x4E7530; 0x00485214: push eax; 0x00485215: push edx; 0x00485216: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(class_id), // argument 1
                                           Core_GetEventWord_4E7530(),          // 0x00485214: push eax (argument 2)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x00424280U))); // 0x00485203: push handler
    // 0x0048521B: add esp,0xc - 3 pushes above.
} // 0x0048521E: ret.

} // extern "C"
