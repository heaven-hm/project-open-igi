// Native: ConditionalSound_Register_424E80 (placeholder name; retail symbol unknown; the class-name string it registers is "ConditionalSound" at 0x0053BA00). retail VA 0x00424E80-0x00425048 (457 bytes, 1 block, 0 edges, 135 instructions).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 `pd` over the exact image plus capstone 5.0.7 aligned from the verified entry 0x00424E80; the two listings agree. Extent: entry `sub esp,0x128`, `c3` at 0x00425048, and the byte after the ret is 0x90 (0x00425049 onward is alignment padding, NOT part of the routine), so the extent is 457 bytes. Frame `sub esp,0x128`, epilogue `add esp,0x140` = 0x128 plus 0x18 of pending arguments. Push accounting, replayed with esp simulated and reconciling with zero slack: `add esp,0x38` at 0x00424ED9 = 2 (the two surplus zero words before the parent getter) + 3 (0x00401900 with its pre-pushed fourth argument) + 3 + 3 + 3 (the four links of this group); `add esp,0xC` at each of 0x00424EF4, 0x00424F0E, 0x00424F29 and 0x00424F44 = one three-argument link each; `add esp,0x10` at 0x00424F65 = 3 (the 0x004CEA20-driven link) + 1 (the 0x004F1950 count); `add esp,0x18` at each of the first five appends = the six pending dwords of one 0x004F1A80; `add esp,0x140` at 0x00425042 = 0x128 frame + 0x18 (the sixth append, deferred). Call census from the 0xE8 enumeration, not from handler immediates: 0x004018F0 x1, 0x00401900 x1, 0x00401400 x4, 0x00401530 x4, 0x004F1A60 x1, 0x004F1A70 x1, 0x004F1210 x1, 0x004F1220 x1, 0x004CEA20 x1, 0x004F1950 x1, 0x004F0FC0 x1, 0x004F0FE0 x1, 0x004F1010 x1, 0x004F1000 x3, 0x004F1A80 x6; 28 call instructions in all, no branch of any kind.
// Behavior/CFG: one straight-line block, no branch, no loop. The parent getter is 0x004018F0 (QTask_GetCurrentType, `mov ax,[0x54F928]; ret`) - the same choice 0x00423FC0 makes, and NOT 0x00424850 the way the Menu* registrars do. It registers "ConditionalSound" with a 0x128-byte object through 0x00401900 and stores the 16-bit id at 0x0057BC68 (past the file-backed end of .data, so the image value is 0). Then eight handler links on that id, in retail order: 0x00401400 event 0 -> 0x00425050; 0x00401530 event 1 -> 0x004250D0; 0x00401530 event 2 -> 0x00425340; 0x00401400 the phase byte from 0x004F1A60 -> 0x00425390; 0x00401400 the phase byte from 0x004F1A70 -> 0x004253A0; 0x00401530 the word from 0x004F1210 -> 0x004253C0; 0x00401530 the word from 0x004F1220 -> 0x004254A0; 0x00401400 the byte from 0x004CEA20 -> 0x00425560. Then allocates a count-6 container through 0x004F1950 into 0x0057BC6C and makes six six-argument appends through 0x004F1A80. Every append has arg2 = frame+0x00 and arg5 = 7; arg3 = frame+0x20, 0x70, 0x90, 0xA8, 0xA9, 0xB3 with the argument-less loads 0x004F0FC0, 0x004F0FE0, 0x004F1010, 0x004F1000, 0x004F1000, 0x004F1000 as their fourth argument and arg6 = 0 in all six. THE LAST THREE arg3 VALUES ARE ADJACENT BYTES (0xA8, 0xA9, 0xB3), not dword-aligned slots - they are frame addresses the callee only ever subtracts, so the port keeps them as byte offsets inside the local scratch range and does not align them. NOTE: the frame size 0x128 and the registered object size 0x128 COINCIDE. They are different quantities and one does not imply the other. STRUCTURAL ABSENCE, measured, not an omission: this registrar makes NO 0x004B8810 and no 0x004B8890 registration - the only script-facing thing it does is the container append set.
// Status/limits: TRANSCRIBED; LINKABLE (the eight handler-link entry points this routine registers are raw code immediates in retail and are transcribed by nobody, so they are passed as cast integers and create no link dependency; they stay recorded gaps in the tree, not stubs). STRUCTURALLY_VERIFIED for the CFG only, which is one block with no edges. BUILD_VERIFIED (i686-w64-mingw32-g++ -std=c++20 -O2 -Wall -Wextra single-file compile clean). Runtime UNOBSERVED in both binaries. THE APPEND FRAME OFFSETS ARE DERIVED, NOT MEASURED BY A DEBUGGER: an esp simulation over the full instruction stream computes `esp_at_lea + disp - frame_base` for every `lea reg,[esp+X]` and yields frame+0x20/0x00/0x70/0x00/0x90/0x00/0xA8/0x00/0xA9/0x00/0xB3/0x00 from the raw offsets 0x28/0xC/0x78/0x10/0x98/0xC/0xB0/0xC/0xB1/0xC/0xBB/0xC. In this routine the high pointer is computed BEFORE the container handle is pushed in all six appends, unlike files 1-3, so each raw offset is recorded beside its argument.
// Ownership note (per the Lead's two-variable trap): the two globals this file OWNS are emitted with C++ linkage because they sit in `namespace igi::retail::core`. A future file that declares 0x0057BC68 or 0x0057BC6C as `extern "C"` would get a SECOND, distinct variable with NO link error. Neither address has any other owner in the tree, and both are past the file-backed end of .data (raw ends at VA 0x00550000), so the image value is zero and no sentinel is being preserved. Domain: core/, because it is a task-type registrar of the guard 0x00402930 family and that is where this band lives; the class name says "sound", which is a naming question for the Lead, not a placement change I should make unilaterally.
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

// 0x0057BC68: the ConditionalSound class id slot. Past the raw extent of .data, so the image value is zero.
std::uint16_t g_57bc68 = 0U; // 0x0057BC68
// 0x0057BC6C: the class parameter container allocated at 0x00424F68. Past raw, image value zero.
std::uint32_t g_57bc6c = 0U; // 0x0057BC6C

} // namespace igi::retail::core

extern "C" {

extern std::uint16_t NATIVE_DECL QTask_GetCurrentType();             // 0x004018F0 (declared in igi/retail/core/task_types.hpp)
extern std::uint32_t NATIVE_DECL Core_GetEventWord_4E7530();         // 0x004E7530: the current event word
extern std::uint8_t Script_PhaseIdGetE4_4F1A60(void);                // 0x004F1A60: argument-less byte getter
extern std::uint8_t Script_PhaseIdGetE5_4F1A70(void);                // 0x004F1A70: argument-less byte getter
extern std::uint32_t NATIVE_DECL Sys_GetGlobal32_4F1210(void);       // 0x004F1210
extern std::uint32_t NATIVE_DECL Sys_GetGlobal32_4F1220(void);       // 0x004F1220
extern std::uint8_t QTask_PhaseIdGet548177_4CEA20(void);             // 0x004CEA20: argument-less byte getter
extern std::uint32_t NATIVE_DECL Sys_GetGlobal32_4F0FC0(void);       // 0x004F0FC0: argument-less load
extern std::uint32_t NATIVE_DECL Sys_GetGlobal32_4F0FE0(void);       // 0x004F0FE0: argument-less load
extern std::uint32_t NATIVE_DECL Sys_GetGlobal32_4F1010(void);       // 0x004F1010: argument-less load
extern std::uint32_t NATIVE_DECL Camera_Get_4F1000(void);            // 0x004F1000: argument-less load
extern void* Script_TaskContainer_Alloc(std::int32_t count);         // 0x004F1950
// Declared in engine/src/retail/script/native_4f1a80.cpp: six arguments, args 2-6 are int32_t.
extern void* Script_TaskContainer_Append(std::int32_t* const container, std::int32_t const arg2,
                                         std::int32_t const arg3, std::int32_t const arg4,
                                         std::int32_t const arg5, std::int32_t const arg6);

void NATIVE_DECL ConditionalSound_Register_424E80()
{
    NATIVE_LOG_ENTER("ConditionalSound_Register_424E80", "0x00424E80");
    std::uint8_t append_frame[0x128]; // 0x00424E80: sub esp,0x128

    // 0x00424E86/0x00424E88: the two zeros are surplus for the argument-less getter and are consumed as
    // 0x00401900's fourth argument (task list = 0). The parent getter is 0x004018F0, not 0x00424850.
    const std::uint16_t parent = QTask_GetCurrentType();                       // 0x00424E8A
    const std::uint16_t class_id =
        TaskType_Register("ConditionalSound", 0x128U, parent, 0U);             // 0x00424E9A
    igi::retail::core::g_57bc68 = class_id;                                    // 0x00424EA7
    QTask_LinkQTaskEventHandler(
        class_id, 0U, reinterpret_cast<igi::retail::core::FrameHandler>(0x00425050)); // 0x00424EAD
    QTask_LinkQTaskEventHandlerNoContextFn(
        class_id, 1U, reinterpret_cast<igi::retail::core::FrameHandler>(0x004250D0)); // 0x00424EC0
    QTask_LinkQTaskEventHandlerNoContextFn(
        class_id, 2U, reinterpret_cast<igi::retail::core::FrameHandler>(0x00425340)); // 0x00424ED4
    QTask_LinkQTaskEventHandler(
        class_id, static_cast<std::uint32_t>(Script_PhaseIdGetE4_4F1A60()),
        reinterpret_cast<igi::retail::core::FrameHandler>(0x00425390));        // 0x00424EEF
    QTask_LinkQTaskEventHandler(
        class_id, static_cast<std::uint32_t>(Script_PhaseIdGetE5_4F1A70()),
        reinterpret_cast<igi::retail::core::FrameHandler>(0x004253A0));        // 0x00424F09
    // 0x00424F11/0x00424F2C: each handler immediate is pre-pushed and is the surplus argument of the
    // argument-less getter in front of it.
    QTask_LinkQTaskEventHandlerNoContextFn(
        class_id, Sys_GetGlobal32_4F1210(),
        reinterpret_cast<igi::retail::core::FrameHandler>(0x004253C0));        // 0x00424F24
    QTask_LinkQTaskEventHandlerNoContextFn(
        class_id, Sys_GetGlobal32_4F1220(),
        reinterpret_cast<igi::retail::core::FrameHandler>(0x004254A0));        // 0x00424F3F
    QTask_LinkQTaskEventHandler(
        class_id, static_cast<std::uint32_t>(QTask_PhaseIdGet548177_4CEA20()),
        reinterpret_cast<igi::retail::core::FrameHandler>(0x00425560));        // 0x00424F59
    igi::retail::core::g_57bc6c = static_cast<std::uint32_t>(
        reinterpret_cast<std::uintptr_t>(Script_TaskContainer_Alloc(6)));      // 0x00424F68
    // Each append: the two pre-pushed dwords arrive as arg6 (0) and arg5 (7). arg3 offsets are retail order;
    // the last three are adjacent bytes (0xA8, 0xA9, 0xB3) and are deliberately not aligned or sorted.
    Script_TaskContainer_Append(
        reinterpret_cast<std::int32_t*>(igi::retail::core::g_57bc6c),
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x00)), // 0x00424F80: lea edx,[esp+0xC]
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x20)), // 0x00424F76: lea ecx,[esp+0x28]
        static_cast<std::int32_t>(Sys_GetGlobal32_4F0FC0()), 7, 0);                       // 0x00424F87
    Script_TaskContainer_Append(
        reinterpret_cast<std::int32_t*>(igi::retail::core::g_57bc6c),
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x00)), // 0x00424F9E: lea edx,[esp+0x10]
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x70)), // 0x00424F98: lea ecx,[esp+0x78]
        static_cast<std::int32_t>(Sys_GetGlobal32_4F0FE0()), 7, 0);                       // 0x00424FA9
    Script_TaskContainer_Append(
        reinterpret_cast<std::int32_t*>(igi::retail::core::g_57bc6c),
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x00)), // 0x00424FC7: lea edx,[esp+0xC]
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x90)), // 0x00424FBA: lea ecx,[esp+0x98]
        static_cast<std::int32_t>(Sys_GetGlobal32_4F1010()), 7, 0);                       // 0x00424FCE
    Script_TaskContainer_Append(
        reinterpret_cast<std::int32_t*>(igi::retail::core::g_57bc6c),
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x00)), // 0x00424FEC: lea edx,[esp+0xC]
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0xA8)), // 0x00424FDF: lea ecx,[esp+0xB0]
        static_cast<std::int32_t>(Camera_Get_4F1000()), 7, 0);                            // 0x00424FF3
    Script_TaskContainer_Append(
        reinterpret_cast<std::int32_t*>(igi::retail::core::g_57bc6c),
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x00)), // 0x00425011: lea edx,[esp+0xC]
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0xA9)), // 0x00425004: lea ecx,[esp+0xB1]
        static_cast<std::int32_t>(Camera_Get_4F1000()), 7, 0);                            // 0x00425018
    Script_TaskContainer_Append(
        reinterpret_cast<std::int32_t*>(igi::retail::core::g_57bc6c),
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x00)), // 0x00425036: lea edx,[esp+0xC]
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0xB3)), // 0x00425029: lea ecx,[esp+0xBB]
        static_cast<std::int32_t>(Camera_Get_4F1000()), 7, 0);                            // 0x0042503D
}

} // extern "C"
