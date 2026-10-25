// Native: Task_DefineComputerStartup (behavior-derived placeholder name; retail symbol unknown); retail VA 0x004663D0-0x00466471.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 162 bytes, 1 block, 42 instructions.
// Tools/evidence: radare2 6.2.0 af/afbj/pdf with bin.relocs.apply over the exact image (1 block / 42 instructions / 162 bytes); all three cleanups are
//   accounted for independently: `add esp,0x14` at 0x004663E9 is 20 bytes = 5 pushes (2 inert zeros + 3 TaskType_Register arguments), `add esp,0x28`
//   at 0x0046643B is 40 bytes = 10 pushes (three 3-argument links + the 0x004B0E70 name argument), and `add esp,0x18` at 0x0046646E is 24 bytes =
//   6 pushes (the 0x004950E0 pair, the 0x004B0EC0 triple, the 0x004978D0 name argument, and the 0x004978F0 pair). 0x004018F0 is the argument-less
//   QTask_GetCurrentType() getter, so the two zeros pushed at 0x004663D0/0x004663D2 are inert and survive as TaskType_Register's fourth and fifth
//   slots. 0x004E7530 is the transcribed getter Core_GetEventWord_4E7530, called with one ignored argument. 0x00401400/0x00401530 read
//   [esp+4]/[esp+8]/[esp+0xC] as (class id, event id, handler). 0x0053F520 is written as a word at 0x004663EC and read back as a word at 0x004663FD,
//   0x00466409 and 0x0046641D; the image holds the dword 0x00000181 there (bytes 81 01 00 00 at 0x0053F520), so the port initialises the word to 385.
//   RETURN VALUES USED BY THIS CALLER, and the reason this file declares them differently from two existing files: 0x004B0E70's body ends
//   `xor eax,eax; ... scan ...; mov [eax*8+0xA94A80],ecx; mov byte [eax*8+0xA94A84],1; ret`, so EAX is the slot index it just filled, and this caller
//   stores it to 0x005BDC54 and passes it on; 0x004978D0's body is `mov eax,[esp+4]; push eax; call 0x004B0E70; add esp,4; ret`, so it forwards that
//   same index. sys/native_4b0e70.cpp and sys/native_4978d0.cpp currently declare both as void, which drops the value; see Status/limits.
// Behavior/CFG: registers the "ComputerStartup" task type as TaskType_Register("ComputerStartup", 0x90, QTask_GetCurrentType(), 0) and stores the
//   returned 16-bit id at 0x0053F520; links the id to handler 0x00466CF0 on the event id from 0x004E7530, to handler 0x00466C90 on event 1 and to
//   handler 0x00466360 on event 2, all three through 0x00401530; registers the render-method name "ComputerStartup" through 0x004B0E70 and keeps the
//   returned slot index at 0x005BDC54; installs handler 0x00466480 into that slot through 0x004B0EC0 with the dword read from 0x005CA0E0 as its first
//   argument (via the getter 0x004950E0) and the slot index as its second; then registers the same name again through 0x004978D0, keeps the returned
//   index at 0x005BDC50, and enumerates the render-mode pair (that index, 0x00466C60) through 0x004978F0. Straight-line, no branch, no loop.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. TWO EXISTING FILES DISAGREE WITH THIS ONE ON RETURN TYPE and the disagreement is silent at link
//   time because all three are C-linkage: sys/native_4b0e70.cpp:24 defines `void Sys_AddSlot_4B0E70(const char*)` and sys/native_4978d0.cpp defines
//   `void Sys_AddSlotName_4978D0(const char*)`, while the evidence above says both leave the slot index in EAX and this caller uses it. On i386 the
//   current void definitions still leave the right value in EAX, so the runtime path is correct today, but the declared types must be changed to
//   return the index in the same change that links this file, otherwise the next caller to use the value will be reading a type the tree denies.
//   Those two files are outside my current 0x0046xxxx-0x0048xxxx write scope, so I have not edited them. Handlers 0x00466CF0, 0x00466C90, 0x00466360
//   and 0x00466480 are raw code immediates in retail and are not transcribed yet (native gaps); 0x00497920 is the callback 0x004978F0 passes on.
//   0x0053F520, 0x005BDC54 and 0x005BDC50 have no other owner, so this file defines them (the first with its image value 385, the other two zeroed as
//   the loader leaves them). Domain: core/, because it is one of the 0x00402930 startup registrars and belongs to the Computer family.
#include <cstdint>
#include "igi/retail/core/task_types.hpp"


extern "C" std::uint32_t Core_GetEventWord_4E7530();               // 0x004E7530: argument-less event-word getter
extern "C" std::uint32_t Sys_AddSlot_4B0E70(const char* name);     // 0x004B0E70: returns the slot index it filled (see Tools/evidence)
extern "C" std::uint32_t Sys_GetGlobal32_4950E0(void);             // 0x004950E0: argument-less dword getter
extern "C" void Sys_SetSlotWord_4B0EC0(std::int32_t first, std::int32_t second, std::uint32_t value); // 0x004B0EC0
extern "C" std::uint32_t Sys_AddSlotName_4978D0(const char* name); // 0x004978D0: returns the index forwarded from 0x004B0E70
extern "C" void Sys_EnumRenderModesPair_4978F0(std::uint32_t first, std::uint32_t second); // 0x004978F0

extern "C" {

std::uint16_t g_53f520 = 385U; // retail 0x0053F520: the "ComputerStartup" task type id (image dword 0x00000181)
std::uint32_t g_5bdc54 = 0U;   // retail 0x005BDC54: render-method slot index from 0x004B0E70 (image value 0)
std::uint32_t g_5bdc50 = 0U;   // retail 0x005BDC50: slot index from 0x004978D0 (image value 0)

void Task_DefineComputerStartup()
{
    // 0x004663D0: push 0 (inert fifth slot); 0x004663D2: push 0 (the fourth TaskType_Register argument);
    // 0x004663D4: call 0x4018F0; 0x004663D9: push eax; 0x004663DA: push 0x90; 0x004663DF: push "ComputerStartup"; 0x004663E4: call 0x401900.
    g_53f520 = TaskType_Register("ComputerStartup",          // 0x004663DF: push 0x0053F540
                                 0x90U,                      // 0x004663DA: push 0x90 (object size)
                                 QTask_GetCurrentType(),     // 0x004663D4: call 0x4018F0 (parent type)
                                 0U);                        // 0x004663D2: push 0 (task list count)
    // 0x004663E9: add esp,0x14; 0x004663EC: mov word [0x53F520],ax.

    const std::uint32_t class_id = static_cast<std::uint32_t>(::g_53f520); // 0x004663FD/0x00466409/0x0046641D: read back from 0x53F520

    // 0x004663F2: push 0x00466CF0 (pre-pushed third argument); 0x004663F7: call 0x4E7530; 0x004663FC: push eax; 0x00466403: push eax; 0x00466404: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(class_id), // argument 1
                                           Core_GetEventWord_4E7530(),          // 0x004663FC: push eax (argument 2)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x00466CF0U))); // 0x004663F2: push handler

    // 0x00466410: push 0x00466C90 (pre-pushed third argument); 0x00466415: push 1; 0x00466417: push ecx; 0x00466418: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(class_id), // argument 1
                                           1U,                                 // 0x00466415: push 1 (argument 2)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x00466C90U))); // 0x00466410: push handler

    // 0x00466424: push 0x00466360 (pre-pushed third argument); 0x00466429: push 2; 0x0046642B: push edx; 0x0046642C: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(class_id), // argument 1
                                           2U,                                 // 0x00466429: push 2 (argument 2)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x00466360U))); // 0x00466424: push handler

    // 0x00466431: push "ComputerStartup"; 0x00466436: call 0x4B0E70; 0x0046643B: add esp,0x28; 0x0046643E: mov [0x5BDC54],eax.
    g_5bdc54 = Sys_AddSlot_4B0E70("ComputerStartup");        // 0x00466431: push 0x0053F540 (the returned index is stored at 0x005BDC54)

    // 0x00466443: push 0x00466480; 0x00466448: push eax; 0x00466449: call 0x4950E0; 0x0046644E: push eax; 0x0046644F: call 0x4B0EC0.
    Sys_SetSlotWord_4B0EC0(static_cast<std::int32_t>(Sys_GetGlobal32_4950E0()), // 0x0046644E: push eax (argument 1, the dword at 0x005CA0E0)
                           static_cast<std::int32_t>(::g_5bdc54),               // 0x00466448: push eax (argument 2, the slot index)
                           static_cast<std::uint32_t>(0x00466480U));            // 0x00466443: push handler (argument 3)

    // 0x00466454: push "ComputerStartup"; 0x00466459: call 0x4978D0; 0x00466463: push 0x00466C60; 0x00466469: call 0x4978F0.
    g_5bdc50 = Sys_AddSlotName_4978D0("ComputerStartup");    // 0x00466454: push 0x0053F540 (index stored at 0x005BDC50)
    Sys_EnumRenderModesPair_4978F0(::g_5bdc50,               // 0x00466463: push eax (argument 1, the slot index)
                                   static_cast<std::uint32_t>(0x00466C60U)); // 0x0046645E: push 0x00466C60 (argument 2)
    // 0x0046646E: add esp,0x18 - 6 pushes above.
} // 0x00466471: ret.

} // extern "C"
