// Native: CarAI_Register_425C80 (placeholder name; retail symbol unknown; the class-name string it registers is "CarAI" at 0x0053BA90). retail VA 0x00425C80-0x00425D86 (263 bytes, 1 block, 0 edges, 78 instructions).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 `pd` over the exact image plus capstone 5.0.7 aligned from the verified entry 0x00425C80; the two listings agree. Extent: entry `sub esp,0x84`, `c3` at 0x00425D86, and the byte after the ret is 0x90 (0x00425D87 onward is alignment padding, NOT part of the routine), so the extent is 263 bytes. Frame `sub esp,0x84`, epilogue `add esp,0x9C` = 0x84 plus 0x18 of pending arguments. Push accounting, replayed with esp simulated and reconciling with zero slack: `add esp,0x38` at 0x00425CD9 = 2 (the two surplus zero words before the parent getter) + 3 (0x00401900 with its pre-pushed fourth argument) + 3 + 3 + 3 (the four links of this group); `add esp,0xC` at each of 0x00425CF4 and 0x00425D0E = one three-argument link each; `add esp,0x10` at 0x00425D15 = 3 (the last link) + 1 (the 0x004F1950 count); `add esp,0x18` at each of the first two appends = the six pending dwords of one 0x004F1A80; `add esp,0x9C` at 0x00425D80 = 0x84 frame + 0x18 (the third append, deferred). Call census from the 0xE8 enumeration, not from handler immediates: 0x004018F0 x1, 0x00401900 x1, 0x00401400 x3, 0x00401530 x2, 0x004F1A60 x1, 0x004F1A70 x1, 0x004F1950 x1, 0x004F0FA0 x3, 0x004F1A80 x3; 16 call instructions in all, no branch of any kind.
// Behavior/CFG: one straight-line block, no branch, no loop. The parent getter is 0x004018F0 (QTask_GetCurrentType, `mov ax,[0x54F928]; ret`), NOT 0x00424850 - three of nine files in this band now make that choice, so the split is a property of the band and not an anomaly in one file. It registers "CarAI" with a 0x84-byte object through 0x00401900 and stores the 16-bit id at 0x0053BA50 (image word 0x0181, the not-registered sentinel). Then five handler links on that id, in retail order: 0x00401400 event 0 -> 0x00425D90; 0x00401530 event 1 -> 0x00425DE0; 0x00401530 event 2 -> 0x004D3710; 0x00401400 the phase byte from 0x004F1A60 -> 0x00426590; 0x00401400 the phase byte from 0x004F1A70 -> 0x004265A0. Then allocates a count-3 container through 0x004F1950 into 0x0057BC74 and makes three six-argument appends through 0x004F1A80, all three with the SAME argument-less load 0x004F0FA0 as their fourth argument, arg2 = frame+0x00, arg5 = 7, arg6 = 0, and arg3 = frame+0x28, 0x20, 0x24. NOTE: those three arg3 values are NOT in ascending order in call order (0x28 then 0x20 then 0x24) - retail order, must not be sorted. NOTE: the frame size 0x84 and the registered object size 0x84 COINCIDE; they are different quantities and one does not imply the other.
// Status/limits: TRANSCRIBED; LINKABLE (of the five handler-link addresses, four are raw code immediates in retail that nobody has transcribed - 0x00425D90, 0x00425DE0, 0x00426590, 0x004265A0 - so they are passed as cast integers and create no link dependency; they stay recorded gaps in the tree, not stubs. 0x004D3710 IS transcribed and is referenced as a symbol, so it is a real link edge). STRUCTURALLY_VERIFIED for the CFG only, which is one block with no edges. BUILD_VERIFIED (i686-w64-mingw32-g++ -std=c++20 -O2 -Wall -Wextra single-file compile clean). Runtime UNOBSERVED in both binaries. THE APPEND FRAME OFFSETS ARE DERIVED, NOT MEASURED BY A DEBUGGER: an esp simulation over the full instruction stream computes `esp_at_lea + disp - frame_base` for every `lea reg,[esp+X]` and yields frame+0x28/0x00, 0x20/0x00 and 0x24/0x00 from the raw offsets 0x30/0xC, 0x28/0xC and 0x2C/0xC. The high pointer is computed before the container handle is pushed in all three appends, as in files 4, 5 and 6.
// Ownership note (per the Lead's two-variable trap): the two globals this file OWNS are emitted with C++ linkage because they sit in `namespace igi::retail::core`. A future file that declares 0x0053BA50 or 0x0057BC74 as `extern "C"` would get a SECOND, distinct variable with NO link error. Neither address has any other owner in the tree (checked before writing); 0x0057BC74 is past the file-backed end of .data, so its image value is zero.
// Domain: core/, per the Lead's ruling that these files are task-type registrars of the guard 0x00402930 family and belong in one folder, whatever subsystem the class they register will later serve. The class name says "CarAI"; the body is core task registration.
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

// 0x0053BA50: the CarAI class id. The retail image holds the word 0x0181 here (px 4 -> 81 01 00 00),
// the "not registered" sentinel, NOT zero.
std::uint16_t g_53ba50 = 0x0181U; // 0x0053BA50
// 0x0057BC74: the class parameter container allocated at 0x00425D18. Past the raw extent of .data, so zero.
std::uint32_t g_57bc74 = 0U; // 0x0057BC74

} // namespace igi::retail::core

extern "C" {

extern std::uint16_t NATIVE_DECL QTask_GetCurrentType();          // 0x004018F0 (declared in igi/retail/core/task_types.hpp)
extern std::uint8_t Script_PhaseIdGetE4_4F1A60(void);             // 0x004F1A60: argument-less byte getter
extern std::uint8_t Script_PhaseIdGetE5_4F1A70(void);             // 0x004F1A70: argument-less byte getter
extern std::uint32_t NATIVE_DECL Core_GetRenderWordA_4F0FA0();    // 0x004F0FA0: `mov eax,[0xA70C7C]; ret` - reads no argument
extern void* Script_TaskContainer_Alloc(std::int32_t count);      // 0x004F1950
extern void NATIVE_DECL QTask_NoOpHandler(void);                  // 0x004D3710: single `ret`
// Declared in engine/src/retail/script/native_4f1a80.cpp: six arguments, args 2-6 are int32_t.
extern void* Script_TaskContainer_Append(std::int32_t* const container, std::int32_t const arg2,
                                         std::int32_t const arg3, std::int32_t const arg4,
                                         std::int32_t const arg5, std::int32_t const arg6);

void NATIVE_DECL CarAI_Register_425C80()
{
    std::uint8_t append_frame[0x84]; // 0x00425C80: sub esp,0x84

    // 0x00425C86/0x00425C88: the two zeros are surplus for the argument-less getter and are consumed as
    // 0x00401900's fourth argument (task list = 0). The parent getter is 0x004018F0, not 0x00424850.
    const std::uint16_t parent = QTask_GetCurrentType();                      // 0x00425C8A
    const std::uint16_t class_id =
        TaskType_Register("CarAI", 0x84U, parent, 0U);                        // 0x00425C9A
    igi::retail::core::g_53ba50 = class_id;                                   // 0x00425CA7
    QTask_LinkQTaskEventHandler(
        class_id, 0U, reinterpret_cast<igi::retail::core::FrameHandler>(0x00425D90)); // 0x00425CAD
    QTask_LinkQTaskEventHandlerNoContextFn(
        class_id, 1U, reinterpret_cast<igi::retail::core::FrameHandler>(0x00425DE0)); // 0x00425CC0
    // 0x00425CCC: this handler IS transcribed, so it is referenced as a symbol and links.
    QTask_LinkQTaskEventHandlerNoContextFn(
        class_id, 2U, reinterpret_cast<igi::retail::core::FrameHandler>(&QTask_NoOpHandler)); // 0x00425CD4
    QTask_LinkQTaskEventHandler(
        class_id, static_cast<std::uint32_t>(Script_PhaseIdGetE4_4F1A60()),
        reinterpret_cast<igi::retail::core::FrameHandler>(0x00426590));       // 0x00425CEF
    QTask_LinkQTaskEventHandler(
        class_id, static_cast<std::uint32_t>(Script_PhaseIdGetE5_4F1A70()),
        reinterpret_cast<igi::retail::core::FrameHandler>(0x004265A0));       // 0x00425D09
    igi::retail::core::g_57bc74 = static_cast<std::uint32_t>(
        reinterpret_cast<std::uintptr_t>(Script_TaskContainer_Alloc(3)));     // 0x00425D18
    // Each append: the two pre-pushed dwords arrive as arg6 (0) and arg5 (7). arg3 offsets are retail order
    // (0x28, 0x20, 0x24) and are deliberately NOT sorted.
    Script_TaskContainer_Append(
        reinterpret_cast<std::int32_t*>(igi::retail::core::g_57bc74),
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x00)), // 0x00425D30: lea edx,[esp+0xC]
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x28)), // 0x00425D26: lea ecx,[esp+0x30]
        static_cast<std::int32_t>(Core_GetRenderWordA_4F0FA0()), 7, 0);                   // 0x00425D37
    Script_TaskContainer_Append(
        reinterpret_cast<std::int32_t*>(igi::retail::core::g_57bc74),
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x00)), // 0x00425D52: lea edx,[esp+0xC]
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x20)), // 0x00425D48: lea ecx,[esp+0x28]
        static_cast<std::int32_t>(Core_GetRenderWordA_4F0FA0()), 7, 0);                   // 0x00425D59
    Script_TaskContainer_Append(
        reinterpret_cast<std::int32_t*>(igi::retail::core::g_57bc74),
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x00)), // 0x00425D74: lea edx,[esp+0xC]
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x24)), // 0x00425D6A: lea ecx,[esp+0x2C]
        static_cast<std::int32_t>(Core_GetRenderWordA_4F0FA0()), 7, 0);                   // 0x00425D7B
}

} // extern "C"
