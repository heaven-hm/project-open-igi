// Native: MenuButton_Register_421430 (placeholder name; retail symbol unknown; the class-name string it registers is "MenuButton" at 0x0053B7F4). retail VA 0x00421430-0x0042150E (223 bytes, 1 block, 0 edges, 61 instructions).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 `pd` over the exact image plus capstone 5.0.7 aligned from the verified entry 0x00421430; the two listings agree instruction-for-instruction. Extent: entry `sub esp,0x98`, `c3` at 0x0042150E, and the byte after the ret is 0x90 (0x0042150F is alignment padding, NOT part of the routine), so the extent is 223 bytes and not 224. Frame `sub esp,0x98`, epilogue `add esp,0xB0` = 0x98 plus 0x18 of pending arguments. Push accounting, replayed with esp simulated and reconciling with zero slack: `add esp,0x20` at 0x00421462 = 2 (0x00424850 pre-zeros) + 3 (0x00401900 with its pre-pushed fourth argument) + 3 (0x00401530); `add esp,0x24` at 0x004214A4 = 3+3+3 (two 0x00401530 and one 0x00401400); `add esp,0xC` at 0x004214BE = 3 (0x00401400); `add esp,0x10` at 0x004214E0 = 3 (0x00401400) + 1 (0x004F1950); `add esp,0xB0` at 0x00421508 = 0x98 frame + 6 pending pushes for the single 0x004F1A80. Call census from the 0xE8 enumeration, not from handler immediates: 0x00424850 x1, 0x00401900 x1, 0x00401530 x3, 0x00401400 x3, 0x004E7530 x1, 0x004F1A60 x1, 0x004F1A70 x1, 0x004F1950 x1, 0x004F0FE0 x1, 0x004F1A80 x1; 14 call instructions in all, no branch of any kind.
// Behavior/CFG: one straight-line block, no branch, no loop. Reads the parent type through 0x00424850, registers "MenuButton" with a 0x98-byte object through 0x00401900 and stores the 16-bit id at 0x0053B7F0 (image word 0x0181, the not-registered sentinel). Then makes six handler links on that id: 0x00401530 to 0x004215D0 on event 1; 0x00401530 to 0x00421600 on the event word from 0x004E7530; 0x00401530 to 0x004D3710 on event 2; 0x00401400 to 0x00421510 on event 0; 0x00401400 to 0x004216F0 on the phase id from 0x004F1A60; 0x00401400 to 0x00421550 on the phase id from 0x004F1A70. Then allocates a count-1 container through 0x004F1950 into 0x0057BBFC and makes one six-argument append through 0x004F1A80, whose record is {frame+0x54 - frame+0x00, 0x004F0FE0's value, arg6 = 0, byte(arg5 = 7)}. NOTE: the frame size 0x98 and the registered object size 0x98 COINCIDE. They are different quantities and one does not imply the other.
// Status/limits: TRANSCRIBED; LINKABLE (the handler entry points this routine registers are raw code immediates in retail and are transcribed by nobody, so they are passed as cast integers and create no link dependency; they stay recorded gaps in the tree, not stubs). STRUCTURALLY_VERIFIED for the CFG only, which is one block with no edges, so there is no branch structure to disagree about. Runtime UNOBSERVED in both binaries. THE APPEND FRAME OFFSETS ARE DERIVED, NOT MEASURED BY A DEBUGGER: 0x004214F8 `lea edx,[esp+0x60]` and 0x004214FC `lea eax,[esp+0xC]` are relative to the esp at each instruction, and 12 bytes (the two surplus pushes and the 0x004F0FE0 result push) had been pushed by then, so they map to frame+0x54 and frame+0x00. Raw `lea` offsets are recorded beside each argument so the arithmetic can be rechecked. This file OWNS two globals that had no owner: 0x0053B7F0 (16-bit class id, written here) and 0x0057BBFC (32-bit container handle, written here at 0x004214E3; it is past the raw extent of .data, so its image value is zero - checked against the section table, not inferred).
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

// 0x0053B7F0: the MenuButton class id. The retail image holds the word 0x0181 here (px 4 -> 81 01 00 00),
// the "not registered" sentinel, NOT zero - the same shape as 0x0053B854 and 0x0053B90C.
std::uint16_t g_53b7f0 = 0x0181U; // 0x0053B7F0
// 0x0057BBFC: the class parameter container allocated at 0x004214E3. The address is past the raw extent of
// .data (.data raw ends at 0x00550000, this is 0x0057BBFC), so the image value is zero.
std::uint32_t g_57bbfc = 0U; // 0x0057BBFC

} // namespace igi::retail::core

extern "C" {

extern std::uint16_t NATIVE_DECL Sys_GetGlobal16_424850(void); // 0x00424850: argument-less 16-bit getter (defined in engine/src/retail/sys/native_424850.cpp)
extern std::uint32_t NATIVE_DECL Core_GetEventWord_4E7530();   // 0x004E7530: the current event word
extern std::uint32_t NATIVE_DECL Sys_GetGlobal32_4F0FE0(void); // 0x004F0FE0: `mov eax,[0xA71888]; ret`
extern std::uint8_t Script_PhaseIdGetE4_4F1A60(void);          // 0x004F1A60: argument-less byte getter
extern std::uint8_t Script_PhaseIdGetE5_4F1A70(void);          // 0x004F1A70: argument-less byte getter
extern void* Script_TaskContainer_Alloc(std::int32_t count);   // 0x004F1950
// Declared in engine/src/retail/script/native_4f1a80.cpp: six arguments, args 2-6 are int32_t.
extern void* Script_TaskContainer_Append(std::int32_t* const container, std::int32_t const arg2,
                                         std::int32_t const arg3, std::int32_t const arg4,
                                         std::int32_t const arg5, std::int32_t const arg6);
extern void NATIVE_DECL QTask_NoOpHandler(void); // 0x004D3710: single `ret`, defined in engine/src/retail/script/native_4d3710.cpp


void NATIVE_DECL MenuButton_Register_421430()
{
    NATIVE_LOG_ENTER("MenuButton_Register_421430", "0x00421430");
    std::uint8_t append_frame[0x98]; // 0x00421430: sub esp,0x98

    // 0x00421436/0x00421438: the two zeros are surplus for the argument-less getter and are consumed as
    // 0x00401900's fourth argument (task list = 0) - they are not arguments to 0x00424850, which reads none.
    const std::uint16_t parent = Sys_GetGlobal16_424850();                    // 0x0042143A
    const std::uint16_t class_id =
        TaskType_Register("MenuButton", 0x98U, parent, 0U);                   // 0x0042144A
    igi::retail::core::g_53b7f0 = class_id;                                   // 0x00421457
    QTask_LinkQTaskEventHandlerNoContextFn(
        class_id, 1U, reinterpret_cast<igi::retail::core::FrameHandler>(0x004215D0));                            // 0x0042145D
    // 0x00421465: the handler immediate is pre-pushed and is the surplus argument of 0x004E7530, which
    // reads none; 0x004E7530's return is the event word.
    QTask_LinkQTaskEventHandlerNoContextFn(
        class_id, Core_GetEventWord_4E7530(), reinterpret_cast<igi::retail::core::FrameHandler>(0x00421600));    // 0x00421477
    QTask_LinkQTaskEventHandlerNoContextFn(
        class_id, 2U, reinterpret_cast<igi::retail::core::FrameHandler>(&QTask_NoOpHandler)); // 0x0042148B
    QTask_LinkQTaskEventHandler(
        class_id, 0U, reinterpret_cast<igi::retail::core::FrameHandler>(0x00421510));                            // 0x0042149F
    QTask_LinkQTaskEventHandler(
        class_id, static_cast<std::uint32_t>(Script_PhaseIdGetE4_4F1A60()),
        reinterpret_cast<igi::retail::core::FrameHandler>(0x004216F0));                                          // 0x004214B9
    QTask_LinkQTaskEventHandler(
        class_id, static_cast<std::uint32_t>(Script_PhaseIdGetE5_4F1A70()),
        reinterpret_cast<igi::retail::core::FrameHandler>(0x00421550));                                          // 0x004214D4
    igi::retail::core::g_57bbfc = static_cast<std::uint32_t>(
        reinterpret_cast<std::uintptr_t>(Script_TaskContainer_Alloc(1)));     // 0x004214E3
    // 0x004214E8/0x004214EA: the 0 and the 7 are pre-pushed arguments for the append below, not for
    // 0x004F0FE0, which reads none. They arrive as arg6 (0) and arg5 (7).
    Script_TaskContainer_Append(
        reinterpret_cast<std::int32_t*>(igi::retail::core::g_57bbfc),
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x00)), // 0x004214FC: lea eax,[esp+0xC]
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x54)), // 0x004214F8: lea edx,[esp+0x60]
        static_cast<std::int32_t>(Sys_GetGlobal32_4F0FE0()), 7, 0);                       // 0x00421503
}

} // extern "C"
