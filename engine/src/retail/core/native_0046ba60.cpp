// Native: Task_DefineComputer (behavior-derived placeholder name; retail symbol unknown); retail VA 0x0046BA60-0x0046BB06.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 167 bytes, 1 block, 45 instructions.
// Tools/evidence: radare2 6.2.0 `af` + `pdf` over the exact image (single block, single ret, so the 45-instruction and 167-byte counts agree exactly); callee bodies re-read at their VAs for argument layout - 0x004C48C0, 0x00416830, 0x00416850 and 0x00416880 are argument-less getters, while 0x00401400 and 0x00401530 read [esp+4]/[esp+8]/[esp+0xC] as (class id, event id, handler) after their prologues, so the single push that precedes each getter call is that link call's pre-pushed handler slot, not a getter argument. Literal 0x0053FDA0 "Computer" read as raw .rdata bytes.
// Behavior/CFG: registers the "Computer" task type with TaskType_Register("Computer", 0x198, <word getter 0x4C48C0>, 0) and stores the returned 16-bit id at 0x005BE2C4; links that id to handler 0x0046BB10 on event 0 through QTask_LinkQTaskEventHandler, to handlers 0x0046BD50 and 0x0046BF80 on events 1 and 2 through QTask_LinkQTaskEventHandlerNoContextFn, and then to handlers 0x0045F920 three more times, with the byte getters 0x00416830, 0x00416850 and 0x00416880 supplying the three event ids. Unlike the Gunx2 registrar it does not register a TASKTYPE_ script name - retail's body has no Script_QHashNodeRegister call.
// Status/limits: BUILD_VERIFIED. The `add esp` cleanups independently bound the call set: 0x38 = 14 pushes (2 shared slots + 3 register arguments + 3*3 link arguments), then 0xC three times (one pre-pushed handler slot + two link arguments each). Getter results are zero-extended into the 32-bit event-id argument; retail passes them in EAX with untouched upper bytes and the callee masks the low byte/word, so the computed index is identical. Handlers 0x0046BB10, 0x0046BD50, 0x0046BF80 and 0x0045F920 are raw code immediates in retail and are not transcribed yet (native gaps). Runtime UNOBSERVED.
#include "igi/retail/core/task_types.hpp"

#include <cstdint>

extern "C" std::uint16_t Sys_GetGlobal16_4C48C0();          // 0x004C48C0
extern "C" std::uint8_t Sys_GetSettingByte_416830();        // 0x00416830
extern "C" std::uint8_t Sys_GetSettingByte_416850();        // 0x00416850
extern "C" std::uint8_t Sys_GetSettingByte_416880();        // 0x00416880

extern "C" {

std::uint16_t g_5be2c4 = 0U; // retail 0x005BE2C4: the "Computer" task type id written here (image value 0)

void Task_DefineComputer()
{
    // 0x0046BA60: push 0 (inert slot below the arguments); 0x0046BA62: push 0 (fourth TaskType_Register argument);
    // 0x0046BA64: call 0x4C48C0; 0x0046BA69: push eax; 0x0046BA6A: push 0x198; 0x0046BA6F: push "Computer"; 0x0046BA74: call 0x401900.
    g_5be2c4 = TaskType_Register("Computer",              // 0x0046BA6F: push 0x0053FDA0
                                 0x198U,                  // 0x0046BA6A: push 0x198 (object size)
                                 Sys_GetGlobal16_4C48C0(), // 0x0046BA64: call 0x004C48C0 (parent type)
                                 0U);                     // 0x0046BA62: push 0 (task list count)
    // 0x0046BA81: mov word [0x5BE2C4],ax.

    const std::uint32_t class_id = static_cast<std::uint32_t>(g_5be2c4); // 0x0046BA8C/0x0046BA9F/0x0046BAC0/0x0046BADC/0x0046BAF5
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(class_id),    // 0x0046BA80: push eax (argument 1)
                                0U,                                      // 0x0046BA7E: push 0 (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x0046BB10U))); // 0x0046BA79: push handler (argument 3)
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(class_id), // 0x0046BA99: push eax (argument 1)
                                           1U,                               // 0x0046BA97: push 1 (argument 2)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x0046BD50U))); // 0x0046BA92: push handler
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(class_id), // 0x0046BAAD: push ecx (argument 1)
                                           2U,                               // 0x0046BAAB: push 2 (argument 2)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x0046BF80U))); // 0x0046BAA6: push handler
    // 0x0046BAB3: add esp,0x38 - 14 pushes above (2 shared + 3 register arguments + 3*3 link arguments).

    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(class_id), // 0x0046BAC8: push edx (argument 1)
                                static_cast<std::uint32_t>(Sys_GetSettingByte_416830()), // 0x0046BABB/0x0046BAC7: call, push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x0045F920U))); // 0x0046BAB6: pre-pushed handler (argument 3)
    // 0x0046BACE: add esp,0xC - handler slot + two link arguments.

    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(class_id), // 0x0046BAE2: push eax (argument 1)
                                static_cast<std::uint32_t>(Sys_GetSettingByte_416850()), // 0x0046BAD6/0x0046BADB: call, push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x0045F920U))); // 0x0046BAD1: pre-pushed handler
    // 0x0046BAE8: add esp,0xC.

    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(class_id), // 0x0046BAFD: push ecx (argument 1)
                                static_cast<std::uint32_t>(Sys_GetSettingByte_416880()), // 0x0046BAF0/0x0046BAFC: call, push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x0045F920U))); // 0x0046BAEB: pre-pushed handler
    // 0x0046BB03: add esp,0xC; 0x0046BB06: ret.
}

} // extern "C"
