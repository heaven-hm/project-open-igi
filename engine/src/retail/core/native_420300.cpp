// Native: MenuTextConditional_Register_420300 (placeholder name; retail symbol unknown; the class-name string it registers is "MenuTextConditional" at 0x0053B744). retail VA 0x00420300-0x004204F0 (497 bytes, 1 block, 0 edges, 147 instructions).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 `pd` over the exact image plus capstone 5.0.7 aligned from the verified entry 0x00420300; the two listings agree. Extent: entry `sub esp,0x218`, `c3` at 0x004204F0, and the byte after the ret is 0x90 (0x004204F1 onward is alignment padding, NOT part of the routine), so the extent is 497 bytes. Frame `sub esp,0x218`, epilogue `add esp,0x230` = 0x218 plus 0x18 of pending arguments. Push accounting, replayed with esp simulated and reconciling with zero slack: `add esp,0x20` at 0x00420332 = 2 (0x00424850 pre-zeros) + 3 (0x00401900 with its pre-pushed fourth argument) + 3 (0x00401530); `add esp,0x24` at 0x00420374 = 3+3+3 (the 0x004E7530-driven link, the event-2 link and the 0x00401400 event-0 link); `add esp,0xC` at each of 0x0042038E, 0x004203A9 and 0x004203C4 = one three-argument link each; `add esp,0x10` at 0x004203E5 = 3 (the 0x00424B80-driven link) + 1 (0x004F1950); `add esp,0x18` at each of the first six appends = the 6 pending dwords of one 0x004F1A80; `add esp,0x230` at 0x004204EA = 0x218 frame + 0x18 (the seventh append, deferred). Call census from the 0xE8 enumeration, not from handler immediates: 0x00424850 x1, 0x00401900 x1, 0x00401530 x4, 0x00401400 x4, 0x004E7530 x1, 0x004F1A60 x1, 0x004F1A70 x1, 0x00424890 x1, 0x00424B80 x1, 0x004F1950 x1, 0x004F0FE0 x3, 0x004F0FA0 x1, 0x004F0FC0 x3, 0x004F1A80 x7; 30 call instructions in all, no branch of any kind.
// Behavior/CFG: one straight-line block, no branch, no loop. Reads the parent type through 0x00424850, registers "MenuTextConditional" with a 0x218-byte object through 0x00401900 and stores the 16-bit id at 0x0053B700 (image word 0x0181, the not-registered sentinel). Then makes eight handler links on that id, in retail order: 0x00401530 to 0x004207E0 on event 1; 0x00401530 to 0x004208F0 on the event word from 0x004E7530; 0x00401530 to 0x004205F0 on event 2; 0x00401400 to 0x00420550 on event 0; 0x00401400 to 0x00420BB0 on the phase id from 0x004F1A60; 0x00401400 to 0x00420640 on the phase id from 0x004F1A70; 0x00401530 to 0x00420BC0 on the word from 0x00424890; 0x00401400 to 0x00420500 on the byte from 0x00424B80. Then allocates a count-7 container through 0x004F1950 into 0x0057BBEC and makes seven six-argument appends through 0x004F1A80, all with arg2 = frame+0x00, arg5 = 7, arg6 = 0, and arg3 = frame+0x54, 0x94, 0x118, 0x16C, 0x1BC, 0x11C, 0xD4 for the getters 0x004F0FE0, 0x004F0FE0, 0x004F0FA0, 0x004F0FC0, 0x004F0FC0, 0x004F0FC0, 0x004F0FE0. NOTE: the arg3 offsets are NOT in ascending order in call order (0x16C, 0x1BC, then 0x11C, 0xD4) - that is the retail order and must not be sorted. NOTE: 0x00420BC0 is shared with the MenuText registrar (engine/src/retail/core/native_420cc0.cpp) - it is one entry point registered by two classes. Both pass it as a cast integer, so it creates no symbol and there is no duplicate definition to collide with; when it is transcribed it must be a single file. NOTE: the frame size 0x218 and the registered object size 0x218 COINCIDE. They are different quantities and one does not imply the other.
// Status/limits: TRANSCRIBED; LINKABLE (the handler entry points this routine registers are raw code immediates in retail and are transcribed by nobody, so they are passed as cast integers and create no link dependency; they stay recorded gaps in the tree, not stubs). STRUCTURALLY_VERIFIED for the CFG only, which is one block with no edges. Runtime UNOBSERVED in both binaries. THE APPEND FRAME OFFSETS ARE DERIVED, NOT MEASURED BY A DEBUGGER: each `lea` is relative to the esp at that instruction; the "high" pointer is computed before the container handle is pushed and the "low" pointer after it, so the raw offsets 0x5C/0x9C/0x120/0x174/0x1C4/0x124/0xDC minus 0x0C map to frame+0x54/0x94/0x118/0x16C/0x1BC/0x11C/0xD4, and the raw 0xC/0x10 map to frame+0x00. Raw `lea` offsets are recorded beside each argument so the arithmetic can be rechecked.
// Linkage note (per the Lead's trap): the two globals this file OWNS are emitted with C++ linkage because they sit in `namespace igi::retail::core`. A future file that declares 0x0053B700 or 0x0057BBEC as `extern "C"` would get a SECOND, distinct variable with NO link error. Declare them with the same namespace, or the ownership must move here.
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

// 0x0053B700: the MenuTextConditional class id. The retail image holds the word 0x0181 here
// (px 4 -> 81 01 00 00), the "not registered" sentinel, NOT zero.
std::uint16_t g_53b700 = 0x0181U; // 0x0053B700
// 0x0057BBEC: the class parameter container allocated at 0x004203E8. The address is past the raw extent of
// .data (.data raw ends at 0x00550000, this is 0x0057BBEC), so the image value is zero.
std::uint32_t g_57bbec = 0U; // 0x0057BBEC

} // namespace igi::retail::core

extern "C" {

extern std::uint16_t NATIVE_DECL Sys_GetGlobal16_424850(void); // 0x00424850: argument-less 16-bit getter (defined in engine/src/retail/sys/native_424850.cpp)
extern std::uint32_t NATIVE_DECL Sys_GetGlobal32_424890(void); // 0x00424890: argument-less 32-bit getter (defined in engine/src/retail/sys/native_424890.cpp)
extern std::uint8_t NATIVE_DECL Sys_GetGlobal8_424B80(void);   // 0x00424B80: argument-less 8-bit getter (defined in engine/src/retail/sys/native_424b80.cpp)
extern std::uint32_t NATIVE_DECL Core_GetEventWord_4E7530();   // 0x004E7530: the current event word
extern std::uint32_t NATIVE_DECL Sys_GetGlobal32_4F0FE0(void); // 0x004F0FE0: `mov eax,[0xA71888]; ret`
extern std::uint32_t NATIVE_DECL Core_GetRenderWordA_4F0FA0(); // 0x004F0FA0
extern std::uint32_t NATIVE_DECL Sys_GetGlobal32_4F0FC0(void); // 0x004F0FC0
extern std::uint8_t Script_PhaseIdGetE4_4F1A60(void);          // 0x004F1A60: argument-less byte getter
extern std::uint8_t Script_PhaseIdGetE5_4F1A70(void);          // 0x004F1A70: argument-less byte getter
extern void* Script_TaskContainer_Alloc(std::int32_t count);   // 0x004F1950
// Declared in engine/src/retail/script/native_4f1a80.cpp: six arguments, args 2-6 are int32_t.
extern void* Script_TaskContainer_Append(std::int32_t* const container, std::int32_t const arg2,
                                         std::int32_t const arg3, std::int32_t const arg4,
                                         std::int32_t const arg5, std::int32_t const arg6);


void NATIVE_DECL MenuTextConditional_Register_420300()
{
    NATIVE_LOG_ENTER("MenuTextConditional_Register_420300", "0x00420300");
    std::uint8_t append_frame[0x218]; // 0x00420300: sub esp,0x218

    // 0x00420306/0x00420308: the two zeros are surplus for the argument-less getter and are consumed as
    // 0x00401900's fourth argument (task list = 0) - they are not arguments to 0x00424850, which reads none.
    const std::uint16_t parent = Sys_GetGlobal16_424850();                    // 0x0042030A
    const std::uint16_t class_id =
        TaskType_Register("MenuTextConditional", 0x218U, parent, 0U);         // 0x0042031A
    igi::retail::core::g_53b700 = class_id;                                   // 0x00420327
    QTask_LinkQTaskEventHandlerNoContextFn(
        class_id, 1U, reinterpret_cast<igi::retail::core::FrameHandler>(0x004207E0));                   // 0x0042032D
    // 0x00420335: the handler immediate is pre-pushed and is the surplus argument of 0x004E7530, which
    // reads none; 0x004E7530's return is the event word.
    QTask_LinkQTaskEventHandlerNoContextFn(
        class_id, Core_GetEventWord_4E7530(), reinterpret_cast<igi::retail::core::FrameHandler>(0x004208F0)); // 0x00420347
    QTask_LinkQTaskEventHandlerNoContextFn(
        class_id, 2U, reinterpret_cast<igi::retail::core::FrameHandler>(0x004205F0));                   // 0x0042035B
    QTask_LinkQTaskEventHandler(
        class_id, 0U, reinterpret_cast<igi::retail::core::FrameHandler>(0x00420550));                   // 0x0042036F
    QTask_LinkQTaskEventHandler(
        class_id, static_cast<std::uint32_t>(Script_PhaseIdGetE4_4F1A60()),
        reinterpret_cast<igi::retail::core::FrameHandler>(0x00420BB0));                                 // 0x00420389
    QTask_LinkQTaskEventHandler(
        class_id, static_cast<std::uint32_t>(Script_PhaseIdGetE5_4F1A70()),
        reinterpret_cast<igi::retail::core::FrameHandler>(0x00420640));                                 // 0x004203A4
    // Each of the last two links pre-pushes its handler immediate, which is the surplus argument of the
    // argument-less getter in front of it.
    QTask_LinkQTaskEventHandlerNoContextFn(
        class_id, Sys_GetGlobal32_424890(), reinterpret_cast<igi::retail::core::FrameHandler>(0x00420BC0));        // 0x004203BF
    QTask_LinkQTaskEventHandler(
        class_id, static_cast<std::uint32_t>(Sys_GetGlobal8_424B80()),
        reinterpret_cast<igi::retail::core::FrameHandler>(0x00420500));                                 // 0x004203D9
    igi::retail::core::g_57bbec = static_cast<std::uint32_t>(
        reinterpret_cast<std::uintptr_t>(Script_TaskContainer_Alloc(7)));     // 0x004203E8
    // Each append: the 0 and the 7 are pre-pushed arguments for the append, not for the getter in front of
    // them, which reads none. They arrive as arg6 (0) and arg5 (7). arg3 offsets are in retail order.
    Script_TaskContainer_Append(
        reinterpret_cast<std::int32_t*>(igi::retail::core::g_57bbec),
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x00)), // 0x00420400: lea edx,[esp+0xC]
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x54)), // 0x004203F6: lea ecx,[esp+0x5C]
        static_cast<std::int32_t>(Sys_GetGlobal32_4F0FE0()), 7, 0);                       // 0x00420407
    Script_TaskContainer_Append(
        reinterpret_cast<std::int32_t*>(igi::retail::core::g_57bbec),
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x00)), // 0x00420421: lea edx,[esp+0x10]
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x94)), // 0x00420418: lea ecx,[esp+0x9C]
        static_cast<std::int32_t>(Sys_GetGlobal32_4F0FE0()), 7, 0);                       // 0x0042042C
    Script_TaskContainer_Append(
        reinterpret_cast<std::int32_t*>(igi::retail::core::g_57bbec),
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x00)), // 0x0042044A: lea edx,[esp+0xC]
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x118)), // 0x0042043D: lea ecx,[esp+0x120]
        static_cast<std::int32_t>(Core_GetRenderWordA_4F0FA0()), 7, 0);                   // 0x00420451
    Script_TaskContainer_Append(
        reinterpret_cast<std::int32_t*>(igi::retail::core::g_57bbec),
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x00)), // 0x0042046F: lea edx,[esp+0xC]
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x16C)), // 0x00420462: lea ecx,[esp+0x174]
        static_cast<std::int32_t>(Sys_GetGlobal32_4F0FC0()), 7, 0);                       // 0x00420476
    Script_TaskContainer_Append(
        reinterpret_cast<std::int32_t*>(igi::retail::core::g_57bbec),
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x00)), // 0x00420494: lea edx,[esp+0xC]
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x1BC)), // 0x00420487: lea ecx,[esp+0x1C4]
        static_cast<std::int32_t>(Sys_GetGlobal32_4F0FC0()), 7, 0);                       // 0x0042049B
    Script_TaskContainer_Append(
        reinterpret_cast<std::int32_t*>(igi::retail::core::g_57bbec),
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x00)), // 0x004204B9: lea edx,[esp+0xC]
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x11C)), // 0x004204AC: lea ecx,[esp+0x124]
        static_cast<std::int32_t>(Sys_GetGlobal32_4F0FC0()), 7, 0);                       // 0x004204C0
    Script_TaskContainer_Append(
        reinterpret_cast<std::int32_t*>(igi::retail::core::g_57bbec),
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x00)), // 0x004204DE: lea edx,[esp+0xC]
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0xD4)), // 0x004204D1: lea ecx,[esp+0xDC]
        static_cast<std::int32_t>(Sys_GetGlobal32_4F0FE0()), 7, 0);                       // 0x004204E5
}

} // extern "C"
