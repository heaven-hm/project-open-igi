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
extern "C" std::uint8_t Sys_GetGlobal8_489610(void);     // 0x00489610
extern "C" std::uint8_t Sys_GetSettingByte_416840(void); // 0x00416840
extern "C" std::uint8_t Sys_GetSettingByte_416860(void); // 0x00416860
extern "C" std::uint8_t Sys_GetGlobal8_4EE100(void);     // 0x004EE100
extern "C" void Script_QHashNodeRegister(void* arg1, void* value); // 0x004B8810

// Retail 0x0057BD30 is owned by its writer, native 0x00430950 in sys/native_430950.cpp.
extern "C" std::uint16_t g_57bd30;

// Native: Task_DefineWheel (behavior-derived placeholder name from the byte-identical name literal; retail symbol unknown); retail VA 0x0042FF00-0x0042FFF2.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Radare2 6.2.0 `pd` over the exact image; VA 0x0042FF00..0x0042FFF2, 242 bytes, 1 block, 65 instructions, 0 branches. Every push and cleanup was replayed with esp simulated: the body has NO local frame (it never adjusts esp except through pushes), and the four cleanups reconcile with zero slack -- `add esp,0x38` at 0x0042FF53 closes 14 pending dwords, and `add esp,0xC` closes 3 at each of 0x0042FF6E, 0x0042FF88, 0x0042FFA3, 0x0042FFBE, with `add esp,0x14` at 0x0042FFEF closing the final 5. All seven callees were swept for first; 0x004EE100 was absent from the tree and is transcribed alongside this file.
// Behavior/CFG: one basic block, no branch of its own and no local frame. Registers "Wheel" (size 0x158) through 0x00401900, storing the 16-bit id at 0x0057BD30, then makes eight handler links on it: 0x00401400 to 0x00430000 on event 0, 0x00401530 to 0x004301C0 on phase 1, 0x00401530 to 0x00430170 on phase 2, and five further 0x00401400 links whose event ids come from argument-less getters -- 0x004D9FB0 to handler 0x00430640, 0x00416840 to 0x004D3710, 0x00416860 to 0x004D3710, 0x004EE100 to 0x0042DA70 and 0x00489610 to 0x00426D30. Finally registers the name "TASKTYPE_WHEEL" against the id through 0x004B8810 and returns at 0x0042FFF2.
// Status/limits: STRUCTURALLY_VERIFIED. All eight links, the registration and the hash registration are transcribed from their own pushes, and the cleanup sums reconcile with zero slack. Limits: no runtime observation of this native in either binary. 0x004D3710 is used as the handler of two different links here, which is the same shared-handler pattern as 0x0051B940 across three types. Five of the eight links take their event id from an argument-less getter, and in each the handler immediate is pushed before that getter, so the port writes each as one nested call: argument values and call order are preserved, the literal instruction order is not. 0x0057BD30 is written at 16-bit width and read once at 32-bit width with `and ecx,0xffff` at 0x0042FFDE, so it is declared std::uint16_t on the strength of the writer.
std::int32_t NATIVE_DECL Task_DefineWheel()
{
    NATIVE_LOG_ENTER("Task_DefineWheel", "~0x0042FF00");
    // 0x0042FF00/0x0042FF02: the two surplus zeros of the argument-less 0x004D9FA0.
    g_57bd30 = TaskType_Register("Wheel", 0x158U, Sys_GetGlobal16_4D9FA0(), 0U);        // 0x0042FF14

    QTask_LinkQTaskEventHandler(g_57bd30, 0U,
        reinterpret_cast<igi::retail::core::FrameHandler>(0x00430000));                 // 0x0042FF27
    QTask_LinkQTaskEventHandlerNoContextFn(g_57bd30, 1U,
        reinterpret_cast<igi::retail::core::FrameHandler>(0x004301C0));                 // 0x0042FF3A
    QTask_LinkQTaskEventHandlerNoContextFn(g_57bd30, 2U,
        reinterpret_cast<igi::retail::core::FrameHandler>(0x00430170));                 // 0x0042FF4E

    // 0x0042FF56, 0x0042FF71, 0x0042FF8B, 0x0042FFA6, 0x0042FFC1: in each, the handler
    // immediate is pushed before the argument-less getter and consumed by the link.
    QTask_LinkQTaskEventHandler(g_57bd30, Sys_GetGlobal8_4D9FB0(),
        reinterpret_cast<igi::retail::core::FrameHandler>(0x00430640));                 // 0x0042FF69
    QTask_LinkQTaskEventHandler(g_57bd30, Sys_GetSettingByte_416840(),
        reinterpret_cast<igi::retail::core::FrameHandler>(0x004D3710));                 // 0x0042FF83
    QTask_LinkQTaskEventHandler(g_57bd30, Sys_GetSettingByte_416860(),
        reinterpret_cast<igi::retail::core::FrameHandler>(0x004D3710));                 // 0x0042FF9E
    QTask_LinkQTaskEventHandler(g_57bd30, Sys_GetGlobal8_4EE100(),
        reinterpret_cast<igi::retail::core::FrameHandler>(0x0042DA70));                 // 0x0042FFB9
    QTask_LinkQTaskEventHandler(g_57bd30, Sys_GetGlobal8_489610(),
        reinterpret_cast<igi::retail::core::FrameHandler>(0x00426D30));                 // 0x0042FFD3

    Script_QHashNodeRegister(const_cast<char*>("TASKTYPE_WHEEL"),
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(g_57bd30)));                // 0x0042FFEA

    return 0;   // 0x0042FFF2: this path returns without setting EAX
}

} // extern "C"
