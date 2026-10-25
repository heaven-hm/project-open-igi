// Native: MenuLoader_Register_4190E0 (placeholder name; retail symbol unknown; the class-name string it registers is "MenuLoader" at 0x00539AE0). retail VA 0x004190E0-0x004191EC (269 bytes, 1 block, 0 edges, 78 instructions).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 `af`/`pdf` over the exact image. Frame `sub esp,0x124`, epilogue `add esp,0x13C` = 0x124 plus 0x18 of pending arguments. 269 bytes is far below the page threshold, so there is NO 0x004A54A0 stack-probe site; r2's `afi` `size`/`stackframe` fields are derived and were not used. Push accounting closes: `add esp,0x2C` at 0x00419125 is 11 dwords = 2 (0x004018F0) + 3 (0x00401900, whose fourth argument is the pre-pushed zero) + 3 (0x00401400) + 3 (0x00401530), and `add esp,0x1C` at 0x00419175 is 7 = 1 (0x004F1950) + 3 + 3. Image bytes: 0x00539ADC is `81 01` inside raw, i.e. the word 0x0181 and NOT zero, so it is initialised to the 0x181 sentinel here; 0x0057BAF4 reads 0 and is past raw, so zero.
// Behavior/CFG: one straight-line block, no branch of any kind. Reads the current task type through 0x004018F0, registers the "MenuLoader" class with a 0x124-byte object through 0x00401900, stores the 16-bit id at 0x00539ADC, links phase 0 handler 0x004191F0 through 0x00401400, links phase 2 of 0x004D3710 (QTask_NoOpHandler) through 0x00401530, then links 0x004192A0 and 0x00419210 against the phase ids returned by 0x004F1A60/0x004F1A70 through 0x00401400, links phase 1 of 0x004D3710 through 0x00401530, allocates a count-3 container through 0x004F1950 storing it at 0x0057BAF4, and makes three six-argument appends through 0x004F1A80.
// Status/limits: TRANSCRIBED; STRUCTURALLY_VERIFIED (static only) - single block, no edges, so the CFG is trivial. Runtime UNOBSERVED. THE FRAME OFFSETS ARE NOT VERIFIED: the three appends pass two frame-relative pointers whose retail `lea` offsets are 0x0C/0x28, 0x0C/0x128 and 0x0C/0x129, but r2 reports those relative to the `esp` current at each instruction, and arguments are pushed between the frame setup and the `lea`s. The comment beside each argument records the raw `lea` offset only. The exact frame-relative derivation must be established before this file is trusted for the append bounds.
// Blocked: three handler-only entry points are pushed as arguments and are transcribed by nobody - 0x004191F0, 0x00419210, 0x004192A0. They are declared here as placeholders, not stubbed.
#include "igi/retail/core/task_types.hpp"
#include "igi/retail/script/script_state.hpp"

#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

namespace igi::retail::core {

// 0x00539ADC: the MenuLoader class id. The retail image holds the word 0x0181 here (px 4 -> 81 01
// 00 00), i.e. the "not registered" sentinel, NOT zero. Initialising it to 0 would make the class
// look pre-registered on entry, a silent behavioural defect no build or link check can catch.
std::uint16_t g_539adc = 0x0181U; // 0x00539ADC

// 0x0057BAF4: the task-container handle. Past the raw extent of .data, so zero-filled, and the
// image reads 0. The two globals differ, so "past raw means zero" is not a rule to apply as a pair.
std::uint32_t g_57baf4 = 0U; // 0x0057BAF4

} // namespace igi::retail::core

extern "C" {

// Declared in engine/src/retail/core/native_488800.cpp:44. Copied verbatim; six arguments, with
// arg5 recorded as a byte at record +0x0C and arg6 as a dword at record +0x08.
extern void* Script_TaskContainer_Append(std::int32_t* const container, std::int32_t const arg2,
                                         std::int32_t const arg3, std::int32_t const arg4,
                                         std::int32_t const arg5, std::int32_t const arg6);
extern void* Script_TaskContainer_Alloc(std::int32_t count); // 0x004F1950
// 0x004D3710 is already implemented at engine/src/retail/script/native_4d3710.cpp as
// QTask_NoOpHandler (1 byte, reads no argument). Referenced, never redefined (rule 13).
void NATIVE_DECL QTask_NoOpHandler();
extern std::uint32_t Sys_GetGlobal32_4F0EF0(void);           // 0x004F0EF0
extern std::uint32_t Sys_GetGlobal32_4F0FF0(void);           // 0x004F0FF0
extern std::uint8_t Script_PhaseIdGetE4_4F1A60(void);        // 0x004F1A60
extern std::uint8_t Script_PhaseIdGetE5_4F1A70(void);        // 0x004F1A70

// Handler-only entry points: each has exactly one occurrence in .text, pushed as an argument here.

void NATIVE_DECL MenuLoader_Register_4190E0()
{
    std::uint8_t append_frame[0x124]; // 0x004190E0: sub esp,0x124

    // 0x004190E6: two zero pushes. 0x004018F0 reads no arguments, so the second zero is consumed
    // by 0x00401900 below as its fourth argument, exactly as at 0x00401FC0.
    const std::uint16_t parent = QTask_GetCurrentType();                     // 0x004190EA
    const std::uint16_t class_id =
        TaskType_Register("MenuLoader", 0x124U, parent, 0U);                 // 0x004190FA
    igi::retail::core::g_539adc = class_id;                                  // 0x00419107
    QTask_LinkQTaskEventHandler(class_id, 0U, reinterpret_cast<igi::retail::core::FrameHandler>(reinterpret_cast<igi::retail::core::FrameHandler>(0x004191F0)));   // 0x0041910D
    QTask_LinkQTaskEventHandlerNoContextFn(
        class_id, 2U, reinterpret_cast<igi::retail::core::FrameHandler>(&QTask_NoOpHandler));   // 0x00419120
    QTask_LinkQTaskEventHandler(
        class_id, static_cast<std::uint32_t>(Script_PhaseIdGetE4_4F1A60()),
        reinterpret_cast<igi::retail::core::FrameHandler>(reinterpret_cast<igi::retail::core::FrameHandler>(0x004192A0)));                                         // 0x0041913B
    QTask_LinkQTaskEventHandler(
        class_id, static_cast<std::uint32_t>(Script_PhaseIdGetE5_4F1A70()),
        reinterpret_cast<igi::retail::core::FrameHandler>(reinterpret_cast<igi::retail::core::FrameHandler>(0x00419210)));                                         // 0x00419156
    QTask_LinkQTaskEventHandlerNoContextFn(
        class_id, 1U, reinterpret_cast<igi::retail::core::FrameHandler>(&QTask_NoOpHandler));   // 0x00419169
    igi::retail::core::g_57baf4 = static_cast<std::uint32_t>(
        reinterpret_cast<std::uintptr_t>(Script_TaskContainer_Alloc(3)));    // 0x00419170
    // 0x0041917D/0x0041917F: the 7 and 0 are pre-pushed arguments for the append below, not for
    // 0x004F0FF0, which reads none.
    Script_TaskContainer_Append(
        reinterpret_cast<std::int32_t*>(igi::retail::core::g_57baf4),
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x00)),                // 0x00419190: lea edx,[esp+0xC]
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x1C)),                // 0x00419186: lea ecx,[esp+0x28]
        static_cast<std::int32_t>(Sys_GetGlobal32_4F0FF0()), 7, 0);          // 0x00419197
    Script_TaskContainer_Append(
        reinterpret_cast<std::int32_t*>(igi::retail::core::g_57baf4),
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x00)),                // 0x004191B5: lea edx,[esp+0xC]
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x11C)),               // 0x004191A8: lea ecx,[esp+0x128]
        static_cast<std::int32_t>(Sys_GetGlobal32_4F0EF0()), 6, 0);          // 0x004191BC
    Script_TaskContainer_Append(
        reinterpret_cast<std::int32_t*>(igi::retail::core::g_57baf4),
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x00)),                // 0x004191DA: lea edx,[esp+0xC]
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x11D)),               // 0x004191CD: lea ecx,[esp+0x129]
        static_cast<std::int32_t>(Sys_GetGlobal32_4F0EF0()), 6, 0);          // 0x004191E1
}

} // extern "C"
