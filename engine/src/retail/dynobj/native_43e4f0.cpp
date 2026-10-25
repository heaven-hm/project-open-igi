// Native: Task_DefineGenericPhysicsMagicObj (behavior-derived placeholder name; retail symbol unknown); retail VA 0x0043E4F0-0x0043E57D.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 142 bytes, 1 block, 38 instructions.
// Tools/evidence: radare2 6.2.0 af/afbj/pdf with bin.relocs.apply over the exact image (1 block / 38 instructions / 142 bytes); all four cleanups are
//   accounted for independently: `add esp,0x2c` at 0x0043E52F is 44 bytes = 11 pushes (2 inert zeros + 3 TaskType_Register arguments + two
//   3-argument links), `add esp,0xc` at 0x0043E54A is the pre-pushed handler 0x0043E670 plus 2 link arguments, `add esp,0xc` at 0x0043E565 is
//   consumed by the following registration group, and `add esp,0x14` at 0x0043E57A is 20 bytes = the two 0x004B8810 arguments plus the three pushes
//   of the 0x00401400 link at 0x0043E560. 0x005042A0 is the transcribed argument-less word getter Sys_GetGlobal16_5042A0, so the two zeros pushed at
//   0x0043E4F0/0x0043E4F2 are inert and survive as TaskType_Register's fourth and fifth slots. 0x00416840 and 0x00416860 are the transcribed
//   argument-less setting-byte getters Sys_GetSettingByte_416840/416860, called with one ignored argument each, so the pre-pushed 0x0043E670 and
//   0x0043E7A0 are those two links' handlers and the getters' bytes are their event ids. 0x00401400 and 0x00401530 read
//   [esp+4]/[esp+8]/[esp+0xC] as (class id, event id, handler). 0x0053C46C is written as a word at 0x0043E511 and read back at 0x0043E51C,
//   0x0043E53C, 0x0043E557 and 0x0043E565; the image holds the dword 0x00000181 there (bytes 81 01 00 00 at 0x0053C46C), so the port initialises the
//   word to 385 exactly as the loader leaves it.
// Behavior/CFG: registers the "GenericPhysicsMagicObj" task type as TaskType_Register("GenericPhysicsMagicObj", 0x768, Sys_GetGlobal16_5042A0(), 0)
//   and stores the returned 16-bit id at 0x0053C46C; there is NO event-0 link - the first link is handler 0x0043E580 on event 1 through 0x00401530,
//   the second is handler 0x0043E650 on event 2 through 0x00401530, then handler 0x0043E670 on the event id from 0x00416840 and handler 0x0043E7A0
//   on the event id from 0x00416860, both through 0x00401400; finally registers the script name "TASKTYPE_GENERICPHYSICSMAGICOBJ" for the id through
//   0x004B8810. Straight-line, no branch, no loop.
// Status/limits: BUILD_VERIFIED; runtime UNOBSERVED. Handlers 0x0043E580, 0x0043E650, 0x0043E670 and 0x0043E7A0 are raw code immediates in retail and
//   are not transcribed yet (native gaps). 0x0053C46C has no other owner in the tree, so this file defines it with its image value. The class id
//   pushed at 0x0043E510 is TaskType_Register's return value in EAX, whose meaningful part is the 16-bit id; retail stores it to 0x0053C46C at
//   0x0043E511 after pushing it, and the port reads the stored word back, which is the same value. Domain: dynobj/, because this is a dynamic object
//   class (the dynobj folder already hosts DynCubeObj_Init and SplinePathDynCubeObj_Init), unlike the Computer registrars which live in core/.
#include "igi/retail/core/task_types.hpp"

#include <cstdint>

extern "C" std::uint16_t Sys_GetGlobal16_5042A0(void);   // 0x005042A0: argument-less word getter
extern "C" std::uint8_t Sys_GetSettingByte_416840(void); // 0x00416840: argument-less setting byte getter
extern "C" std::uint8_t Sys_GetSettingByte_416860(void); // 0x00416860: argument-less setting byte getter
extern "C" void Script_QHashNodeRegister(void* const arg1, void* const value); // 0x004B8810

extern "C" {

std::uint16_t g_53c46c = 385U; // retail 0x0053C46C: the "GenericPhysicsMagicObj" task type id (image dword 0x00000181)

void Task_DefineGenericPhysicsMagicObj()
{
    // 0x0043E4F0: push 0 (inert fifth slot); 0x0043E4F2: push 0 (the fourth TaskType_Register argument);
    // 0x0043E4F4: call 0x5042A0; 0x0043E4F9: push eax; 0x0043E4FA: push 0x768; 0x0043E4FF: push "GenericPhysicsMagicObj"; 0x0043E504: call 0x401900.
    g_53c46c = TaskType_Register("GenericPhysicsMagicObj",   // 0x0043E4FF: push 0x0053C490
                                 0x768U,                     // 0x0043E4FA: push 0x768 (object size)
                                 Sys_GetGlobal16_5042A0(),   // 0x0043E4F4: call 0x5042A0 (parent type)
                                 0U);                        // 0x0043E4F2: push 0 (task list count)
    // 0x0043E511: mov word [0x53C46C],ax.

    const std::uint32_t class_id = static_cast<std::uint32_t>(::g_53c46c); // 0x0043E51C/0x0043E53C/0x0043E557: read back from 0x53C46C

    // 0x0043E509: push 0x0043E580 (pre-pushed third argument); 0x0043E50E: push 1; 0x0043E510: push eax; 0x0043E517: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(class_id), // argument 1
                                           1U,                                 // 0x0043E50E: push 1 (argument 2)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x0043E580U))); // 0x0043E509: push handler

    // 0x0043E522: push 0x0043E650 (pre-pushed third argument); 0x0043E527: push 2; 0x0043E529: push eax; 0x0043E52A: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(class_id), // argument 1
                                           2U,                                 // 0x0043E527: push 2 (argument 2)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x0043E650U))); // 0x0043E522: push handler
    // 0x0043E52F: add esp,0x2c - 11 pushes above.

    // 0x0043E532: push 0x0043E670 (pre-pushed third argument); 0x0043E537: call 0x416840; 0x0043E543: push eax; 0x0043E544: push ecx; 0x0043E545: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(class_id),      // argument 1
                                static_cast<std::uint32_t>(Sys_GetSettingByte_416840()), // 0x0043E543: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x0043E670U))); // 0x0043E532: push handler (argument 3)
    // 0x0043E54A: add esp,0xc - 3 pushes above.

    // 0x0043E54D: push 0x0043E7A0 (pre-pushed third argument); 0x0043E552: call 0x416860; 0x0043E55E: push eax; 0x0043E55F: push edx; 0x0043E560: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(class_id),      // argument 1
                                static_cast<std::uint32_t>(Sys_GetSettingByte_416860()), // 0x0043E55E: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x0043E7A0U))); // 0x0043E54D: push handler (argument 3)

    // 0x0043E565: mov eax,[0x53C46C]; 0x0043E56A: and eax,0xffff; 0x0043E56F: push eax; 0x0043E570: push "TASKTYPE_GENERICPHYSICSMAGICOBJ"; 0x0043E575: call 0x4B8810.
    Script_QHashNodeRegister(const_cast<char*>("TASKTYPE_GENERICPHYSICSMAGICOBJ"), // 0x0043E570: push 0x0053C470
                             reinterpret_cast<void*>(static_cast<std::uintptr_t>(class_id))); // 0x0043E56F: push eax
    // 0x0043E57A: add esp,0x14 - the two registration arguments plus the three link pushes above.
} // 0x0043E57D: ret.

} // extern "C"
