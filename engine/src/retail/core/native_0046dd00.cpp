// Native: Task_DefineGunx2 (behavior-derived placeholder name; retail symbol unknown); retail VA 0x0046DD00-0x0046DDBD.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 190 bytes, 1 block, 50 instructions.
// Tools/evidence: radare2 6.2.0 `af` + `pdf` over the exact image (single block, single ret, so the 50-instruction and 190-byte counts agree
//   exactly); callee bodies re-read at their VAs to fix argument layout - 0x004D9FA0, 0x004F1A70, 0x004C56F0 and 0x00416830 are argument-less
//   getters (`mov ax/al,[abs]; ret`), while 0x00401400 and 0x00401530 read [esp+4]/[esp+8]/[esp+0xC] as (class id, event id, handler) after
//   their prologues, so each single push that precedes a getter call is that link call's pre-pushed handler slot, not a getter argument.
//   Literals 0x0053FE7C "Gunx2" and 0x0053FE6C "TASKTYPE_GUNX2" read as raw .rdata bytes.
// Behavior/CFG: registers the "Gunx2" task type with TaskType_Register("Gunx2", 0x138, <word getter 0x4D9FA0>, 0) and stores the returned
//   16-bit id at 0x0053FE68; links the id to handler 0x0046DDC0 on event 0 through QTask_LinkQTaskEventHandler, to handlers 0x0046DE70 and
//   0x0046DEE0 on events 1 and 2 through QTask_LinkQTaskEventHandlerNoContextFn, then links handlers 0x004D3710, 0x00472880 and 0x0045E3D0
//   with the byte getters 0x004F1A70, 0x004C56F0 and 0x00416830 supplying the event ids; finally registers the script name
//   "TASKTYPE_GUNX2" for the id through Script_QHashNodeRegister.
// Status/limits: TRANSCRIBED. The four `add esp` cleanups independently bound the call set: 0x38 = 14 pushes (2 shared slots + 3 register
//   arguments + 3*3 link arguments), then 0xC three times (one pre-pushed handler slot + two link arguments each), then 0x14 (handler slot +
//   two link arguments + two script-registration arguments). Getter results are zero-extended into the 32-bit event-id argument; retail passes
//   them in EAX with untouched upper bytes, and the callee masks the low byte/word, so the index is identical. Handlers 0x0046DDC0,
//   0x0046DE70, 0x0046DEE0, 0x00472880 and 0x0045E3D0 are raw code immediates in retail and are not transcribed yet (native gaps); 0x004D3710
//   does have a transcription in the tree but is still passed as the address constant retail pushes. Runtime UNOBSERVED.
#include "igi/retail/core/task_types.hpp"

#include <cstdint>

extern "C" std::uint16_t Sys_GetGlobal16_4D9FA0();                              // 0x004D9FA0
extern "C" std::uint8_t Script_PhaseIdGetE5_4F1A70();                           // 0x004F1A70
extern "C" std::uint8_t DynObj_GetByte_4C56F0();                                // 0x004C56F0
extern "C" std::uint8_t Sys_GetSettingByte_416830();                            // 0x00416830
extern "C" void Script_QHashNodeRegister(void* const arg1, void* const value);  // 0x004B8810

extern "C" {

std::uint16_t g_53fe68 = 0U; // retail 0x0053FE68: the "Gunx2" task type id written here

void Task_DefineGunx2()
{
    // 0x0046DD00: push 0 (inert slot below the arguments); 0x0046DD02: push 0 (fourth TaskType_Register argument);
    // 0x0046DD04: call 0x4D9FA0; 0x0046DD09: push eax; 0x0046DD0A: push 0x138; 0x0046DD0F: push "Gunx2"; 0x0046DD14: call 0x401900.
    g_53fe68 = TaskType_Register("Gunx2",                 // 0x0046DD0F: push 0x0053FE7C
                                 0x138U,                  // 0x0046DD0A: push 0x138 (object size)
                                 Sys_GetGlobal16_4D9FA0(), // 0x0046DD04: call 0x004D9FA0 (parent type)
                                 0U);                     // 0x0046DD02: push 0 (task list count)
    // 0x0046DD21: mov word [0x53FE68],ax.

    const std::uint32_t class_id = static_cast<std::uint32_t>(g_53fe68); // 0x0046DD2C/0x0046DD3F/0x0046DD60/0x0046DD7C/0x0046DD95
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(class_id),    // 0x0046DD20: push eax (argument 1)
                                0U,                                      // 0x0046DD1E: push 0 (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x0046DDC0U))); // 0x0046DD19: push handler (argument 3)
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(class_id), // 0x0046DD39: push eax (argument 1)
                                           1U,                               // 0x0046DD37: push 1 (argument 2)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x0046DE70U))); // 0x0046DD32: push handler
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(class_id), // 0x0046DD4D: push ecx (argument 1)
                                           2U,                               // 0x0046DD4B: push 2 (argument 2)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x0046DEE0U))); // 0x0046DD46: push handler
    // 0x0046DD53: add esp,0x38 - 14 pushes above (2 shared + 3 register arguments + 3*3 link arguments).

    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(class_id), // 0x0046DD68: push edx (argument 1)
                                static_cast<std::uint32_t>(Script_PhaseIdGetE5_4F1A70()), // 0x0046DD5B/0x0046DD67: call, push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x004D3710U))); // 0x0046DD56: pre-pushed handler (argument 3)
    // 0x0046DD6E: add esp,0xC - handler slot + two link arguments.

    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(class_id), // 0x0046DD82: push eax (argument 1)
                                static_cast<std::uint32_t>(DynObj_GetByte_4C56F0()), // 0x0046DD76/0x0046DD7B: call, push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00472880U))); // 0x0046DD71: pre-pushed handler
    // 0x0046DD88: add esp,0xC.

    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(class_id), // 0x0046DD9D: push ecx (argument 1)
                                static_cast<std::uint32_t>(Sys_GetSettingByte_416830()), // 0x0046DD90/0x0046DD9C: call, push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x0045E3D0U))); // 0x0046DD8B: pre-pushed handler

    // 0x0046DDA3/0x0046DDA9: load word [0x53FE68] and mask 0xFFFF; 0x0046DDB0: push 0x0053FE6C; 0x0046DDB5: call 0x4B8810.
    Script_QHashNodeRegister(const_cast<char*>("TASKTYPE_GUNX2"),
                             reinterpret_cast<void*>(static_cast<std::uintptr_t>(class_id)));
    // 0x0046DDBA: add esp,0x14 - handler slot + two link arguments + two script-registration arguments; 0x0046DDBD: ret.
}

} // extern "C"
