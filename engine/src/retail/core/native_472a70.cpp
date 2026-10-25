// Native: Task_DefineGunSPAS12 (behavior-derived placeholder name; retail symbol unknown); retail VA 0x00472A70-0x00472AD7.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 104 bytes, 1 block, 27 instructions.
// Tools/evidence: radare2 6.2.0 af/afbj/pdf with bin.relocs.apply over the exact image (1 block / 27 instructions / 104 bytes); both stack cleanups
//   are accounted for independently: `add esp,0x14` at 0x00472A89 is 20 bytes = the three TaskType_Register arguments (name, size, parent) plus the
//   two zeros pushed at 0x00472A70/0x00472A72 before the 0x00477BF0 call, and `add esp,0x20` at 0x00472AD4 is 32 bytes = the pre-pushed handler
//   slot + 3 + 3 link arguments + 2 script-registration arguments. 0x00477BF0 is the transcribed word getter sys/native_477bf0.cpp
//   (`mov ax,[0x005BE3C6]; ret`), so the two zeros it is called with are inert and survive on the stack as TaskType_Register's fourth and fifth
//   slots. 0x00401400 reads [esp+4]/[esp+8]/[esp+0xC] as (class id, event id, handler), so the pushes preceding each 0x00460C10 and each link are
//   that link's pre-pushed handler. 0x005BE350 is written as a word at 0x00472A8C, read back as a dword and masked with 0xffff at
//   0x00472ABD/0x00472AC3.
// Behavior/CFG: registers the "GunSPAS12" task type as TaskType_Register("GunSPAS12", 0x1B0, Sys_GetGlobal16_477BF0(), 0) and stores the returned
//   16-bit id at 0x005BE350; links the id to handler 0x00472AE0 on the event id returned by 0x00460C10 and to handler 0x00472CA0 on event 0, both
//   through 0x00401400 (QTask_LinkQTaskEventHandler); then registers the script name "TASKTYPE_GUNSPAS12" for the id through 0x004B8810. Straight-line,
//   no branch, no loop.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. Handlers 0x00472AE0 and 0x00472CA0 are raw code immediates in retail and are not transcribed yet
//   (native gaps). 0x005BE350 has no other owner and image value 0, so this file defines it; the retail dword read at 0x00472ABD is modelled as the
//   zero-extended word, which is what `and edx,0xffff` produces. Both links use 0x00401400 (the full-context variant): unlike the M203 registrar,
//   this one never calls 0x00401530. The event-id argument is the byte returned by 0x00460C10; retail pushes the full EAX, whose upper bits are
//   unestablished, so the port passes the zero-extended byte.
#include "igi/retail/core/task_types.hpp"

#include <cstdint>

extern "C" std::uint16_t Sys_GetGlobal16_477BF0();                        // 0x00477BF0: argument-less word getter
extern "C" std::uint8_t Task_GetCachedEventId_460C10(void);               // 0x00460C10: lazy event-id allocator
extern "C" void Script_QHashNodeRegister(void* const arg1, void* const value); // 0x004B8810

extern "C" {

std::uint16_t g_5be350 = 0U; // retail 0x005BE350: the "GunSPAS12" task type id written here (image value 0)

void Task_DefineGunSPAS12()
{
    // 0x00472A70: push 0 (inert fifth slot); 0x00472A72: push 0 (the fourth TaskType_Register argument);
    // 0x00472A74: call 0x477BF0; 0x00472A79: push eax; 0x00472A7A: push 0x1B0; 0x00472A7F: push "GunSPAS12"; 0x00472A84: call 0x401900.
    g_5be350 = TaskType_Register("GunSPAS12",                // 0x00472A7F: push 0x00540388
                                 0x1B0U,                     // 0x00472A7A: push 0x1B0 (object size)
                                 Sys_GetGlobal16_477BF0(),   // 0x00472A74: call 0x477BF0 (parent type)
                                 0U);                        // 0x00472A72: push 0 (task list count)
    // 0x00472A8C: mov word [0x5BE350],ax.

    const std::uint32_t class_id = static_cast<std::uint32_t>(::g_5be350); // 0x00472A9D/0x00472AA9/0x00472ABD: the id read back from 0x5BE350

    // 0x00472A92: push 0x00472AE0 (pre-pushed third argument); 0x00472A97: call 0x460C10; 0x00472A9C: push eax; 0x00472AA3: push eax; 0x00472AA4: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(class_id),      // argument 1
                                static_cast<std::uint32_t>(Task_GetCachedEventId_460C10()), // 0x00472A9C: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00472AE0U))); // 0x00472A92: push handler (argument 3)

    // 0x00472AB0: push 0x00472CA0 (pre-pushed third argument); 0x00472AB5: push 0; 0x00472AB7: push ecx; 0x00472AB8: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(class_id),      // argument 1
                                0U,                                        // 0x00472AB5: push 0 (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00472CA0U))); // 0x00472AB0: push handler (argument 3)

    // 0x00472AC9: push edx (the id, masked to 16 bits); 0x00472ACA: push "TASKTYPE_GUNSPAS12"; 0x00472ACF: call 0x4B8810.
    Script_QHashNodeRegister(const_cast<char*>("TASKTYPE_GUNSPAS12"),      // 0x00472ACA: push 0x00540374
                             reinterpret_cast<void*>(static_cast<std::uintptr_t>(class_id))); // 0x00472AC9: push edx
    // 0x00472AD4: add esp,0x20 - 8 pushes above.
}

} // extern "C"
