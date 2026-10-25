// Native: MenuText_Register_420CC0 (placeholder name; retail symbol unknown; the class-name string it registers is "MenuText" at 0x0053B7E4). retail VA 0x00420CC0-0x00420E81 (450 bytes, 1 block, 0 edges, 128 instructions).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 `pd` over the exact image plus capstone 5.0.7 aligned from the verified entry 0x00420CC0; the two listings agree. Extent: entry `sub esp,0x170`, `c3` at 0x00420E81, and the byte after the ret is 0x90 (0x00420E82 onward is alignment padding, NOT part of the routine), so the extent is 450 bytes and not 451+. Frame `sub esp,0x170`, epilogue `add esp,0x1A8` = 0x170 plus 0x38 of pending arguments. Push accounting, replayed with esp simulated and reconciling with zero slack: `add esp,0x20` at 0x00420CF2 = 2 (0x00424850 pre-zeros) + 3 (0x00401900 with its pre-pushed fourth argument) + 3 (0x00401530); `add esp,0x24` at 0x00420D34 = 3+3+3 (two 0x00401530 and one 0x00401400); `add esp,0xC` at 0x00420D4E = 3 (0x00401400); `add esp,0xC` at 0x00420D69 = 3 (0x00401400); `add esp,0x10` at 0x00420D8B = 3 (0x00401530) + 1 (0x004F1950); `add esp,0x18` at each of 0x00420DB3, 0x00420DD9, 0x00420DFF and 0x00420E25 = the 6 pending dwords of one 0x004F1A80; `add esp,0x1A8` at 0x00420E7B = 0x170 frame + 0x38 = 6 (the fifth append, deferred) + 8 (four 0x004B8810 two-argument calls). Call census from the 0xE8 enumeration, not from handler immediates: 0x00424850 x1, 0x00401900 x1, 0x00401530 x4, 0x00401400 x3, 0x004E7530 x1, 0x00424890 x1, 0x004F1A60 x1, 0x004F1A70 x1, 0x004F1950 x1, 0x004F0FE0 x3, 0x004F0FA0 x1, 0x004F0FC0 x1, 0x004F1A80 x5, 0x004B8810 x4; 29 call instructions in all, no branch of any kind.
// Behavior/CFG: one straight-line block, no branch, no loop. Reads the parent type through 0x00424850, registers "MenuText" with a 0x170-byte object through 0x00401900 and stores the 16-bit id at 0x0053B770 (image word 0x0181, the not-registered sentinel). Then makes seven handler links on that id: 0x00401530 to 0x00421070 on event 1; 0x00401530 to 0x00421130 on the event word from 0x004E7530; 0x00401530 to 0x00420F00 on event 2; 0x00401400 to 0x00420E90 on event 0; 0x00401400 to 0x004213D0 on the phase id from 0x004F1A60; 0x00401400 to 0x00420F40 on the phase id from 0x004F1A70; 0x00401530 to 0x00420BC0 on the word from 0x00424890. Then allocates a count-5 container through 0x004F1950 into 0x0057BBF4 and makes five six-argument appends through 0x004F1A80, all with arg2 = frame+0x00, arg5 = 7, arg6 = 0, and arg3 = frame+0x54 / frame+0x94 / frame+0x118 / frame+0x11C / frame+0xD4 for the getters 0x004F0FE0, 0x004F0FE0, 0x004F0FA0, 0x004F0FC0 and 0x004F0FE0. Finally registers four script names against the literal ids 0, 1, 2 and 3 through 0x004B8810: "TEXT_BLACK" @0x0053B7D8, "TEXT_WHITE" @0x0053B7CC, "TEXT_GREEN" @0x0053B7C0 and "TEXT_ORANGE" @0x0053B7B4. NOTE: the frame size 0x170 and the registered object size 0x170 COINCIDE. They are different quantities and one does not imply the other.
// Status/limits: TRANSCRIBED; LINKABLE (the handler entry points this routine registers are raw code immediates in retail and are transcribed by nobody, so they are passed as cast integers and create no link dependency; they stay recorded gaps in the tree, not stubs). STRUCTURALLY_VERIFIED for the CFG only, which is one block with no edges. Runtime UNOBSERVED in both binaries. THE APPEND FRAME OFFSETS ARE DERIVED, NOT MEASURED BY A DEBUGGER: each `lea` is relative to the esp at that instruction and 12 bytes (two surplus pushes and the getter result push) had been pushed by then, so the raw offsets 0x60/0xA0/0x124/0x128/0xE0 minus 0x0C map to frame+0x54/0x94/0x118/0x11C/0xD4, and 0xC maps to frame+0x00. Raw `lea` offsets are recorded beside each argument so the arithmetic can be rechecked.
// Linkage note (per the Lead's trap): the two globals this file OWNS are emitted with C++ linkage because they sit in `namespace igi::retail::core` - the dominant tree convention (431 plain definitions against 44 extern "C"). A future file that declares 0x0053B770 or 0x0057BBF4 as `extern "C"` would therefore get a SECOND, distinct variable with NO link error, and the sentinel would be written by one file and read by another. Declare them with the same namespace, or the ownership must move here.
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

// 0x0053B770: the MenuText class id. The retail image holds the word 0x0181 here (px 4 -> 81 01 00 00),
// the "not registered" sentinel, NOT zero.
std::uint16_t g_53b770 = 0x0181U; // 0x0053B770
// 0x0057BBF4: the class parameter container allocated at 0x00420D8E. The address is past the raw extent of
// .data (.data raw ends at 0x00550000, this is 0x0057BBF4), so the image value is zero.
std::uint32_t g_57bbf4 = 0U; // 0x0057BBF4

} // namespace igi::retail::core

extern "C" {

extern std::uint16_t NATIVE_DECL Sys_GetGlobal16_424850(void); // 0x00424850: argument-less 16-bit getter (defined in engine/src/retail/sys/native_424850.cpp)
extern std::uint32_t NATIVE_DECL Sys_GetGlobal32_424890(void); // 0x00424890: argument-less 32-bit getter (defined in engine/src/retail/sys/native_424890.cpp)
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
// Declared in engine/src/retail/core/native_402ff0.cpp:238, defined in engine/src/retail/script/native_4b8810.cpp.
extern void Script_QHashNodeRegister(void* arg1, void* value); // 0x004B8810


void NATIVE_DECL MenuText_Register_420CC0()
{
    std::uint8_t append_frame[0x170]; // 0x00420CC0: sub esp,0x170

    // 0x00420CC6/0x00420CC8: the two zeros are surplus for the argument-less getter and are consumed as
    // 0x00401900's fourth argument (task list = 0) - they are not arguments to 0x00424850, which reads none.
    const std::uint16_t parent = Sys_GetGlobal16_424850();                    // 0x00420CCA
    const std::uint16_t class_id =
        TaskType_Register("MenuText", 0x170U, parent, 0U);                    // 0x00420CDA
    igi::retail::core::g_53b770 = class_id;                                   // 0x00420CE7
    QTask_LinkQTaskEventHandlerNoContextFn(
        class_id, 1U, reinterpret_cast<igi::retail::core::FrameHandler>(0x00421070));                              // 0x00420CED
    // 0x00420CF5: the handler immediate is pre-pushed and is the surplus argument of 0x004E7530, which
    // reads none; 0x004E7530's return is the event word.
    QTask_LinkQTaskEventHandlerNoContextFn(
        class_id, Core_GetEventWord_4E7530(), reinterpret_cast<igi::retail::core::FrameHandler>(0x00421130));      // 0x00420D07
    QTask_LinkQTaskEventHandlerNoContextFn(
        class_id, 2U, reinterpret_cast<igi::retail::core::FrameHandler>(0x00420F00));                              // 0x00420D1B
    QTask_LinkQTaskEventHandler(
        class_id, 0U, reinterpret_cast<igi::retail::core::FrameHandler>(0x00420E90));                              // 0x00420D2F
    QTask_LinkQTaskEventHandler(
        class_id, static_cast<std::uint32_t>(Script_PhaseIdGetE4_4F1A60()),
        reinterpret_cast<igi::retail::core::FrameHandler>(0x004213D0));                                            // 0x00420D49
    QTask_LinkQTaskEventHandler(
        class_id, static_cast<std::uint32_t>(Script_PhaseIdGetE5_4F1A70()),
        reinterpret_cast<igi::retail::core::FrameHandler>(0x00420F40));                                            // 0x00420D64
    // 0x00420D6C: the handler immediate is pre-pushed and is the surplus argument of 0x00424890, which
    // reads none; 0x00424890's return is the event id.
    QTask_LinkQTaskEventHandlerNoContextFn(
        class_id, Sys_GetGlobal32_424890(), reinterpret_cast<igi::retail::core::FrameHandler>(0x00420BC0));        // 0x00420D7F
    igi::retail::core::g_57bbf4 = static_cast<std::uint32_t>(
        reinterpret_cast<std::uintptr_t>(Script_TaskContainer_Alloc(5)));     // 0x00420D8E
    // Each append: the 0 and the 7 are pre-pushed arguments for the append, not for the getter in front of
    // them, which reads none. They arrive as arg6 (0) and arg5 (7).
    Script_TaskContainer_Append(
        reinterpret_cast<std::int32_t*>(igi::retail::core::g_57bbf4),
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x00)), // 0x00420DA7: lea ecx,[esp+0xC]
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x54)), // 0x00420DA3: lea eax,[esp+0x60]
        static_cast<std::int32_t>(Sys_GetGlobal32_4F0FE0()), 7, 0);                       // 0x00420DAE
    Script_TaskContainer_Append(
        reinterpret_cast<std::int32_t*>(igi::retail::core::g_57bbf4),
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x00)), // 0x00420DCD: lea ecx,[esp+0xC]
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x94)), // 0x00420DC6: lea eax,[esp+0xA0]
        static_cast<std::int32_t>(Sys_GetGlobal32_4F0FE0()), 7, 0);                       // 0x00420DD4
    Script_TaskContainer_Append(
        reinterpret_cast<std::int32_t*>(igi::retail::core::g_57bbf4),
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x00)), // 0x00420DF3: lea ecx,[esp+0xC]
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x118)), // 0x00420DEC: lea eax,[esp+0x124]
        static_cast<std::int32_t>(Core_GetRenderWordA_4F0FA0()), 7, 0);                   // 0x00420DFA
    Script_TaskContainer_Append(
        reinterpret_cast<std::int32_t*>(igi::retail::core::g_57bbf4),
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x00)), // 0x00420E19: lea ecx,[esp+0xC]
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x11C)), // 0x00420E12: lea eax,[esp+0x128]
        static_cast<std::int32_t>(Sys_GetGlobal32_4F0FC0()), 7, 0);                       // 0x00420E20
    Script_TaskContainer_Append(
        reinterpret_cast<std::int32_t*>(igi::retail::core::g_57bbf4),
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x00)), // 0x00420E3F: lea ecx,[esp+0xC]
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0xD4)), // 0x00420E38: lea eax,[esp+0xE0]
        static_cast<std::int32_t>(Sys_GetGlobal32_4F0FE0()), 7, 0);                       // 0x00420E46

    // 0x00420E4B/0x00420E57/0x00420E63/0x00420E6F: the string pointer is pushed last, so it is argument 1.
    Script_QHashNodeRegister(const_cast<char*>("TEXT_BLACK"),
                             reinterpret_cast<void*>(static_cast<std::uintptr_t>(0U)));   // 0x00420E52
    Script_QHashNodeRegister(const_cast<char*>("TEXT_WHITE"),
                             reinterpret_cast<void*>(static_cast<std::uintptr_t>(1U)));   // 0x00420E5E
    Script_QHashNodeRegister(const_cast<char*>("TEXT_GREEN"),
                             reinterpret_cast<void*>(static_cast<std::uintptr_t>(2U)));   // 0x00420E6A
    Script_QHashNodeRegister(const_cast<char*>("TEXT_ORANGE"),
                             reinterpret_cast<void*>(static_cast<std::uintptr_t>(3U)));   // 0x00420E76
}

} // extern "C"
