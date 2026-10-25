// Native: Task_DefineGunShot (behavior-derived placeholder name; retail symbol unknown); retail VA 0x0047B4B0-0x0047B55B.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 172 bytes, 1 block, 45 instructions.
// Tools/evidence: radare2 6.2.0 `af` + `pdf` over the exact image (single block, single ret, so the 45-instruction and 172-byte counts agree exactly); callee bodies re-read at their VAs for argument layout - 0x004018F0 is `mov ax,[0x54F928]; ret` and 0x00413B20 is `mov al,[0x5391AE]; ret`, both argument-less, while 0x00401400 and 0x00401530 read [esp+4]/[esp+8]/[esp+0xC] as (class id, event id, handler) after their prologues, so each single push that precedes a getter call is that link call's pre-pushed handler slot. Literals 0x00540890 "GunShot" and 0x0054087C "TASKTYPE_GUNSHOT" read as raw .rdata bytes.
// Behavior/CFG: registers the "GunShot" task type with TaskType_Register("GunShot", 0xC38, QTask_GetCurrentType(), 0) and stores the returned 16-bit id at 0x005BE3CE; stores the low byte of QTask_AllocateEvent(0) at 0x005BE3CC; links the id to handler 0x0047B560 on event 0 and to handler 0x0047ABD0 on event 4 through QTask_LinkQTaskEventHandler, and to handlers 0x0047B690 and 0x0047C3D0 on events 1 and 2 through QTask_LinkQTaskEventHandlerNoContextFn, then links handler 0x0047C420 with the byte getter 0x00413B20 supplying the event id; finally registers the script name "TASKTYPE_GUNSHOT" for the id through Script_QHashNodeRegister.
// Status/limits: BUILD_VERIFIED. The two `add esp` cleanups independently bound the call set: 0x48 = 18 pushes (2 register slots + 3 register arguments + 1 allocate argument + 4*3 link arguments), then 0x14 = 5 pushes (pre-pushed handler slot + 2 link arguments + 2 script-registration arguments). The byte getter result is zero-extended into the 32-bit event-id argument; retail passes it in EAX with untouched upper bytes and the callee masks the low byte, so the computed index is identical. Handlers 0x0047B560, 0x0047B690, 0x0047C3D0, 0x0047ABD0 and 0x0047C420 are raw code immediates in retail and are not transcribed yet (native gaps). 0x005BE3CC has its definition in sys/native_47b100.cpp and is declared extern here; this routine is retail's writer for it. Runtime UNOBSERVED.
#include "igi/retail/core/task_types.hpp"

#include <cstdint>

extern "C" std::uint8_t Core_GetFlagByte_413B20();                             // 0x00413B20: byte getter, takes no arguments
extern "C" void Script_QHashNodeRegister(void* const arg1, void* const value); // 0x004B8810

extern "C" {

extern std::uint8_t g_5be3cc; // retail 0x005BE3CC: defined in sys/native_47b100.cpp, written by this routine

std::uint16_t g_5be3ce = 0U; // retail 0x005BE3CE: the "GunShot" task type id written here (image value 0)

void Task_DefineGunShot()
{
    // 0x0047B4B0: push 0 (inert slot below the arguments); 0x0047B4B2: push 0 (fourth TaskType_Register argument);
    // 0x0047B4B4: call 0x4018F0; 0x0047B4B9: push eax; 0x0047B4BA: push 0xC38; 0x0047B4BF: push "GunShot"; 0x0047B4C4: call 0x401900.
    g_5be3ce = TaskType_Register("GunShot",          // 0x0047B4BF: push 0x00540890
                                 0xC38U,             // 0x0047B4BA: push 0xC38 (object size)
                                 QTask_GetCurrentType(), // 0x0047B4B4: call 0x4018F0 (parent type)
                                 0U);                // 0x0047B4B2: push 0 (task list count)
    // 0x0047B4CB: mov word [0x5BE3CE],ax.
    g_5be3cc = static_cast<std::uint8_t>(QTask_AllocateEvent(0)); // 0x0047B4C9: push 0; 0x0047B4D1: call 0x4017C0; 0x0047B4D6: mov byte [0x5BE3CC],al

    const std::uint32_t class_id = static_cast<std::uint32_t>(g_5be3ce); // 0x0047B4DB/0x0047B4EE/0x0047B502/0x0047B516/0x0047B536
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(class_id),    // 0x0047B4E8: push eax (argument 1)
                                0U,                                      // 0x0047B4E6: push 0 (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x0047B560U))); // 0x0047B4E1: push handler (argument 3)
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(class_id), // 0x0047B4FC: push ecx (argument 1)
                                           1U,                               // 0x0047B4FA: push 1 (argument 2)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x0047B690U))); // 0x0047B4F5: push handler
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(class_id), // 0x0047B510: push edx (argument 1)
                                           2U,                               // 0x0047B50E: push 2 (argument 2)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x0047C3D0U))); // 0x0047B509: push handler
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(class_id), // 0x0047B523: push eax (argument 1)
                                4U,                                      // 0x0047B521: push 4 (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x0047ABD0U))); // 0x0047B51C: push handler
    // 0x0047B529: add esp,0x48 - 18 pushes above (2 register slots + 3 register arguments + 1 allocate argument + 4*3 link arguments).

    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(class_id), // 0x0047B53E: push ecx (argument 1)
                                static_cast<std::uint32_t>(Core_GetFlagByte_413B20()), // 0x0047B531/0x0047B53D: call, push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x0047C420U))); // 0x0047B52C: pre-pushed handler (argument 3)

    // 0x0047B544/0x0047B546: xor edx,edx then load word [0x5BE3CE]; 0x0047B54E: push 0x0054087C; 0x0047B553: call 0x4B8810.
    Script_QHashNodeRegister(const_cast<char*>("TASKTYPE_GUNSHOT"),
                             reinterpret_cast<void*>(static_cast<std::uintptr_t>(class_id)));
    // 0x0047B558: add esp,0x14 - pre-pushed handler slot + two link arguments + two script-registration arguments; 0x0047B55B: ret.
}

} // extern "C"
