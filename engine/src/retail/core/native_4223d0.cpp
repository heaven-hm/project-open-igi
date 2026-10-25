// Native: MenuFrame_Register_4223D0 (placeholder name; retail symbol unknown; the class-name string it registers is "MenuFrame" at 0x0053B83C). retail VA 0x004223D0-0x004225E6 (535 bytes, 1 block, 0 edges, 159 instructions).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 `pd` over the exact image plus capstone 5.0.7 aligned from the verified entry 0x004223D0; the two listings agree. Extent: entry `sub esp,0xB4`, `c3` at 0x004225E6, and the byte after the ret is 0x90 (0x004225E7 onward is alignment padding, NOT part of the routine), so the extent is 535 bytes. Frame `sub esp,0xB4`, epilogue `add esp,0xCC` = 0xB4 plus 0x18 of pending arguments. Push accounting, replayed with esp simulated and reconciling with zero slack: `add esp,0x20` at 0x00422416 = 2 (0x00424850 pre-zeros) + 3 (0x00401900 with its pre-pushed fourth argument) + 3 (0x00401530); `add esp,0x24` at 0x00422458 = 3+3+3 (the 0x004E7530-driven link, the event-2 link and the 0x00401400 event-0 link); `add esp,0xC` at 0x00422472 = one three-argument link; `add esp,0x10` at 0x00422494 = 3 (the last link) + 1 (0x004F1950); `add esp,0x18` at each of the first eight appends = the 6 pending dwords of one 0x004F1A80; `add esp,0xCC` at 0x004225E0 = 0xB4 frame + 0x18 (the ninth append, deferred). Call census from the 0xE8 enumeration, not from handler immediates: 0x00424850 x1, 0x00401900 x1, 0x00401530 x3, 0x00401400 x3, 0x004E7530 x1, 0x004F1A60 x1, 0x004F1A70 x1, 0x004F1950 x1, 0x004F0FE0 x1, 0x004F0FA0 x5, 0x004F0FB0 x2, 0x004F1000 x1, 0x004F1A80 x9; 30 call instructions in all, no branch of any kind.
// Behavior/CFG: one straight-line block, no branch, no loop. FIRST, before any registration, it stores 0 at 0x0057BC18 and 0x280 at 0x0057BC1C (0x004223D6/0x004223E0) - retail order, kept. Then reads the parent type through 0x00424850, registers "MenuFrame" with a 0xB4-byte object through 0x00401900 and stores the 16-bit id at 0x0053B838 (image word 0x0181, the not-registered sentinel). Then makes six handler links on that id, in retail order: 0x00401530 to 0x00422A50 on event 1; 0x00401530 to 0x004226D0 on the event word from 0x004E7530; 0x00401530 to 0x004D3710 on event 2; 0x00401400 to 0x004225F0 on event 0; 0x00401400 to 0x00422B00 on the phase id from 0x004F1A60; 0x00401400 to 0x00422640 on the phase id from 0x004F1A70. Then allocates a count-9 container through 0x004F1950 into 0x0057BC10 and makes nine six-argument appends through 0x004F1A80. Every append has arg2 = frame+0x00 and arg5 = 7; arg3 = frame+0x54, 0x20, 0x24, 0x28, 0x2C, 0x38, 0x30, 0x34, 0x4C for the getters 0x004F0FE0, 0x004F0FA0, 0x004F0FA0, 0x004F0FA0, 0x004F0FA0, 0x004F0FA0, 0x004F0FB0, 0x004F0FB0, 0x004F1000. THE ARG6 FIELD IS NOT ALWAYS ZERO: appends 2 through 5 carry arg6 = 0x0057BC18 (the ADDRESS of the slot this routine zeroed at 0x004223D6), because retail pre-pushes that address before the argument-less getter 0x004F0FA0 and the same stack slot is read as arg6 by 0x004F1A80; appends 1 and 6 through 9 carry arg6 = 0. The pre-pushed value is inert for 0x004F0FA0/0x004F0FB0/0x004F1000 (each is a single `mov eax,[global]; ret` that reads no argument) but it is NOT inert overall - dropping it would silently change the appended record, so the port passes it as arg6. NOTE: the arg3 offsets are NOT in ascending order in call order - retail order, must not be sorted. NOTE: the frame size 0xB4 and the registered object size 0xB4 COINCIDE. They are different quantities and one does not imply the other.
// Status/limits: TRANSCRIBED; LINKABLE (the five handler entry points this routine registers are raw code immediates in retail and are transcribed by nobody, so they are passed as cast integers and create no link dependency; they stay recorded gaps in the tree, not stubs). 0x004D3710 IS transcribed (engine/src/retail/script/native_4d3710.cpp) and is referenced as a symbol, so it is a real link edge. STRUCTURALLY_VERIFIED for the CFG only, which is one block with no edges. Runtime UNOBSERVED in both binaries. THE APPEND FRAME OFFSETS ARE DERIVED, NOT MEASURED BY A DEBUGGER: each `lea` is relative to the esp at that instruction, and in this routine the "high" pointer is sometimes computed before the container handle is pushed (append 3) and sometimes after (appends 1, 2, 4-9), so each raw offset is recorded beside its argument and the frame offset is raw minus the bytes pending at that point. Raw `lea` offsets 0x60/0x2C/0x2C/0x34/0x38/0x44/0x3C/0x40/0x58 map to frame+0x54/0x20/0x24/0x28/0x2C/0x38/0x30/0x34/0x4C, and 0xC/0x10 map to frame+0x00.
// Linkage note (per the Lead's trap): the three globals this file OWNS are emitted with C++ linkage because they sit in `namespace igi::retail::core`. A future file that declares 0x0057BC10, 0x0057BC18 or 0x0057BC1C as `extern "C"` would get a SECOND, distinct variable with NO link error. Declare them with the same namespace, or the ownership must move here.
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

namespace igi::retail::core {

// 0x0053B838: the MenuFrame class id. The retail image holds the word 0x0181 here (px 4 -> 81 01 00 00),
// the "not registered" sentinel, NOT zero.
std::uint16_t g_53b838 = 0x0181U; // 0x0053B838
// 0x0057BC10: the class parameter container allocated at 0x00422497. Past the raw extent of .data
// (.data raw ends at 0x00550000), so the image value is zero.
std::uint32_t g_57bc10 = 0U; // 0x0057BC10
// 0x0057BC18: zeroed by this routine at 0x004223D6 and passed BY ADDRESS as arg6 of appends 2-5.
std::uint32_t g_57bc18 = 0U; // 0x0057BC18
// 0x0057BC1C: written 0x280 by this routine at 0x004223E0; image value zero (past raw).
std::uint32_t g_57bc1c = 0U; // 0x0057BC1C

} // namespace igi::retail::core

extern "C" {

extern std::uint16_t NATIVE_DECL Sys_GetGlobal16_424850(void); // 0x00424850: argument-less 16-bit getter (defined in engine/src/retail/sys/native_424850.cpp)
extern std::uint32_t NATIVE_DECL Core_GetEventWord_4E7530();   // 0x004E7530: the current event word
extern std::uint32_t NATIVE_DECL Sys_GetGlobal32_4F0FE0(void); // 0x004F0FE0: `mov eax,[0xA71888]; ret`
extern std::uint32_t NATIVE_DECL Core_GetRenderWordA_4F0FA0(); // 0x004F0FA0: `mov eax,[0xA70C7C]; ret` - reads no argument
extern std::uint32_t NATIVE_DECL Sys_GetGlobal32_4F0FB0(void); // 0x004F0FB0: `mov eax,[0xA70C78]; ret` - reads no argument
extern std::uint32_t NATIVE_DECL Camera_Get_4F1000(void);      // 0x004F1000: `mov eax,[0xA70C90]; ret` - reads no argument
extern std::uint8_t Script_PhaseIdGetE4_4F1A60(void);          // 0x004F1A60: argument-less byte getter
extern std::uint8_t Script_PhaseIdGetE5_4F1A70(void);          // 0x004F1A70: argument-less byte getter
extern void* Script_TaskContainer_Alloc(std::int32_t count);   // 0x004F1950
// Declared in engine/src/retail/script/native_4f1a80.cpp: six arguments, args 2-6 are int32_t.
extern void* Script_TaskContainer_Append(std::int32_t* const container, std::int32_t const arg2,
                                         std::int32_t const arg3, std::int32_t const arg4,
                                         std::int32_t const arg5, std::int32_t const arg6);
extern void NATIVE_DECL QTask_NoOpHandler(void); // 0x004D3710: single `ret`, defined in engine/src/retail/script/native_4d3710.cpp

void NATIVE_DECL MenuFrame_Register_4223D0()
{
    NATIVE_LOG_ENTER("MenuFrame_Register_4223D0", "0x004223D0");
    std::uint8_t append_frame[0xB4]; // 0x004223D0: sub esp,0xB4

    // 0x004223D6/0x004223E0: retail writes these two globals BEFORE it registers anything. Retail order.
    igi::retail::core::g_57bc18 = 0U;      // 0x004223D6: mov dword [0x57BC18],0
    igi::retail::core::g_57bc1c = 0x280U;  // 0x004223E0: mov dword [0x57BC1C],0x280

    // 0x004223EA/0x004223EC: the two zeros are surplus for the argument-less getter and are consumed as
    // 0x00401900's fourth argument (task list = 0) - they are not arguments to 0x00424850, which reads none.
    const std::uint16_t parent = Sys_GetGlobal16_424850();                    // 0x004223EE
    const std::uint16_t class_id =
        TaskType_Register("MenuFrame", 0xB4U, parent, 0U);                    // 0x004223FE
    igi::retail::core::g_53b838 = class_id;                                   // 0x0042240B
    QTask_LinkQTaskEventHandlerNoContextFn(
        class_id, 1U, reinterpret_cast<igi::retail::core::FrameHandler>(0x00422A50)); // 0x00422411
    // 0x00422419: the handler immediate is pre-pushed and is the surplus argument of 0x004E7530, which
    // reads none; 0x004E7530's return is the event word.
    QTask_LinkQTaskEventHandlerNoContextFn(
        class_id, Core_GetEventWord_4E7530(),
        reinterpret_cast<igi::retail::core::FrameHandler>(0x004226D0));      // 0x0042242B
    // 0x00422437: this handler IS transcribed, so it is referenced as a symbol and links.
    QTask_LinkQTaskEventHandlerNoContextFn(
        class_id, 2U, reinterpret_cast<igi::retail::core::FrameHandler>(&QTask_NoOpHandler)); // 0x0042243F
    QTask_LinkQTaskEventHandler(
        class_id, 0U, reinterpret_cast<igi::retail::core::FrameHandler>(0x004225F0)); // 0x00422453
    QTask_LinkQTaskEventHandler(
        class_id, static_cast<std::uint32_t>(Script_PhaseIdGetE4_4F1A60()),
        reinterpret_cast<igi::retail::core::FrameHandler>(0x00422B00));       // 0x0042246D
    QTask_LinkQTaskEventHandler(
        class_id, static_cast<std::uint32_t>(Script_PhaseIdGetE5_4F1A70()),
        reinterpret_cast<igi::retail::core::FrameHandler>(0x00422640));       // 0x00422488
    igi::retail::core::g_57bc10 = static_cast<std::uint32_t>(
        reinterpret_cast<std::uintptr_t>(Script_TaskContainer_Alloc(9)));     // 0x00422497
    // arg6 of appends 2-5 is the ADDRESS 0x0057BC18 (retail pre-pushes it before the argument-less getter
    // and 0x004F1A80 reads that same slot as arg6). It is not zero, and it is not the slot's value.
    std::int32_t const arg6_bc18 =
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(&igi::retail::core::g_57bc18)); // 0x0057BC18
    // Each append: the two pre-pushed dwords arrive as arg6 and arg5 (7). arg3 offsets are in retail order.
    Script_TaskContainer_Append(
        reinterpret_cast<std::int32_t*>(igi::retail::core::g_57bc10),
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x00)), // 0x004224B0: lea eax,[esp+0xC]
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x54)), // 0x004224AC: lea edx,[esp+0x60]
        static_cast<std::int32_t>(Sys_GetGlobal32_4F0FE0()), 7, 0);                       // 0x004224B7
    Script_TaskContainer_Append(
        reinterpret_cast<std::int32_t*>(igi::retail::core::g_57bc10),
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x00)), // 0x004224D6: lea eax,[esp+0xC]
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x20)), // 0x004224D2: lea edx,[esp+0x2C]
        static_cast<std::int32_t>(Core_GetRenderWordA_4F0FA0()), 7, arg6_bc18);           // 0x004224DD
    Script_TaskContainer_Append(
        reinterpret_cast<std::int32_t*>(igi::retail::core::g_57bc10),
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x00)), // 0x004224F7: lea eax,[esp+0x10]
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x24)), // 0x004224F1: lea edx,[esp+0x2C]
        static_cast<std::int32_t>(Core_GetRenderWordA_4F0FA0()), 7, arg6_bc18);           // 0x00422503
    Script_TaskContainer_Append(
        reinterpret_cast<std::int32_t*>(igi::retail::core::g_57bc10),
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x00)), // 0x00422522: lea eax,[esp+0xC]
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x28)), // 0x0042251E: lea edx,[esp+0x34]
        static_cast<std::int32_t>(Core_GetRenderWordA_4F0FA0()), 7, arg6_bc18);           // 0x00422529
    Script_TaskContainer_Append(
        reinterpret_cast<std::int32_t*>(igi::retail::core::g_57bc10),
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x00)), // 0x00422548: lea eax,[esp+0xC]
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x2C)), // 0x00422544: lea edx,[esp+0x38]
        static_cast<std::int32_t>(Core_GetRenderWordA_4F0FA0()), 7, arg6_bc18);           // 0x0042254F
    Script_TaskContainer_Append(
        reinterpret_cast<std::int32_t*>(igi::retail::core::g_57bc10),
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x00)), // 0x0042256B: lea eax,[esp+0xC]
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x38)), // 0x00422567: lea edx,[esp+0x44]
        static_cast<std::int32_t>(Core_GetRenderWordA_4F0FA0()), 7, 0);                   // 0x00422572
    Script_TaskContainer_Append(
        reinterpret_cast<std::int32_t*>(igi::retail::core::g_57bc10),
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x00)), // 0x0042258E: lea eax,[esp+0xC]
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x30)), // 0x0042258A: lea edx,[esp+0x3C]
        static_cast<std::int32_t>(Sys_GetGlobal32_4F0FB0()), 7, 0);                       // 0x00422595
    Script_TaskContainer_Append(
        reinterpret_cast<std::int32_t*>(igi::retail::core::g_57bc10),
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x00)), // 0x004225B1: lea eax,[esp+0xC]
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x34)), // 0x004225AD: lea edx,[esp+0x40]
        static_cast<std::int32_t>(Sys_GetGlobal32_4F0FB0()), 7, 0);                       // 0x004225B8
    Script_TaskContainer_Append(
        reinterpret_cast<std::int32_t*>(igi::retail::core::g_57bc10),
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x00)), // 0x004225D4: lea eax,[esp+0xC]
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x4C)), // 0x004225D0: lea edx,[esp+0x58]
        static_cast<std::int32_t>(Camera_Get_4F1000()), 7, 0);                            // 0x004225DB
}

} // extern "C"
