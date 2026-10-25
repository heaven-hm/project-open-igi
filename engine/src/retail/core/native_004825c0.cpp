// Native: Task_DefineHumanCamera (behavior-derived placeholder name; retail symbol unknown); retail VA 0x004825C0-0x00482642.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 131 bytes, 3 blocks, 34 instructions.
// Tools/evidence: radare2 6.2.0 `af` + `afbj` + `pdf` over the exact image, counted per block so the multi-block body is not truncated (block 1 0x004825C0-0x00482626 linear, block 2 0x00482629-0x00482640 the clearing loop with its test at the bottom, block 3 0x00482642 the ret); 0x004018F0 is `mov ax,[0x54F928]; ret` (argument-less), 0x00401400 and 0x00401530 read [esp+4]/[esp+8]/[esp+0xC] as (class id, event id, handler), so each single push before a getter call is that link call's pre-pushed handler slot. Literal 0x00540994 "HumanCamera" read as raw .rdata bytes.
// Behavior/CFG: registers the "HumanCamera" task type with TaskType_Register("HumanCamera", 0x128, QTask_GetCurrentType(), 0), stores the 16-bit id at 0x00540990 and the low byte of QTask_AllocateEvent(0) at 0x00540992; links handler 0x00482650 on event 0 through QTask_LinkQTaskEventHandler and handlers 0x00482700 and 0x00484CE0 on events 1 and 2 through QTask_LinkQTaskEventHandlerNoContextFn; then walks 128 records of the 8-byte table at 0x005C1188 clearing the dword at each record's start and the byte at record start + 5.
// Status/limits: BUILD_VERIFIED. `add esp,0x3C` bounds the call set independently: 15 pushes = 2 register slots + 3 register arguments + 1 allocate argument + 3*3 link arguments. The clearing loop is retail's own do-while: `mov eax,0x5C118D` then body (dword [eax-5]=0, byte [eax]=0, eax+=8) and the test `cmp eax,0x5C158D; jl` at the BOTTOM, so the body runs at least once and the last iteration runs with eax = 0x005C1585 (slots 0x5C118D + 8*n for n = 0..127). Handlers 0x00482650, 0x00482700 and 0x00484CE0 are raw code immediates in retail and are not transcribed yet (native gaps). Runtime UNOBSERVED.
#include "igi/retail/core/task_types.hpp"

#include <cstdint>

extern "C" {

std::uint16_t g_540990 = 0U;          // retail 0x00540990: the "HumanCamera" task type id written here (image value 0x0181)
std::uint8_t g_540992 = 0U;           // retail 0x00540992: the QTask_AllocateEvent id byte written here (image value 0xFF)
std::uint8_t g_5c1188[0x405] = {};    // retail 0x005C1188-0x005C158C: the 128-record table below cleared by this routine

void Task_DefineHumanCamera()
{
    // 0x004825C0: push 0 (inert slot below the arguments); 0x004825C2: push 0 (fourth TaskType_Register argument);
    // 0x004825C4: call 0x4018F0; 0x004825C9: push eax; 0x004825CA: push 0x128; 0x004825CF: push "HumanCamera"; 0x004825D4: call 0x401900.
    g_540990 = TaskType_Register("HumanCamera",  // 0x004825CF: push 0x00540994
                                 0x128U,          // 0x004825CA: push 0x128 (object size)
                                 QTask_GetCurrentType(), // 0x004825C4: call 0x4018F0 (parent type)
                                 0U);             // 0x004825C2: push 0 (task list count)
    // 0x004825DB: mov word [0x540990],ax.
    g_540992 = static_cast<std::uint8_t>(QTask_AllocateEvent(0)); // 0x004825D9: push 0; 0x004825E1: call 0x4017C0; 0x004825E6: mov byte [0x540992],al

    const std::uint32_t class_id = static_cast<std::uint32_t>(g_540990); // 0x004825EB/0x004825FE/0x00482612
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(class_id),    // 0x004825F8: push eax (argument 1)
                                0U,                                      // 0x004825F6: push 0 (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00482650U))); // 0x004825F1: push handler (argument 3)
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(class_id), // 0x0048260C: push ecx (argument 1)
                                           1U,                               // 0x0048260A: push 1 (argument 2)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x00482700U))); // 0x00482605: push handler
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(class_id), // 0x00482620: push edx (argument 1)
                                           2U,                               // 0x0048261E: push 2 (argument 2)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x00484CE0U))); // 0x00482619: push handler
    // 0x00482626: add esp,0x3C - 15 pushes above (2 register slots + 3 register arguments + 1 allocate argument + 3*3 link arguments).

    // 0x00482629: mov eax,0x5C118D - the first slot is record start + 5, so the offset from 0x005C1188 is 5.
    std::uint32_t slot = 0x5U;
    do
    {
        *reinterpret_cast<std::uint32_t*>(&g_5c1188[slot - 5U]) = 0U; // 0x0048262E: mov dword [eax-5],0
        g_5c1188[slot] = 0U;                                         // 0x00482635: mov byte [eax],0
        slot += 8U;                                                  // 0x00482638: add eax,8
    } while (slot < 0x405U);                                         // 0x0048263B/0x00482640: cmp eax,0x5C158D; jl 0x0048262E
    // 0x00482642: ret.
}

} // extern "C"
