// Native: Task_DefineComputerMenu (behavior-derived placeholder name; retail symbol unknown); retail VA 0x0046AB60-0x0046ABDD.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 126 bytes, 1 block, 34 instructions.
// Tools/evidence: radare2 6.2.0 af/afbj/pdf with bin.relocs.apply over the exact image (1 block / 34 instructions / 126 bytes); both cleanups are
//   accounted for independently: `add esp,0x38` at 0x0046ABB0 is 56 bytes = 14 pushes (2 inert zeros + 3 TaskType_Register arguments + three
//   3-argument links), and `add esp,0x14` at 0x0046ABDA is 20 bytes = the pre-pushed handler 0x0046AE30 + 2 link arguments + the two alias
//   arguments. 0x004018F0 is the argument-less QTask_GetCurrentType() getter, so the two zeros pushed at 0x0046AB60/0x0046AB62 are inert and
//   survive as TaskType_Register's fourth and fifth slots. 0x004E7530 is the transcribed getter Core_GetEventWord_4E7530 (`mov eax,[0x005488C8]; ret`),
//   called with one ignored argument, so the pre-pushed 0x0046AE30 is the fourth link's handler and the getter's return is that link's event id.
//   0x00401400 and 0x00401530 read [esp+4]/[esp+8]/[esp+0xC] as (class id, event id, handler). 0x005BDDA8 is written as a word at 0x0046AB7E and read
//   back as a word at 0x0046AB89, 0x0046AB9C and 0x0046ABBD. 0x004B12F0 is the transcribed QFile_RegisterAlias_4B12F0(const char* key,
//   const char* target), called with ("COMPUTER:", "LOCAL:computer/").
// Behavior/CFG: registers the "ComputerMenu" task type as TaskType_Register("ComputerMenu", 0x3C, QTask_GetCurrentType(), 0) and stores the returned
//   16-bit id at 0x005BDDA8; links the id to handler 0x0046ABE0 on event 0 through 0x00401400, then to handler 0x0046AC30 on event 1 and handler
//   0x0046AE20 on event 2 through 0x00401530, then to handler 0x0046AE30 on the event id returned by 0x004E7530 through 0x00401530 again; finally
//   registers the file alias "COMPUTER:" -> "LOCAL:computer/" through 0x004B12F0. Like the other Computer registrars it does NOT call 0x004B8810
//   (no "TASKTYPE_..." script name). Straight-line, no branch, no loop.
// Status/limits: BUILD_VERIFIED; runtime UNOBSERVED. Handlers 0x0046ABE0, 0x0046AC30, 0x0046AE20 and 0x0046AE30 are raw code immediates in retail and
//   are not transcribed yet (native gaps). 0x005BDDA8 has no other owner and image value 0, so this file defines it. The class id pushed at
//   0x0046AB7D is TaskType_Register's return value in EAX, whose meaningful part is the 16-bit id; the port passes the zero-extended word, matching
//   every later read of 0x005BDDA8. This is the first registrar in the batch whose two handlers differ from the shared 0x00488700/0x004D3710 pair
//   (see the batch report: the ComputerMenu handlers are its own, so the shared pair is not universal). Domain: core/, because it is one of the
//   0x00402930 startup registrars.
#include "igi/retail/core/task_types.hpp"

#include <cstdint>

extern "C" std::uint32_t Core_GetEventWord_4E7530();                       // 0x004E7530: argument-less event-word getter
extern "C" void QFile_RegisterAlias_4B12F0(const char* key, const char* target); // 0x004B12F0 (cdecl is the i386 default)

extern "C" {

std::uint16_t g_5bdda8 = 0U; // retail 0x005BDDA8: the "ComputerMenu" task type id written here (image value 0)

void Task_DefineComputerMenu()
{
    // 0x0046AB60: push 0 (inert fifth slot); 0x0046AB62: push 0 (the fourth TaskType_Register argument);
    // 0x0046AB64: call 0x4018F0; 0x0046AB69: push eax; 0x0046AB6A: push 0x3C; 0x0046AB6C: push "ComputerMenu"; 0x0046AB71: call 0x401900.
    g_5bdda8 = TaskType_Register("ComputerMenu",             // 0x0046AB6C: push 0x0053FA08
                                 0x3CU,                      // 0x0046AB6A: push 0x3C (object size)
                                 QTask_GetCurrentType(),     // 0x0046AB64: call 0x4018F0 (parent type)
                                 0U);                        // 0x0046AB62: push 0 (task list count)
    // 0x0046AB7E: mov word [0x5BDDA8],ax.

    const std::uint32_t class_id = static_cast<std::uint32_t>(::g_5bdda8); // 0x0046AB89/0x0046AB9C/0x0046ABBD: the id read back from 0x5BDDA8

    // 0x0046AB76: push 0x0046ABE0 (pre-pushed third argument); 0x0046AB7B: push 0; 0x0046AB7D: push eax; 0x0046AB84: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(class_id),      // argument 1
                                0U,                                        // 0x0046AB7B: push 0 (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x0046ABE0U))); // 0x0046AB76: push handler (argument 3)

    // 0x0046AB8F: push 0x0046AC30 (pre-pushed third argument); 0x0046AB94: push 1; 0x0046AB96: push eax; 0x0046AB97: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(class_id), // argument 1
                                           1U,                                 // 0x0046AB94: push 1 (argument 2)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x0046AC30U))); // 0x0046AB8F: push handler

    // 0x0046ABA3: push 0x0046AE20 (pre-pushed third argument); 0x0046ABA8: push 2; 0x0046ABAA: push ecx; 0x0046ABAB: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(class_id), // argument 1
                                           2U,                                 // 0x0046ABA8: push 2 (argument 2)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x0046AE20U))); // 0x0046ABA3: push handler
    // 0x0046ABB0: add esp,0x38 - 14 pushes above.

    // 0x0046ABB3: push 0x0046AE30 (pre-pushed third argument); 0x0046ABB8: call 0x4E7530; 0x0046ABC4: push eax; 0x0046ABC5: push edx; 0x0046ABC6: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(class_id), // argument 1
                                           Core_GetEventWord_4E7530(),          // 0x0046ABC4: push eax (argument 2)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x0046AE30U))); // 0x0046ABB3: push handler

    // 0x0046ABCB: push "LOCAL:computer/"; 0x0046ABD0: push "COMPUTER:"; 0x0046ABD5: call 0x4B12F0; 0x0046ABDA: add esp,0x14.
    QFile_RegisterAlias_4B12F0("COMPUTER:",                  // 0x0046ABD0: push 0x0053F9EC
                               "LOCAL:computer/");           // 0x0046ABCB: push 0x0053F9F8
} // 0x0046ABDD: ret.

} // extern "C"
