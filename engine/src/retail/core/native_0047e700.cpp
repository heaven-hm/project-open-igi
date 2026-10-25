// Native: Task_DefineFlashBang (behavior-derived placeholder name; retail symbol unknown); retail VA 0x0047E700-0x0047E780.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 129 bytes, 1 block, 34 instructions.
// Tools/evidence: scripts/registrar_sequence.py 0x0047E700 (code_flow=no, 8 decoded calls, cleanup=add esp,0x40 at 0x0047E778, every push address and immediate reported) cross-checked against radare2 6.2.0 af+afbj (1 block / 34 instructions / 129 bytes) - the script and the per-block count agree. Callee bodies re-read at the VAs: 0x004018F0 is `mov ax,[0x54F928]; ret` and 0x004013B20-style getters read no stack arguments, while 0x00401400/0x00401530 read [esp+4]/[esp+8]/[esp+0xC] as (class id, event id, handler), so the push that precedes a getter call is that link call's pre-pushed handler slot.
// Behavior/CFG: allocates one event id with QTask_AllocateEvent(0) and keeps its low byte at 0x00540914; registers the "FlashBang" task type as TaskType_Register("FlashBang", 0x48, QTask_GetCurrentType(), 4) and stores the returned 16-bit id at 0x005C1144; links the id to handler 0x0047E790 on event 0 through QTask_LinkQTaskEventHandler and to handlers 0x0047E820 and 0x004D3710 on events 1 and 2 through QTask_LinkQTaskEventHandlerNoContextFn; then resolves "burn1.tex" and "smoke1.spr" through the two name-keyed registries at 0x00416930 and 0x004169D0 and keeps the returned record pointers at 0x005C1148 and 0x005C114C. There is no Script_QHashNodeRegister call in this body.
// Status/limits: BUILD_VERIFIED. The single cleanup `add esp,0x40` is 16 dwords and accounts for exactly the decoded pushes: 2 register slots + 3 TaskType_Register arguments + 3*3 link arguments + 2 registry-lookup arguments. The fourth TaskType_Register argument is retail's 0x4, not 0 - the pushed immediates put 0x4 at [esp+0x10] at the 0x00401900 call while the deeper 0 sits at [esp+0x14] beyond the fourth argument, and 0x00401900 branches on that argument (test/je at 0x00401986) to call 0x004B0B00 when it is non-zero, so passing 0 here would change behavior. Handlers 0x0047E790, 0x0047E820 and 0x004D3710 are raw code immediates in retail and are not transcribed yet (native gaps); 0x004D3710 has a separate transcription in the tree but is passed as the address retail pushes. Runtime UNOBSERVED.
#include "igi/retail/core/task_types.hpp"

#include <cstdint>

extern "C" void* Sys_LookupOrAppendRecord_416930(char const* name); // 0x00416930
extern "C" void* Sys_LookupOrAppendRecord_4169D0(char const* name); // 0x004169D0

extern "C" {

std::uint8_t g_540914 = 0xFFU; // retail 0x00540914: low byte of the event id allocated here (image value 0xFF)
std::uint16_t g_5c1144 = 0U; // retail 0x005C1144: the "FlashBang" task type id written here
std::uint32_t g_5c1148 = 0U; // retail 0x005C1148: record pointer returned by 0x00416930 for "burn1.tex"
std::uint32_t g_5c114c = 0U; // retail 0x005C114C: record pointer returned by 0x004169D0 for "smoke1.spr"

void Task_DefineFlashBang()
{
    g_540914 = static_cast<std::uint8_t>(QTask_AllocateEvent(0)); // 0x0047E700: push 0; 0x0047E702: call 0x4017C0; 0x0047E707: add esp,4; 0x0047E70A: mov byte [0x540914],al

    // 0x0047E70F: push 0 (inert slot beyond the fourth argument); 0x0047E711: push 4 (the fourth TaskType_Register argument);
    // 0x0047E713: call 0x4018F0; 0x0047E718: push eax; 0x0047E719: push 0x48; 0x0047E71B: push "FlashBang"; 0x0047E720: call 0x401900.
    g_5c1144 = TaskType_Register("FlashBang",     // 0x0047E71B: push 0x00540924
                                 0x48U,            // 0x0047E719: push 0x48 (object size)
                                 QTask_GetCurrentType(), // 0x0047E713: call 0x4018F0 (parent type)
                                 4U);              // 0x0047E711: push 4 (task list count - non-zero in retail)
    // 0x0047E72D: mov word [0x5C1144],ax.

    const std::uint32_t class_id = static_cast<std::uint32_t>(g_5c1144); // 0x0047E738/0x0047E74B
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(class_id),    // 0x0047E72C: push eax (argument 1)
                                0U,                                      // 0x0047E72A: push 0 (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x0047E790U))); // 0x0047E725: push handler (argument 3)
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(class_id), // 0x0047E745: push eax (argument 1)
                                           1U,                               // 0x0047E743: push 1 (argument 2)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x0047E820U))); // 0x0047E73E: push handler
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(class_id), // 0x0047E759: push ecx (argument 1)
                                           2U,                               // 0x0047E757: push 2 (argument 2)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x004D3710U))); // 0x0047E752: push handler

    // 0x0047E75F: push "burn1.tex"; 0x0047E764: call 0x416930; 0x0047E76E: mov dword [0x5C1148],eax
    g_5c1148 = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Sys_LookupOrAppendRecord_416930("burn1.tex")));
    // 0x0047E769: push "smoke1.spr"; 0x0047E773: call 0x4169D0; 0x0047E77B: mov dword [0x5C114C],eax
    g_5c114c = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Sys_LookupOrAppendRecord_4169D0("smoke1.spr")));
    // 0x0047E778: add esp,0x40 - 16 pushes above; 0x0047E780: ret.
}

} // extern "C"
