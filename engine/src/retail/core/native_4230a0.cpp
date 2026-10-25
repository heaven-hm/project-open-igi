// Native: ControlsMenu_Register_4230A0 (placeholder name; retail symbol unknown; the class-name string it registers is "ControlsMenu" at 0x0053B8C0). retail VA 0x004230A0-0x004231D7 (312 bytes, 1 block, 0 edges, 83 instructions).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 `pd` over the exact image plus capstone 5.0.7 aligned from the verified entry 0x004230A0; the two listings agree. Extent: entry `push 0` (there is NO frame allocation in this routine - no `sub esp` anywhere), `c3` at 0x004231D7, and the byte after the ret is 0x90 (0x004231D8 onward is alignment padding, NOT part of the routine), so the extent is 312 bytes. Epilogue `add esp,0x2C` = 0x2C of pending arguments only, because there is no frame. Push accounting, replayed with esp simulated and reconciling with zero slack: `add esp,0x20` at 0x004230CC = 2 (0x00424850 pre-zeros) + 3 (0x00401900 with its pre-pushed fourth argument) + 3 (0x00401530); `add esp,0x24` at 0x0042310E = 3+3+3 (the 0x004E7530-driven link, the event-2 link and the 0x00401400 event-0 link); `add esp,0xC` at each of 0x00423128, 0x00423143, 0x0042315E, 0x00423178 and 0x00423193 = one three-argument link each; `add esp,0x2C` at 0x004231D4 = 3 (the 0x00424880-driven link) + 4 + 4 (the two 0x004B8890 registrations, four pushes each). Call census from the 0xE8 enumeration, not from handler immediates: 0x00424850 x1, 0x00401900 x1, 0x00401530 x6, 0x00401400 x3, 0x004E7530 x1, 0x004F1A60 x1, 0x004F1A70 x1, 0x004F1210 x1, 0x004F1220 x1, 0x00424870 x1, 0x00424880 x1, 0x004B8890 x2; 21 call instructions in all, no branch of any kind.
// Behavior/CFG: one straight-line block, no branch, no loop. Reads the parent type through 0x00424850, registers "ControlsMenu" with a 0x364-byte object through 0x00401900 and stores the 16-bit id at 0x0053B874 (image word 0x0181, the not-registered sentinel). Then makes nine handler links on that id, in retail order: 0x00401530 to 0x004234E0 on event 1; 0x00401530 to 0x004238B0 on the event word from 0x004E7530; 0x00401530 to 0x00423350 on event 2; 0x00401400 to 0x004232B0 on event 0; 0x00401400 to 0x00423CA0 on the phase id from 0x004F1A60; 0x00401400 to 0x004233D0 on the phase id from 0x004F1A70; 0x00401530 to 0x00423CB0 on the word from 0x004F1210; 0x00401530 to 0x00423D30 on the word from 0x004F1220; 0x00401530 to 0x00423DB0 on the word from 0x00424870; 0x00401530 to 0x00423EB0 on the word from 0x00424880. That is ten links across the six 0x00401530 and three 0x00401400 calls. It then makes TWO script registrations through 0x004B8890, each four pushes, which is the only thing in this file that is not a handler link: "ControlsMenu_ConvertFloatFromOffset" (string at 0x0053B89C) with code pointer 0x004231E0, and "ControlsMenu_ConvertOffsetFromFloat" (string at 0x0053B878) with code pointer 0x00423240. Argument order for 0x004B8890 was checked against the existing tree call sites in engine/src/retail/core/native_402ff0.cpp and against retail at 0x00402FF8 (push 1, push 0, push handler, push name), so the port calls it as (name, handler, nullptr, 1) - the LAST push is the string, so the string is argument 0. NOTE: this registrar allocates NO parameter container (no 0x004F1950, no 0x004F1A80) and has no frame, unlike its siblings in this band. That is measured, not an omission. NOTE: the registered object size is 0x364; there is no frame, so there is no frame-size coincidence to compare against here.
// Status/limits: TRANSCRIBED; LINKABLE (the twelve handler/option entry points this routine registers are raw code immediates in retail and are transcribed by nobody, so they are passed as cast integers and create no link dependency; they stay recorded gaps in the tree, not stubs). STRUCTURALLY_VERIFIED for the CFG only, which is one block with no edges. Runtime UNOBSERVED in both binaries. There are no `lea`-derived frame offsets in this file because there is no frame; the only derived quantity is the 0x2C epilogue, which is the sum of the pushes listed above.
// Linkage note (per the Lead's trap): the single global this file OWNS is emitted with C++ linkage because it sits in `namespace igi::retail::core`. A future file that declares 0x0053B874 as `extern "C"` would get a SECOND, distinct variable with NO link error.
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

// 0x0053B874: the ControlsMenu class id. The retail image holds the word 0x0181 here (px 4 -> 81 01 00 00),
// the "not registered" sentinel, NOT zero.
std::uint16_t g_53b874 = 0x0181U; // 0x0053B874

} // namespace igi::retail::core

extern "C" {

extern std::uint16_t NATIVE_DECL Sys_GetGlobal16_424850(void); // 0x00424850: argument-less 16-bit getter (defined in engine/src/retail/sys/native_424850.cpp)
extern std::uint32_t NATIVE_DECL Sys_GetGlobal32_424870(void); // 0x00424870: argument-less 32-bit getter (defined in engine/src/retail/sys/native_424870.cpp)
extern std::uint32_t NATIVE_DECL Sys_GetGlobal32_424880(void); // 0x00424880: argument-less 32-bit getter (defined in engine/src/retail/sys/native_424880.cpp)
extern std::uint32_t NATIVE_DECL Core_GetEventWord_4E7530();   // 0x004E7530: the current event word
extern std::uint32_t NATIVE_DECL Sys_GetGlobal32_4F1210(void); // 0x004F1210
extern std::uint32_t NATIVE_DECL Sys_GetGlobal32_4F1220(void); // 0x004F1220
extern std::uint8_t Script_PhaseIdGetE4_4F1A60(void);          // 0x004F1A60: argument-less byte getter
extern std::uint8_t Script_PhaseIdGetE5_4F1A70(void);          // 0x004F1A70: argument-less byte getter
// Declared in engine/src/retail/core/native_402ff0.cpp:231; argument 0 is the name string, argument 1 the code
// pointer, argument 3 the type. Verified against retail at 0x00402FF8, whose push order is 1, 0, handler, name.
extern void Script_QHashNodeRegisterEx(void* arg0, void* arg1, char const* arg2, void* arg3); // 0x004B8890

void NATIVE_DECL ControlsMenu_Register_4230A0()
{
    NATIVE_LOG_ENTER("ControlsMenu_Register_4230A0", "0x004230A0");
    // 0x004230A0/0x004230A2: the two zeros are surplus for the argument-less getter and are consumed as
    // 0x00401900's fourth argument (task list = 0) - they are not arguments to 0x00424850, which reads none.
    const std::uint16_t parent = Sys_GetGlobal16_424850();                    // 0x004230A4
    const std::uint16_t class_id =
        TaskType_Register("ControlsMenu", 0x364U, parent, 0U);                // 0x004230B4
    igi::retail::core::g_53b874 = class_id;                                   // 0x004230C1
    QTask_LinkQTaskEventHandlerNoContextFn(
        class_id, 1U, reinterpret_cast<igi::retail::core::FrameHandler>(0x004234E0)); // 0x004230C7
    // 0x004230CF: the handler immediate is pre-pushed and is the surplus argument of 0x004E7530, which
    // reads none; 0x004E7530's return is the event word.
    QTask_LinkQTaskEventHandlerNoContextFn(
        class_id, Core_GetEventWord_4E7530(),
        reinterpret_cast<igi::retail::core::FrameHandler>(0x004238B0));      // 0x004230E1
    QTask_LinkQTaskEventHandlerNoContextFn(
        class_id, 2U, reinterpret_cast<igi::retail::core::FrameHandler>(0x00423350)); // 0x004230F5
    QTask_LinkQTaskEventHandler(
        class_id, 0U, reinterpret_cast<igi::retail::core::FrameHandler>(0x004232B0)); // 0x00423109
    QTask_LinkQTaskEventHandler(
        class_id, static_cast<std::uint32_t>(Script_PhaseIdGetE4_4F1A60()),
        reinterpret_cast<igi::retail::core::FrameHandler>(0x00423CA0));       // 0x00423123
    QTask_LinkQTaskEventHandler(
        class_id, static_cast<std::uint32_t>(Script_PhaseIdGetE5_4F1A70()),
        reinterpret_cast<igi::retail::core::FrameHandler>(0x004233D0));       // 0x0042313E
    // Each of the next four links pre-pushes its handler immediate, which is the surplus argument of the
    // argument-less getter in front of it.
    QTask_LinkQTaskEventHandlerNoContextFn(
        class_id, Sys_GetGlobal32_4F1210(),
        reinterpret_cast<igi::retail::core::FrameHandler>(0x00423CB0));       // 0x00423159
    QTask_LinkQTaskEventHandlerNoContextFn(
        class_id, Sys_GetGlobal32_4F1220(),
        reinterpret_cast<igi::retail::core::FrameHandler>(0x00423D30));       // 0x00423173
    QTask_LinkQTaskEventHandlerNoContextFn(
        class_id, Sys_GetGlobal32_424870(),
        reinterpret_cast<igi::retail::core::FrameHandler>(0x00423DB0));       // 0x0042318E
    QTask_LinkQTaskEventHandlerNoContextFn(
        class_id, Sys_GetGlobal32_424880(),
        reinterpret_cast<igi::retail::core::FrameHandler>(0x00423EB0));       // 0x004231A9

    // 0x004231AE..0x004231BC and 0x004231C1..0x004231CF: two four-push script registrations. Retail pushes
    // 1, 0, the code pointer, then the name, so the name is argument 0 and the type is argument 3.
    Script_QHashNodeRegisterEx(
        const_cast<char*>("ControlsMenu_ConvertFloatFromOffset"),             // 0x004231B7: push 0x0053B89C
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x004231E0U)),   // 0x004231B2: push 0x004231E0
        nullptr,                                                              // 0x004231B0: push 0
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(1U)));            // 0x004231AE: push 1
    Script_QHashNodeRegisterEx(
        const_cast<char*>("ControlsMenu_ConvertOffsetFromFloat"),             // 0x004231CA: push 0x0053B878
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x00423240U)),   // 0x004231C5: push 0x00423240
        nullptr,                                                              // 0x004231C3: push 0
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(1U)));            // 0x004231C1: push 1
}

} // extern "C"
