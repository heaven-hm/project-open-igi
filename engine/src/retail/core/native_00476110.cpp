// Native: Task_DefineM203 (behavior-derived placeholder name; retail symbol unknown); retail VA 0x00476110-0x004761A5.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 150 bytes, 1 block, 39 instructions.
// Tools/evidence: scripts/registrar_sequence.py 0x00476110 (code_flow=no, decoded calls with every push address and immediate,
//   cleanup=add esp,0x18 at 0x0047619D) cross-checked against radare2 6.2.0 af+afbj (1 block / 39 instructions / 150 bytes); the earlier
//   `add esp,0x38` at 0x00476163 cleans the first group. 0x004C48C0 is an argument-less getter (`mov ax,[0x54F928]`-class leaf) and
//   0x00416830 is the leaf setting getter; 0x00401400/0x00401530 read [esp+4]/[esp+8]/[esp+0xC] as (class id, event id, handler), so the
//   push that precedes the 0x00416830 call is the following link call's pre-pushed handler.
// Behavior/CFG: registers the "M203" task type as TaskType_Register("M203", 0x128, Sys_GetGlobal16_4C48C0(), 0) and stores the returned
//   16-bit id at 0x005BE3B0; links the id to handler 0x004761B0 on event 0 through QTask_LinkQTaskEventHandler and to handlers 0x00476340 and
//   0x00476600 on events 1 and 2 through QTask_LinkQTaskEventHandlerNoContextFn; links the id a third time through QTask_LinkQTaskEventHandler
//   with the event id taken from the setting byte 0x00416830 and handler 0x00416A90; registers the script name "TASKTYPE_M203" for the id with
//   Script_QHashNodeRegister; then resolves "smoke1.spr" through the name-keyed registry at 0x004169D0 and keeps the record pointer at
//   0x005BE3AC.
// Status/limits: TRANSCRIBED. Both cleanups are accounted for independently: `add esp,0x38` is 14 dwords = 2 register slots + 3
//   TaskType_Register arguments + 3*3 link arguments, and `add esp,0x18` is 6 dwords = the pre-pushed handler slot + 2 link arguments + 2
//   script-registration arguments + 1 registry-lookup argument. Handlers 0x004761B0, 0x00476340, 0x00476600 and 0x00416A90 are raw code
//   immediates in retail and are not transcribed yet (native gaps). Runtime UNOBSERVED.
#include "igi/retail/core/task_types.hpp"

#include <cstdint>

extern "C" std::uint16_t Sys_GetGlobal16_4C48C0();              // 0x004C48C0: argument-less word getter
extern "C" std::uint8_t Sys_GetSettingByte_416830();            // 0x00416830: argument-less setting byte getter
extern "C" void Script_QHashNodeRegister(void* const arg1, void* const value); // 0x004B8810
extern "C" void* Sys_LookupOrAppendRecord_4169D0(char const* name); // 0x004169D0

extern "C" {

std::uint16_t g_5be3b0 = 0U; // retail 0x005BE3B0: the "M203" task type id written here (image value 0x0131)
std::uint32_t g_5be3ac = 0U; // retail 0x005BE3AC: record pointer returned by 0x004169D0 for "smoke1.spr"

void Task_DefineM203()
{
    // 0x00476110: push 0 (inert slot beyond the fourth argument); 0x00476112: push 0 (the fourth TaskType_Register argument);
    // 0x00476114: call 0x4C48C0; 0x00476119: push eax; 0x0047611A: push 0x128; 0x0047611F: push "M203"; 0x00476124: call 0x401900.
    g_5be3b0 = TaskType_Register("M203",                     // 0x0047611F: push 0x00540664
                                 0x128U,                     // 0x0047611A: push 0x128 (object size)
                                 Sys_GetGlobal16_4C48C0(),   // 0x00476114: call 0x4C48C0 (parent type)
                                 0U);                        // 0x00476112: push 0 (task list count)
    // 0x00476131: mov word [0x5BE3B0],ax.

    const std::uint32_t class_id = static_cast<std::uint32_t>(g_5be3b0); // 0x00476130: push eax; 0x00476149: push eax; 0x0047615D: push ecx
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(class_id),    // argument 1
                                0U,                                      // 0x0047612E: push 0 (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x004761B0U))); // 0x00476129: push handler (argument 3)
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(class_id), // argument 1
                                           1U,                               // 0x00476147: push 1 (argument 2)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x00476340U))); // 0x00476142: push handler
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(class_id), // argument 1
                                           2U,                               // 0x0047615B: push 2 (argument 2)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x00476600U))); // 0x00476156: push handler
    // 0x00476163: add esp,0x38 - 14 pushes above.

    // 0x00476166: push 0x00416A90 (pre-pushed third argument); 0x0047616B: call 0x416830; 0x00476171: push eax; 0x00476177: push edx;
    // 0x00476179: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(class_id),    // 0x00476178: push edx (argument 1)
                                static_cast<std::uint32_t>(Sys_GetSettingByte_416830()), // 0x00476171: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00416A90U))); // 0x00476166: push handler (argument 3)

    // 0x00476183: mov eax,[0x5BE3B0]; 0x00476189: push "TASKTYPE_M203"; 0x0047618E: call 0x4B8810.
    Script_QHashNodeRegister(const_cast<char*>("TASKTYPE_M203"),          // 0x00476189: push 0x00540654
                             reinterpret_cast<void*>(static_cast<std::uintptr_t>(g_5be3b0))); // 0x00476180/0x00476188: mov eax,[0x5BE3B0]; push eax

    // 0x00476193: push "smoke1.spr"; 0x00476198: call 0x4169D0; 0x0047619D: add esp,0x18; 0x004761A0: mov dword [0x5BE3AC],eax.
    g_5be3ac = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Sys_LookupOrAppendRecord_4169D0("smoke1.spr")));
    // 0x004761A5: ret.
}

} // extern "C"
