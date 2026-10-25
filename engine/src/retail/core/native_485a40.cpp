// Native: Task_DefineStatusMessageScreen (behavior-derived placeholder name; retail symbol unknown); retail VA 0x00485A40-0x00485AC4.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 133 bytes, 1 block, 36 instructions.
// Tools/evidence: radare2 6.2.0 af/afbj/pdf with bin.relocs.apply over the exact image (1 block / 36 instructions / 133 bytes); both cleanups are
//   accounted for independently: `add esp,0x38` at 0x00485A93 is 56 bytes = 14 pushes (2 inert zeros + 3 TaskType_Register arguments + three
//   3-argument links), and `add esp,0x1c` at 0x00485AC1 is 28 bytes = the pre-pushed handler 0x00486020 + 2 link arguments + the four
//   0x004B8890 arguments. 0x004018F0 is the argument-less QTask_GetCurrentType() getter, so the two zeros pushed at 0x00485A40/0x00485A42 are
//   inert and survive as TaskType_Register's fourth and fifth slots. 0x004E7530 is the transcribed getter Core_GetEventWord_4E7530
//   (`mov eax,[0x005488C8]; ret`), called with one ignored argument, so the pre-pushed 0x00486020 is the fourth link's handler and the getter's
//   return is that link's event id. 0x00401400 and 0x00401530 read [esp+4]/[esp+8]/[esp+0xC] as (class id, event id, handler). 0x00540B08 is
//   written as a word at 0x00485A61 and read back as a word at 0x00485A6C, 0x00485A7F and 0x00485AA0; the image holds the dword 0x00000181 there
//   (bytes 81 01 00 00 at 0x00540B08), so the port initialises the word to 385 exactly as the loader leaves it.
// Behavior/CFG: registers the "StatusMessageScreen" task type as TaskType_Register("StatusMessageScreen", 0x18440, QTask_GetCurrentType(), 0) and
//   stores the returned 16-bit id at 0x00540B08; links the id to handler 0x00485B20 on event 0 through 0x00401400, then to handler 0x00485B40 on
//   event 1 and handler 0x00486010 on event 2 through 0x00401530, then to handler 0x00486020 on the event id returned by 0x004E7530 through
//   0x00401530 again; finally registers the script command "StatusMessageScreen_ClearMessages" with handler 0x00485AD0 through 0x004B8890 with the
//   tuple (name, handler, 0, 0) - note the fourth argument is 0 here, where the ammo and mission registrars pass -1. Straight-line, no branch, no
//   loop. The object size 0x18440 (99,392 bytes) is the largest in this batch.
// Status/limits: BUILD_VERIFIED; runtime UNOBSERVED. Handlers 0x00485B20, 0x00485B40, 0x00486010, 0x00486020 and 0x00485AD0 are raw code immediates
//   in retail and are not transcribed yet (native gaps). 0x00540B08 has no other owner in the tree, so this file defines it with its image value.
//   The class id pushed at 0x00485A60 is TaskType_Register's return value in EAX, whose meaningful part is the 16-bit id; the port passes the
//   zero-extended word, matching every later read of 0x00540B08. Domain: core/, because it is one of the 0x00402930 startup registrars.
#include "igi/retail/core/task_types.hpp"

#include <cstdint>

extern "C" std::uint32_t Core_GetEventWord_4E7530();  // 0x004E7530: argument-less event-word getter
extern "C" void Script_QHashNodeRegisterEx(void* const arg0, void* const arg1, char const* const arg2,
                                           void* const arg3); // 0x004B8890 (copied from script/native_4b8890.cpp)

extern "C" {

std::uint16_t g_540b08 = 385U; // retail 0x00540B08: the "StatusMessageScreen" task type id (image dword 0x00000181)

void Task_DefineStatusMessageScreen()
{
    // 0x00485A40: push 0 (inert fifth slot); 0x00485A42: push 0 (the fourth TaskType_Register argument);
    // 0x00485A44: call 0x4018F0; 0x00485A49: push eax; 0x00485A4A: push 0x18440; 0x00485A4F: push "StatusMessageScreen"; 0x00485A54: call 0x401900.
    g_540b08 = TaskType_Register("StatusMessageScreen",      // 0x00485A4F: push 0x00540B30
                                 0x18440U,                   // 0x00485A4A: push 0x18440 (object size)
                                 QTask_GetCurrentType(),     // 0x00485A44: call 0x4018F0 (parent type)
                                 0U);                        // 0x00485A42: push 0 (task list count)
    // 0x00485A61: mov word [0x540B08],ax.

    const std::uint32_t class_id = static_cast<std::uint32_t>(::g_540b08); // 0x00485A6C/0x00485A7F/0x00485AA0: the id read back from 0x540B08

    // 0x00485A59: push 0x00485B20 (pre-pushed third argument); 0x00485A5E: push 0; 0x00485A60: push eax; 0x00485A67: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(class_id),      // argument 1
                                0U,                                        // 0x00485A5E: push 0 (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00485B20U))); // 0x00485A59: push handler (argument 3)

    // 0x00485A72: push 0x00485B40 (pre-pushed third argument); 0x00485A77: push 1; 0x00485A79: push eax; 0x00485A7A: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(class_id), // argument 1
                                           1U,                                 // 0x00485A77: push 1 (argument 2)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x00485B40U))); // 0x00485A72: push handler

    // 0x00485A86: push 0x00486010 (pre-pushed third argument); 0x00485A8B: push 2; 0x00485A8D: push ecx; 0x00485A8E: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(class_id), // argument 1
                                           2U,                                 // 0x00485A8B: push 2 (argument 2)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x00486010U))); // 0x00485A86: push handler
    // 0x00485A93: add esp,0x38 - 14 pushes above.

    // 0x00485A96: push 0x00486020 (pre-pushed third argument); 0x00485A9B: call 0x4E7530; 0x00485AA7: push eax; 0x00485AA8: push edx; 0x00485AA9: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(class_id), // argument 1
                                           Core_GetEventWord_4E7530(),          // 0x00485AA7: push eax (argument 2)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x00486020U))); // 0x00485A96: push handler

    // 0x00485AAE: push 0; 0x00485AB0: push 0; 0x00485AB2: push 0x00485AD0; 0x00485AB7: push "StatusMessageScreen_ClearMessages"; 0x00485ABC: call 0x4B8890.
    Script_QHashNodeRegisterEx(                                       // 0x00485ABC: call 0x004B8890
        const_cast<char*>("StatusMessageScreen_ClearMessages"),       // 0x00485AB7: push 0x00540B0C
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x00485AD0U)), // 0x00485AB2: push 0x00485AD0 (handler)
        nullptr,                                                     // 0x00485AB0: push 0
        nullptr);                                                    // 0x00485AAE: push 0 (retail passes 0, not -1)
    // 0x00485AC1: add esp,0x1c - 7 pushes above.
} // 0x00485AC4: ret.

} // extern "C"
