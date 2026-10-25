// Native: ModelViewBox_Register_422B40 (placeholder name; retail symbol unknown; the class-name string it registers is "ModelViewBox" at 0x0053B858). retail VA 0x00422B40-0x00422C8E (335 bytes, 1 block, 0 edges, 89 instructions).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 `af`/`pdf` over the exact image. Frame `sub esp,0xC8`, epilogue `add esp,0xE0` = 0xC8 plus 0x18 of pending arguments. 335 bytes is far below the page threshold, so NO 0x004A54A0 probe site; r2's `afi` size/stackframe fields are derived and were not used. Push accounting: `add esp,0x20` = 2 + 3 + 3 (0x00424850 pre-zeros, 0x00401900 with its pre-pushed fourth argument, 0x00401530); `add esp,0x24` = 3+3+3; `add esp,0x10` = 1 (0x004F1950) + 2 (0x004F0FA0); `add esp,0x18` = 6 per append. Image bytes: 0x0053B854 reads `81 01` inside raw, i.e. the word 0x0181 and NOT zero, so it is initialised to the 0x181 sentinel; 0x0057BC28/30/34 read zero and are past raw. THE PAIR DIFFERS AGAIN, so "past raw means zero" is not a rule - each was checked.
// Behavior/CFG: one straight-line block, no branch. Reads the parent type through 0x00424850, registers "ModelViewBox" with a 0xC8-byte object through 0x00401900, stores the 16-bit id at 0x0053B854, makes one 0x00401530 link to 0x00422F10 event 1, one to 0x00422F30 with 0x004E7530's return, one to 0x00422DF0 event 2, one 0x00401400 link to 0x00422C90 event 0, two 0x00401400 links to 0x00422FB0 and 0x00422E40 against the phase ids from 0x004F1A60/0x004F1A70, two 0x00401530 links to 0x00422FC0 and 0x00423000 with 0x004F1210/0x004F1220 returns, allocates a count-2 container through 0x004F1950 into 0x0057BC28, and makes two six-argument appends through 0x004F1A80. NOTE: frame size 0xC8 and registered object size 0xC8 COINCIDE - this is the fourth registrar where they do (Flow, MenuLoader, AIGraph, DialogWindow were the others). They are different quantities and one does not imply the other.
// Status/limits: TRANSCRIBED; BUILD_VERIFIED; runtime UNOBSERVED. STRUCTURALLY_VERIFIED for the CFG only, which is one block with no edges - there is no branch structure to disagree about. THE APPEND FRAME OFFSETS ARE NOT VERIFIED: r2 reports `lea` offsets relative to the `esp` at each instruction and arguments are pushed in between, so var_ch/var_30h/var_38h map to frame+0x00/frame+0x18/frame+0x2C by subtracting the bytes pushed at each point. Raw `lea` offsets are recorded beside each argument so the arithmetic can be rechecked. This file OWNS four globals that had no owner: 0x0053B854, 0x0057BC28, 0x0057BC30, 0x0057BC34.
// Blocked: eight handler-only entry points are pushed as arguments and are transcribed by nobody - 0x00422C90, 0x00422DF0, 0x00422E40, 0x00422F10, 0x00422F30, 0x00422FB0, 0x00422FC0, 0x00423000. Declared here as placeholders, NOT stubbed.
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

// 0x0053B854: the ModelViewBox class id. The retail image holds the word 0x0181 here (px 4 -> 81 01
// 00 00), the "not registered" sentinel, NOT zero - the same shape as 0x00539ADC. Initialising it to
// 0 would make the class look pre-registered on entry.
std::uint16_t g_53b854 = 0x0181U; // 0x0053B854
// 0x0057BC28/0x0057BC30/0x0057BC34: container handle and two adjacent words, all zero in the image
// and past the raw extent of .data. Checked individually rather than inferred from the pair above.
std::uint32_t g_57bc28 = 0U; // 0x0057BC28
std::uint32_t g_57bc30 = 0U; // 0x0057BC30
std::uint32_t g_57bc34 = 0U; // 0x0057BC34

} // namespace igi::retail::core

extern "C" {

// Declared in engine/src/retail/core/native_488800.cpp:44; six arguments, args 2-6 are int32_t.
extern void* Script_TaskContainer_Append(std::int32_t* const container, std::int32_t const arg2,
                                         std::int32_t const arg3, std::int32_t const arg4,
                                         std::int32_t const arg5, std::int32_t const arg6);
extern void* Script_TaskContainer_Alloc(std::int32_t count);  // 0x004F1950
extern std::uint16_t NATIVE_DECL Sys_GetGlobal16_424850(void); // 0x00424850
extern std::uint32_t NATIVE_DECL Core_GetEventWord_4E7530();  // 0x004E7530
extern std::uint32_t NATIVE_DECL Sys_GetGlobal32_4F1210(void); // 0x004F1210
extern std::uint32_t NATIVE_DECL Sys_GetGlobal32_4F1220(void); // 0x004F1220
extern std::uint32_t NATIVE_DECL Core_GetRenderWordA_4F0FA0(void); // 0x004F0FA0
extern std::uint8_t Script_PhaseIdGetE4_4F1A60(void);         // 0x004F1A60
extern std::uint8_t Script_PhaseIdGetE5_4F1A70(void);         // 0x004F1A70

// Handler-only entry points, each with a single occurrence in .text.

void NATIVE_DECL ModelViewBox_Register_422B40()
{
    std::uint8_t append_frame[0xC8]; // 0x00422B40: sub esp,0xC8

    const std::uint16_t parent = Sys_GetGlobal16_424850();                    // 0x00422B4A
    const std::uint16_t class_id =
        TaskType_Register("ModelViewBox", 0xC8U, parent, 0U);                 // 0x00422B5A
    igi::retail::core::g_53b854 = class_id;                                   // 0x00422B67
    QTask_LinkQTaskEventHandlerNoContextFn(
        class_id, 1U, reinterpret_cast<igi::retail::core::FrameHandler>(reinterpret_cast<igi::retail::core::FrameHandler>(0x00422F10)));                          // 0x00422B6D
    QTask_LinkQTaskEventHandlerNoContextFn(
        class_id, Core_GetEventWord_4E7530(), reinterpret_cast<igi::retail::core::FrameHandler>(reinterpret_cast<igi::retail::core::FrameHandler>(0x00422F30)));  // 0x00422B87
    QTask_LinkQTaskEventHandlerNoContextFn(
        class_id, 2U, reinterpret_cast<igi::retail::core::FrameHandler>(reinterpret_cast<igi::retail::core::FrameHandler>(0x00422DF0)));                          // 0x00422B9B
    QTask_LinkQTaskEventHandler(class_id, 0U, reinterpret_cast<igi::retail::core::FrameHandler>(reinterpret_cast<igi::retail::core::FrameHandler>(0x00422C90)));  // 0x00422BAF
    QTask_LinkQTaskEventHandler(
        class_id, static_cast<std::uint32_t>(Script_PhaseIdGetE4_4F1A60()),
        reinterpret_cast<igi::retail::core::FrameHandler>(reinterpret_cast<igi::retail::core::FrameHandler>(0x00422FB0)));                                        // 0x00422BC9
    QTask_LinkQTaskEventHandler(
        class_id, static_cast<std::uint32_t>(Script_PhaseIdGetE5_4F1A70()),
        reinterpret_cast<igi::retail::core::FrameHandler>(reinterpret_cast<igi::retail::core::FrameHandler>(0x00422E40)));                                        // 0x00422BE4
    QTask_LinkQTaskEventHandlerNoContextFn(
        class_id, Sys_GetGlobal32_4F1210(), reinterpret_cast<igi::retail::core::FrameHandler>(reinterpret_cast<igi::retail::core::FrameHandler>(0x00422FC0)));    // 0x00422BFF
    QTask_LinkQTaskEventHandlerNoContextFn(
        class_id, Sys_GetGlobal32_4F1220(), reinterpret_cast<igi::retail::core::FrameHandler>(reinterpret_cast<igi::retail::core::FrameHandler>(0x00423000)));    // 0x00422C19
    igi::retail::core::g_57bc28 = static_cast<std::uint32_t>(
        reinterpret_cast<std::uintptr_t>(Script_TaskContainer_Alloc(2)));     // 0x00422C34
    // 0x00422C41/0x00422C46: the 0x57BC30 pointer and the 7 are pre-pushed arguments for the append
    // below, not for 0x004F0FA0, which reads none.
    Script_TaskContainer_Append(
        reinterpret_cast<std::int32_t*>(igi::retail::core::g_57bc28),
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x00)), // 0x00422C57: lea edx,[esp+0xC]
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x18)), // 0x00422C4D: lea ecx,[esp+0x30]
        static_cast<std::int32_t>(Core_GetRenderWordA_4F0FA0()), 7,
        static_cast<std::int32_t>(0x0057BC30U));                              // 0x00422C5E
    Script_TaskContainer_Append(
        reinterpret_cast<std::int32_t*>(igi::retail::core::g_57bc28),
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x00)), // 0x00422C7C: lea edx,[esp+0xC]
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x2C)), // 0x00422C73: lea ecx,[esp+0x38]
        static_cast<std::int32_t>(Core_GetRenderWordA_4F0FA0()), 7,
        static_cast<std::int32_t>(0x0057BC30U));                              // 0x00422C83
}

} // extern "C"
