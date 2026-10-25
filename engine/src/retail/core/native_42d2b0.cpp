#include "dev_log.hpp"
#include "igi/retail/core/task_types.hpp"

#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

extern "C" std::uint16_t Sys_GetGlobal16_4D9FA0(void);   // 0x004D9FA0
extern "C" std::uint8_t Sys_GetGlobal8_4D9FB0(void);     // 0x004D9FB0
extern "C" std::uint8_t Sys_GetGlobal8_4EE0E0(void);     // 0x004EE0E0
extern "C" std::uint8_t Sys_GetGlobal8_4EE0F0(void);     // 0x004EE0F0
extern "C" std::uint8_t Sys_GetGlobal8_4EE100(void);     // 0x004EE100
extern "C" std::uint8_t Sys_GetGlobal8_47E6F0(void);     // 0x0047E6F0
extern "C" std::uint8_t Sys_GetSettingByte_416840(void); // 0x00416840
extern "C" std::uint8_t Sys_GetSettingByte_416860(void); // 0x00416860
extern "C" std::uint32_t Core_GetEventWord_4E7530(void); // 0x004E7530
extern "C" std::uint32_t DynObj_GetByte_4C56F0(void);    // 0x004C56F0
extern "C" std::uint32_t Sys_AddSlotName_4978D0(const char* name); // 0x004978D0
extern "C" void Sys_EnumRenderModesPair_4978F0(std::uint32_t first, std::uint32_t second); // 0x004978F0
extern "C" void Script_QHashNodeRegister(void* arg1, void* value); // 0x004B8810

std::uint16_t g_57bd10 = 0U;   // retail 0x0057BD10: "Rotor" type id
std::uint32_t g_57bd14 = 0U;   // retail 0x0057BD14: the 0x004978D0 return

// Native: Task_DefineRotor (behavior-derived placeholder name from the byte-identical name literal; retail symbol unknown); retail VA 0x0042D2B0-0x0042D435.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Radare2 6.2.0 `pd` over the exact image; VA 0x0042D2B0..0x0042D435, 389 bytes, 1 block, 108 instructions, 0 branches. Every push and cleanup was replayed with esp simulated, and the six cleanups reconcile with zero slack: `add esp,0x44` at 0x0042D324 closes 17 pending dwords, `0xC` closes 3 at each of 0x0042D33E, 0x0042D359, 0x0042D371, 0x0042D38B and 0x0042D404, `add esp,0x18` at 0x0042D3BA closes 6, and `add esp,0x14` at 0x0042D432 closes 5. Link counts were taken from the 0x00401400/0x00401530 census -- 11 and 2 -- not from handler immediates, which cannot see the four NULL handlers this body carries. All thirteen callees were swept whole-file before being named; 0x004EE0E0 and 0x004EE0F0 were declared extern elsewhere but defined nowhere, so they are transcribed alongside this file.
// Behavior/CFG: one basic block and no local frame. Registers "Rotor" (size 0x140) through 0x00401900, storing the 16-bit id at 0x0057BD10; publishes "RotorPrimitive" through 0x004978D0, storing that return at 0x0057BD14 and passing it with the code immediate 0x0042C630 to 0x004978F0; then makes thirteen handler links on the id. Three carry fixed event ids: 0x00401400 to 0x0042C500 on event 0, 0x00401530 to 0x0042D440 on phase 1 and 0x00401530 to 0x0051B940 on phase 2. Seven take their event id from an argument-less getter with a fixed handler: 0x004D9FB0 to 0x0042D680, 0x00416840 to 0x0042D9B0, 0x00416860 to 0x0042D9F0, 0x004EE0E0 to 0x0042D9F0, 0x004EE0F0 to 0x0042DA60, 0x004EE100 to 0x0042DA70 and the literal 4 to 0x0042D1D0. Three carry a literal NULL handler, with the event supplied by 0x004E7530, 0x004C56F0 and 0x0047E6F0. Finally registers "TASKTYPE_ROTOR" against the id through 0x004B8810 and returns at 0x0042D435.
// Status/limits: STRUCTURALLY_VERIFIED. All thirteen links, the registration, the slot-name publication and the hash registration are transcribed from their own pushes, and the six cleanup sums reconcile with zero slack. Limits: no runtime observation of this native in either binary. Four links carry a literal NULL handler and are invisible to a handler-immediate scan; each was located by replaying esp, where the `push 0` that becomes the handler is simultaneously the surplus argument of the argument-less getter in front of it. 0x0051B940 is shared with the CockpitShield, Gear, Hatch and Afterburner registrars. 0x0057BD10 is written at 16-bit width and read once at 32-bit width with `and ecx,0xffff` at 0x0042D421, so it is declared std::uint16_t on the strength of the writer.
std::int32_t NATIVE_DECL Task_DefineRotor()
{
    NATIVE_LOG_ENTER("Task_DefineRotor", "~0x0042D2B0");
    g_57bd10 = TaskType_Register("Rotor", 0x140U, Sys_GetGlobal16_4D9FA0(), 0U);       // 0x0042D2C4
    g_57bd14 = Sys_AddSlotName_4978D0("RotorPrimitive");                               // 0x0042D2D4, stored 0x0042D2DF
    Sys_EnumRenderModesPair_4978F0(g_57bd14, 0x0042C630U);                             // 0x0042D2E4

    QTask_LinkQTaskEventHandler(g_57bd10, 0U,
        reinterpret_cast<igi::retail::core::FrameHandler>(0x0042C500));                // 0x0042D2F7
    QTask_LinkQTaskEventHandlerNoContextFn(g_57bd10, 1U,
        reinterpret_cast<igi::retail::core::FrameHandler>(0x0042D440));                // 0x0042D30B
    QTask_LinkQTaskEventHandlerNoContextFn(g_57bd10, 2U,
        reinterpret_cast<igi::retail::core::FrameHandler>(0x0051B940));                // 0x0042D31F

    QTask_LinkQTaskEventHandler(g_57bd10, Sys_GetGlobal8_4D9FB0(),
        reinterpret_cast<igi::retail::core::FrameHandler>(0x0042D680));                // 0x0042D339
    QTask_LinkQTaskEventHandler(g_57bd10, Sys_GetSettingByte_416840(),
        reinterpret_cast<igi::retail::core::FrameHandler>(0x0042D9B0));                // 0x0042D354
    QTask_LinkQTaskEventHandler(g_57bd10, Sys_GetSettingByte_416860(),
        reinterpret_cast<igi::retail::core::FrameHandler>(0x0042D9F0));                // 0x0042D36C
    QTask_LinkQTaskEventHandler(g_57bd10, Sys_GetGlobal8_4EE0E0(),
        reinterpret_cast<igi::retail::core::FrameHandler>(0x0042D9F0));                // 0x0042D386
    QTask_LinkQTaskEventHandler(g_57bd10, Sys_GetGlobal8_4EE0F0(),
        reinterpret_cast<igi::retail::core::FrameHandler>(0x0042DA60));                // 0x0042D3A1
    QTask_LinkQTaskEventHandler(g_57bd10, 4U,
        reinterpret_cast<igi::retail::core::FrameHandler>(0x0042D1D0));                // 0x0042D3B5

    // Three NULL-handler links: each `push 0` is surplus for the argument-less getter in
    // front of it and is consumed as the link's third argument.
    QTask_LinkQTaskEventHandlerNoContextFn(g_57bd10, Core_GetEventWord_4E7530(),
        reinterpret_cast<igi::retail::core::FrameHandler>(0));                         // 0x0042D3CC
    QTask_LinkQTaskEventHandler(g_57bd10, DynObj_GetByte_4C56F0(),
        reinterpret_cast<igi::retail::core::FrameHandler>(0));                         // 0x0042D3E4
    QTask_LinkQTaskEventHandler(g_57bd10, Sys_GetGlobal8_47E6F0(),
        reinterpret_cast<igi::retail::core::FrameHandler>(0));                         // 0x0042D416

    Script_QHashNodeRegister(const_cast<char*>("TASKTYPE_ROTOR"),
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(g_57bd10)));               // 0x0042D42D

    return 0;   // 0x0042D435: this path returns without setting EAX
}

} // extern "C"
