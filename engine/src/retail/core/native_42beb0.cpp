// Native: Task_DefineAfterburner (behavior-derived placeholder name from the byte-identical name literal; retail symbol unknown); retail VA 0x0042BEB0-0x0042BF9B.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Radare2 6.2.0 `pd` over the exact image; VA 0x0042BEB0..0x0042BF9B, 235 bytes, 1 block, 62 instructions, 0 branches. Every push, `mov reg,[global]` and cleanup was replayed with esp simulated from the prologue, and the four cleanups reconcile exactly: `add esp,0x44` at 0x0042BF24 closes 17 pending dwords, `add esp,0x18` at 0x0042BF52 closes 6, `add esp,0xC` at 0x0042BF6A closes 3, and `add esp,0x14` at 0x0042BF98 closes 5. All seven callees were swept for before declaring them, and all seven already exist in the tree under the names used here.
// Behavior/CFG: one basic block, no branch of its own. Registers "Afterburner" (size 0x140) through 0x00401900, storing the 16-bit id at 0x0057BCF8; publishes the slot name "AfterburnerPrimitive" through 0x004978D0 and stores that call's return at 0x00BC2364, then passes the same return with the code immediate 0x0042C4B0 to 0x004978F0; makes six handler links on the id -- 0x00401400 to 0x0042BC30 on event 0, 0x00401530 to 0x0042BFA0 on phase 1, 0x00401530 to 0x0051B940 on phase 2, 0x00401400 to 0x0042C0B0 on the event 0x004D9FB0 supplies, 0x00401400 to 0x0042BD40 on event 4, and two links whose handler is a literal NULL (0x00401530 with the event 0x004E7530 supplies, and 0x00401400 with the event 0x004C56F0 supplies) -- and finally registers the name "TASKTYPE_AFTERBURNER" against the id through 0x004B8810. Returns at 0x0042BF9B.
// Status/limits: STRUCTURALLY_VERIFIED. All six links, the registration, the slot-name publication and the hash registration are transcribed from their own pushes, and the four cleanup sums reconcile with zero slack. Limits: no runtime observation of this native in either binary. 0x0051B940 is shared with the CockpitShield, Gear and Hatch registrars, so the handler does not belong to this type alone. Two links here carry a literal NULL handler, which a scan for handler immediates cannot see; they were located by argument position after replaying esp, and both are transcribed as an explicit zero. The `push 0` that becomes each NULL handler is simultaneously the surplus argument of the argument-less getter in front of it, which is the deferred-push straddle; the port writes the link as one nested call, preserving the argument values and the call order but not the literal instruction order. 0x0057BCF8 is written at 16-bit width (`mov word`) and read once at 32-bit width with `and ecx,0xffff` at 0x0042BF87, so it is declared std::uint16_t on the strength of the writer.
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

extern "C" std::uint16_t Sys_GetGlobal16_4D9FA0(void);                  // 0x004D9FA0
extern "C" std::uint8_t Sys_GetGlobal8_4D9FB0(void);                    // 0x004D9FB0
extern "C" std::uint32_t Core_GetEventWord_4E7530(void);                // 0x004E7530
extern "C" std::uint32_t DynObj_GetByte_4C56F0(void);                   // 0x004C56F0
// 0x004978D0 returns 0x004B0E70's eax, which 0x0042BEDF stores at 0x00BC2364, so it is
// declared returning a value here. error_show.cpp declares the same address void because it
// discards the result; C linkage does not encode the return type, so the two agree at link.
extern "C" std::uint32_t Sys_AddSlotName_4978D0(const char* name);      // 0x004978D0
extern "C" void Sys_EnumRenderModesPair_4978F0(std::uint32_t first, std::uint32_t second); // 0x004978F0
extern "C" void Script_QHashNodeRegister(void* arg1, void* value);      // 0x004B8810

std::uint16_t g_57bcf8 = 0U;   // retail 0x0057BCF8: "Afterburner" type id
std::uint32_t g_bc2364 = 0U;   // retail 0x00BC2364: the 0x004978D0 return

std::int32_t NATIVE_DECL Task_DefineAfterburner()
{
    // 0x0042BEB0/0x0042BEB2: the two surplus zeros of the argument-less 0x004D9FA0
    // getter. They are pushed first, so they land as the registration's fourth and
    // fifth slots; the function takes four, and the fifth is the surplus.
    g_57bcf8 = TaskType_Register("Afterburner", 0x140U, Sys_GetGlobal16_4D9FA0(), 0U);   // 0x0042BEC4

    // 0x0042BEC9: the name is pushed before the id store at 0x0042BECE, which is
    // retail's instruction order. 0x0042BEDF stores this call's return.
    g_bc2364 = Sys_AddSlotName_4978D0("AfterburnerPrimitive");                          // 0x0042BED4, stored 0x0042BEDF

    Sys_EnumRenderModesPair_4978F0(g_bc2364, 0x0042C4B0U);                              // 0x0042BEE4

    QTask_LinkQTaskEventHandler(g_57bcf8, 0U,
        reinterpret_cast<igi::retail::core::FrameHandler>(0x0042BC30));                 // 0x0042BEF7
    QTask_LinkQTaskEventHandlerNoContextFn(g_57bcf8, 1U,
        reinterpret_cast<igi::retail::core::FrameHandler>(0x0042BFA0));                 // 0x0042BF0B
    QTask_LinkQTaskEventHandlerNoContextFn(g_57bcf8, 2U,
        reinterpret_cast<igi::retail::core::FrameHandler>(0x0051B940));                 // 0x0042BF1F

    // 0x0042BF27: the handler immediate is pushed before the argument-less getter and
    // is consumed as this link's third argument.
    QTask_LinkQTaskEventHandler(g_57bcf8, Sys_GetGlobal8_4D9FB0(),
        reinterpret_cast<igi::retail::core::FrameHandler>(0x0042C0B0));                 // 0x0042BF39
    QTask_LinkQTaskEventHandler(g_57bcf8, 4U,
        reinterpret_cast<igi::retail::core::FrameHandler>(0x0042BD40));                 // 0x0042BF4D

    // 0x0042BF55: `push 0` is surplus for the argument-less 0x004E7530 and is the
    // NULL handler of the link below. The `add esp,0xC` at 0x0042BF6A closes three
    // dwords, which is where the NULL is visible.
    QTask_LinkQTaskEventHandlerNoContextFn(g_57bcf8, Core_GetEventWord_4E7530(),
        reinterpret_cast<igi::retail::core::FrameHandler>(0));                          // 0x0042BF65

    // 0x0042BF6D: same shape -- `push 0` is surplus for the argument-less 0x004C56F0
    // and is this link's NULL handler. Closed by the `add esp,0x14` at 0x0042BF98.
    QTask_LinkQTaskEventHandler(g_57bcf8, DynObj_GetByte_4C56F0(),
        reinterpret_cast<igi::retail::core::FrameHandler>(0));                          // 0x0042BF7C

    // 0x0042BF81: read at 32-bit width and masked; the writer at 0x0042BECE is 16-bit.
    Script_QHashNodeRegister(const_cast<char*>("TASKTYPE_AFTERBURNER"),
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(g_57bcf8)));                // 0x0042BF93

    return 0;   // 0x0042BF9B: this path returns without setting EAX
}

} // extern "C"
