// Native: MenuScreen_Register_421750 (placeholder name; retail symbol unknown; the class-name string it registers is "MenuScreen" at 0x0053B804). retail VA 0x00421750-0x004219A3 (596 bytes, 1 block, 0 edges, 173 instructions).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 `pd` over the exact image plus capstone 5.0.7 aligned from the verified entry 0x00421750; the two listings agree. Extent: entry `sub esp,0x1A4`, `c3` at 0x004219A3, and the byte after the ret is 0x90 (0x004219A4 onward is alignment padding, NOT part of the routine), so the extent is 596 bytes. Frame `sub esp,0x1A4`, epilogue `add esp,0x1BC` = 0x1A4 plus 0x18 of pending arguments. Push accounting, replayed with esp simulated and reconciling with zero slack: `add esp,0x20` at 0x00421782 = 2 (0x00424850 pre-zeros) + 3 (0x00401900 with its pre-pushed fourth argument) + 3 (0x00401530); `add esp,0x24` at 0x004217C4 = 3+3+3 (the 0x004E7530-driven link, the 0x00401400 event-0 link and the event-2 link); `add esp,0xC` at each of 0x004217DE, 0x004217F9, 0x00421814, 0x0042182E, 0x00421849 = one three-argument link each; `add esp,0x10` at 0x0042186B = 3 (the last link) + 1 (0x004F1950); `add esp,0x18` at each of the first seven appends = the 6 pending dwords of one 0x004F1A80; `add esp,0x1BC` at 0x0042199D = 0x1A4 frame + 0x18 (the eighth append, deferred). Call census from the 0xE8 enumeration, not from handler immediates: 0x00424850 x1, 0x00401900 x1, 0x00401530 x7, 0x00401400 x3, 0x004E7530 x1, 0x004F1A60 x1, 0x004F1A70 x1, 0x004F1210 x1, 0x004F1220 x1, 0x00424870 x1, 0x00424880 x1, 0x004F1950 x1, 0x004F0FE0 x1, 0x004F0FA0 x4, 0x004F0FC0 x3, 0x004F1A80 x8; 36 call instructions in all, no branch of any kind.
// Behavior/CFG: one straight-line block, no branch, no loop. Reads the parent type through 0x00424850, registers "MenuScreen" with a 0x1A4-byte object through 0x00401900 and stores the 16-bit id at 0x0053B800 (image word 0x0181, the not-registered sentinel). Then makes ten handler links on that id, in retail order: 0x00401530 to 0x00422110 on event 1; 0x00401530 to 0x00421C20 on the event word from 0x004E7530; 0x00401400 to 0x004219B0 on event 0; 0x00401530 to 0x00421A10 on event 2; 0x00401400 to 0x00422180 on the phase id from 0x004F1A60; 0x00401400 to 0x00421A70 on the phase id from 0x004F1A70; 0x00401530 to 0x00422190 on the word from 0x004F1210; 0x00401530 to 0x00422200 on the word from 0x004F1220; 0x00401530 to 0x00422260 on the word from 0x00424870; 0x00401530 to 0x00422300 on the word from 0x00424880. Then allocates a count-8 container through 0x004F1950 into 0x0057BC04 and makes eight six-argument appends through 0x004F1A80, all with arg2 = frame+0x00, arg5 = 7, arg6 = 0, and arg3 = frame+0x54, 0x9C, 0xA4, 0xA0, 0xA8, 0xAC, 0xFC, 0x14C for the getters 0x004F0FE0, 0x004F0FA0, 0x004F0FA0, 0x004F0FA0, 0x004F0FA0, 0x004F0FC0, 0x004F0FC0, 0x004F0FC0. NOTE: the arg3 offsets 0x9C/0xA4/0xA0/0xA8 are NOT in ascending order in call order - that is the retail order and must not be sorted. NOTE: the frame size 0x1A4 and the registered object size 0x1A4 COINCIDE. They are different quantities and one does not imply the other.
// Status/limits: TRANSCRIBED; NOT LINKABLE (the ten handler-only entry points below are transcribed by nobody, so the linker cannot bind them - this is the recorded gap, not a stub). STRUCTURALLY_VERIFIED for the CFG only, which is one block with no edges. Runtime UNOBSERVED in both binaries. THE APPEND FRAME OFFSETS ARE DERIVED, NOT MEASURED BY A DEBUGGER: each `lea` is relative to the esp at that instruction and 12 bytes (two surplus pushes and the getter result push) had been pushed by then, so each raw offset minus 0x0C is the frame offset, and the raw 0xC is frame+0x00. Raw `lea` offsets are recorded beside each argument so the arithmetic can be rechecked.
// Linkage note (per the Lead's trap): the two globals this file OWNS are emitted with C++ linkage because they sit in `namespace igi::retail::core`. A future file that declares 0x0053B800 or 0x0057BC04 as `extern "C"` would get a SECOND, distinct variable with NO link error. Declare them with the same namespace, or the ownership must move here.
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

// 0x0053B800: the MenuScreen class id. The retail image holds the word 0x0181 here (px 4 -> 81 01 00 00),
// the "not registered" sentinel, NOT zero.
std::uint16_t g_53b800 = 0x0181U; // 0x0053B800
// 0x0057BC04: the class parameter container allocated at 0x0042186E. The address is past the raw extent of
// .data (.data raw ends at 0x00550000, this is 0x0057BC04), so the image value is zero.
std::uint32_t g_57bc04 = 0U; // 0x0057BC04

} // namespace igi::retail::core

extern "C" {

extern std::uint16_t NATIVE_DECL Sys_GetGlobal16_424850(void); // 0x00424850: argument-less 16-bit getter (defined in engine/src/retail/sys/native_424850.cpp)
extern std::uint32_t NATIVE_DECL Sys_GetGlobal32_424870(void); // 0x00424870: argument-less 32-bit getter (defined in engine/src/retail/sys/native_424870.cpp)
extern std::uint32_t NATIVE_DECL Sys_GetGlobal32_424880(void); // 0x00424880: argument-less 32-bit getter (defined in engine/src/retail/sys/native_424880.cpp)
extern std::uint32_t NATIVE_DECL Core_GetEventWord_4E7530();   // 0x004E7530: the current event word
extern std::uint32_t NATIVE_DECL Sys_GetGlobal32_4F1210(void); // 0x004F1210
extern std::uint32_t NATIVE_DECL Sys_GetGlobal32_4F1220(void); // 0x004F1220
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

// Handler-only entry points of this class, each with a single occurrence in .text (the 0x00401530/0x00401400
// immediates below). They are pushed as arguments and are transcribed by nobody: declared here as
// placeholders, NOT stubbed, so the link gap stays visible.
void NATIVE_DECL MenuScreen_Handler_4219B0(void* task); // 0x004219B0
void NATIVE_DECL MenuScreen_Handler_421A10(void* task); // 0x00421A10
void NATIVE_DECL MenuScreen_Handler_421A70(void* task); // 0x00421A70
void NATIVE_DECL MenuScreen_Handler_421C20(void* task); // 0x00421C20
void NATIVE_DECL MenuScreen_Handler_422110(void* task); // 0x00422110
void NATIVE_DECL MenuScreen_Handler_422180(void* task); // 0x00422180
void NATIVE_DECL MenuScreen_Handler_422190(void* task); // 0x00422190
void NATIVE_DECL MenuScreen_Handler_422200(void* task); // 0x00422200
void NATIVE_DECL MenuScreen_Handler_422260(void* task); // 0x00422260
void NATIVE_DECL MenuScreen_Handler_422300(void* task); // 0x00422300

void NATIVE_DECL MenuScreen_Register_421750()
{
    std::uint8_t append_frame[0x1A4]; // 0x00421750: sub esp,0x1A4

    // 0x00421756/0x00421758: the two zeros are surplus for the argument-less getter and are consumed as
    // 0x00401900's fourth argument (task list = 0) - they are not arguments to 0x00424850, which reads none.
    const std::uint16_t parent = Sys_GetGlobal16_424850();                    // 0x0042175A
    const std::uint16_t class_id =
        TaskType_Register("MenuScreen", 0x1A4U, parent, 0U);                  // 0x0042176A
    igi::retail::core::g_53b800 = class_id;                                   // 0x00421777
    QTask_LinkQTaskEventHandlerNoContextFn(
        class_id, 1U, &MenuScreen_Handler_422110);                            // 0x0042177D
    // 0x00421785: the handler immediate is pre-pushed and is the surplus argument of 0x004E7530, which
    // reads none; 0x004E7530's return is the event word.
    QTask_LinkQTaskEventHandlerNoContextFn(
        class_id, Core_GetEventWord_4E7530(), &MenuScreen_Handler_421C20);    // 0x00421797
    QTask_LinkQTaskEventHandler(
        class_id, 0U, &MenuScreen_Handler_4219B0);                            // 0x004217AB
    QTask_LinkQTaskEventHandlerNoContextFn(
        class_id, 2U, &MenuScreen_Handler_421A10);                            // 0x004217BF
    QTask_LinkQTaskEventHandler(
        class_id, static_cast<std::uint32_t>(Script_PhaseIdGetE4_4F1A60()),
        &MenuScreen_Handler_422180);                                          // 0x004217D9
    QTask_LinkQTaskEventHandler(
        class_id, static_cast<std::uint32_t>(Script_PhaseIdGetE5_4F1A70()),
        &MenuScreen_Handler_421A70);                                          // 0x004217F4
    // Each of the next four links pre-pushes its handler immediate, which is the surplus argument of the
    // argument-less getter in front of it.
    QTask_LinkQTaskEventHandlerNoContextFn(
        class_id, Sys_GetGlobal32_4F1210(), &MenuScreen_Handler_422190);      // 0x0042180F
    QTask_LinkQTaskEventHandlerNoContextFn(
        class_id, Sys_GetGlobal32_4F1220(), &MenuScreen_Handler_422200);      // 0x00421829
    QTask_LinkQTaskEventHandlerNoContextFn(
        class_id, Sys_GetGlobal32_424870(), &MenuScreen_Handler_422260);      // 0x00421844
    QTask_LinkQTaskEventHandlerNoContextFn(
        class_id, Sys_GetGlobal32_424880(), &MenuScreen_Handler_422300);      // 0x0042185F
    igi::retail::core::g_57bc04 = static_cast<std::uint32_t>(
        reinterpret_cast<std::uintptr_t>(Script_TaskContainer_Alloc(8)));     // 0x0042186E
    // Each append: the 0 and the 7 are pre-pushed arguments for the append, not for the getter in front of
    // them, which reads none. They arrive as arg6 (0) and arg5 (7). The arg3 offsets are in retail order.
    Script_TaskContainer_Append(
        reinterpret_cast<std::int32_t*>(igi::retail::core::g_57bc04),
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x00)), // 0x00421887: lea ecx,[esp+0xC]
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x54)), // 0x0042187D: lea eax,[esp+0x60]
        static_cast<std::int32_t>(Sys_GetGlobal32_4F0FE0()), 7, 0);                       // 0x0042188E
    Script_TaskContainer_Append(
        reinterpret_cast<std::int32_t*>(igi::retail::core::g_57bc04),
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x00)), // 0x004218AD: lea ecx,[esp+0xC]
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x9C)), // 0x004218A6: lea eax,[esp+0xA8]
        static_cast<std::int32_t>(Core_GetRenderWordA_4F0FA0()), 7, 0);                   // 0x004218B4
    Script_TaskContainer_Append(
        reinterpret_cast<std::int32_t*>(igi::retail::core::g_57bc04),
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x00)), // 0x004218D3: lea ecx,[esp+0xC]
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0xA4)), // 0x004218CC: lea eax,[esp+0xB0]
        static_cast<std::int32_t>(Core_GetRenderWordA_4F0FA0()), 7, 0);                   // 0x004218DA
    Script_TaskContainer_Append(
        reinterpret_cast<std::int32_t*>(igi::retail::core::g_57bc04),
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x00)), // 0x004218F9: lea ecx,[esp+0xC]
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0xA0)), // 0x004218F2: lea eax,[esp+0xAC]
        static_cast<std::int32_t>(Core_GetRenderWordA_4F0FA0()), 7, 0);                   // 0x00421900
    Script_TaskContainer_Append(
        reinterpret_cast<std::int32_t*>(igi::retail::core::g_57bc04),
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x00)), // 0x0042191F: lea ecx,[esp+0xC]
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0xA8)), // 0x00421918: lea eax,[esp+0xB4]
        static_cast<std::int32_t>(Core_GetRenderWordA_4F0FA0()), 7, 0);                   // 0x00421926
    Script_TaskContainer_Append(
        reinterpret_cast<std::int32_t*>(igi::retail::core::g_57bc04),
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x00)), // 0x00421945: lea ecx,[esp+0xC]
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0xAC)), // 0x0042193E: lea eax,[esp+0xB8]
        static_cast<std::int32_t>(Sys_GetGlobal32_4F0FC0()), 7, 0);                       // 0x0042194C
    Script_TaskContainer_Append(
        reinterpret_cast<std::int32_t*>(igi::retail::core::g_57bc04),
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x00)), // 0x0042196B: lea ecx,[esp+0xC]
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0xFC)), // 0x00421964: lea eax,[esp+0x108]
        static_cast<std::int32_t>(Sys_GetGlobal32_4F0FC0()), 7, 0);                       // 0x00421972
    Script_TaskContainer_Append(
        reinterpret_cast<std::int32_t*>(igi::retail::core::g_57bc04),
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x00)), // 0x00421991: lea ecx,[esp+0xC]
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x14C)), // 0x0042198A: lea eax,[esp+0x158]
        static_cast<std::int32_t>(Sys_GetGlobal32_4F0FC0()), 7, 0);                       // 0x00421998
}

} // extern "C"
