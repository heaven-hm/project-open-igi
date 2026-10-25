// Native: HitZone_Register_427D90 (placeholder name; retail symbol unknown; the class-name string it registers is "HitZone" at 0x0053BB84). retail VA 0x00427D90-0x00427F29 (410 bytes, 1 block, 0 edges, 114 instructions).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 `pd` over the exact image plus capstone 5.0.7 aligned from the verified entry 0x00427D90; the two listings agree. Extent: entry begins directly with `push 0; push 0; call 0x004D9FA0` - there is NO `sub esp` prologue - and `c3` sits at 0x00427F29, with 0x90 from 0x00427F2A onward, so the extent is 410 bytes. Push accounting for a FRAMELESS body, replayed with esp simulated from the entry and reconciling exactly: 55 pushes in total, 11 incremental cleanups (`add esp,0x38` at 0x00427DE3 = 14 dwords; `add esp,0xC` at each of 0x00427DFE, 0x00427E15, 0x00427E2D, 0x00427E48, 0x00427E62, 0x00427E7D, 0x00427ED1 and 0x00427EE8 = 3 dwords each; `add esp,0x24` at 0x00427EB9 = 9 dwords; `add esp,0x20` at 0x00427F26 = 8 dwords), and the simulated esp is back to its entry value at the `ret`, so the stack is balanced with zero slack. Call census from the 0xE8 enumeration: 0x004D9FA0 x1, 0x00401900 x1, 0x00401400 x14, 0x00401530 x2, 0x004D9FB0 x1, 0x00416840 x1, 0x00416860 x1, 0x004C56D0 x1, 0x004C56E0 x1, 0x004F4860 x1, 0x004D14D0 x1, 0x004CEA20 x1, 0x004CEA10 x1, 0x004C56F0 x1, 0x004B8810 x1; 29 call instructions, no branch of any kind.
// Behavior/CFG: one straight-line block, no branch, no loop. The parent getter is 0x004D9FA0 (`mov ax,[0x00548648]; ret`), the third parent source. It registers "HitZone" with a 0x148-byte object through 0x00401900 and stores the 16-bit id at 0x0053BB6C (image word 0x0181, the not-registered sentinel). Then SIXTEEN handler links on that id, in retail order: 0x00401400 event 0 -> 0x00427F30; 0x00401530 event 1 -> 0x004D3710; 0x00401530 event 2 -> 0x004D3710; 0x00401400 the byte from 0x004D9FB0 -> 0x004D3710; 0x00401400 the byte from 0x00416840 -> 0; 0x00401400 the byte from 0x00416860 -> 0; 0x00401400 the byte from 0x004C56D0 -> 0x00427F50; 0x00401400 the byte from 0x004C56E0 -> 0x00427F80; 0x00401400 the byte from 0x004F4860 -> 0x00427F90; 0x00401400 the byte from 0x004D14D0 -> 0; 0x00401400 literal event 9 -> 0; 0x00401400 literal event 0xC -> 0x0045F920; 0x00401400 the byte from 0x004CEA20 -> 0; 0x00401400 the byte from 0x004CEA10 -> 0; 0x00401400 the byte from 0x004C56F0 -> 0; 0x00401400 literal event 4 -> 0. Every handler immediate is pushed BEFORE its argument-less getter, so it is not that getter's argument; it stays on the stack and the link consumes it as its third argument, which is why those cleanups are 3 dwords and not 2. Finally it registers the byte-identical name "TASKTYPE_HITZONE" (0x0053BB70) against the id through 0x004B8810, masking the id to 16 bits first (`and eax,0xFFFF`).
// EIGHT MEASURED NULL HANDLERS: links 5, 6, 10, 11, 13, 14, 15 and 16 all carry the literal immediate 0 as the handler. Links 10, 11 and 16 push the 0 directly as the handler with a literal event (the 0x004D14D0 link pushes 0 as the getter's argument, the 9 and 4 links push it as the event's pre-push); the rest pre-push it before an argument-less getter. Half of this class's event table is deliberately unhandled, and the port keeps every one as a null pointer - none of them is an untranscribed address, and filling any of them in would invent behaviour.
// TWO MORE LITERAL EVENTS: link 11 uses event 9 and link 16 uses event 4 with the handler pushed directly, so those two events do NOT come from a getter at all. A transcription that routed every event through a getter would be wrong for these two.
// STRUCTURAL ABSENCE, measured, not an omission: NO FRAME (no `sub esp` at any point; the stack is cleaned incrementally at eleven sites) and NO parameter container (never calls 0x004F1950 or 0x004F1A80). Its only script-facing act is the single 0x004B8810 name registration.
// Status/limits: TRANSCRIBED; LINKABLE. Of the sixteen handler-link addresses, five are distinct raw code immediates with no implementation anywhere in the tree - 0x00427F30, 0x00427F50, 0x00427F90, 0x0045F920 - so they are passed as cast integers and create no link dependency, and stay recorded gaps, not stubs. 0x004D3710 (`QTask_NoOpHandler`) and 0x00427F80 (`Script_SetConstant_427F80`) ARE transcribed and are referenced as symbols, so both are real link edges. STRUCTURALLY_VERIFIED for the CFG only, which is one block with no edges. BUILD_VERIFIED (i686-w64-mingw32-g++ -std=c++20 -O2 -Wall -Wextra single-file compile, zero warning lines). Runtime UNOBSERVED in both binaries.
// ABI NOTE, and this file adds a fourth sample to the population: 0x0045F920 is `mov eax,[esp+8]; mov byte [eax],0; ret` - the same two-argument script-setter shape as the `Script_SetConstant_*` family, and it is NOT transcribed. It is recorded as a cast integer, so no signature is asserted for it. The `Script_SetConstant_427F80` reference keeps the symbol edge through the two-step `void (*)()` cast, zero warning lines; the dispatcher's argument count remains UNRESOLVED.
// Ownership note (per the Lead's two-variable trap): 0x0053BB6C is owned by this file and emitted with C++ linkage inside `namespace igi::retail::core`. No other file in the tree references it, and the image value is the sentinel word 0x0181, not zero.
// Domain: core/, per the Lead's ruling that these files are task-type registrars of the guard 0x00402930 family. This file is one of the 24 registrars the guard calls in descending address order at 0x00402B79-0x00402C32; its teardown is 0x00427FB0.
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

// 0x0053BB6C: the HitZone class id. The retail image holds the word 0x0181 here, the "not registered"
// sentinel, NOT zero.
std::uint16_t g_53bb6c = 0x0181U; // 0x0053BB6C

} // namespace igi::retail::core

extern "C" {

// All nine of these are single-instruction argument-less byte readers; names and signatures match the
// declarations already used elsewhere in the tree so the symbols resolve.
extern std::uint16_t NATIVE_DECL Sys_GetGlobal16_4D9FA0(void);    // 0x004D9FA0: `mov ax,[0x548648]; ret` (parent type)
extern std::uint8_t NATIVE_DECL Sys_GetGlobal8_4D9FB0(void);      // 0x004D9FB0: `mov al,[0x54864A]; ret`
extern std::uint8_t NATIVE_DECL Sys_GetSettingByte_416840(void);  // 0x00416840: `mov al,[0x539549]; ret`
extern std::uint8_t NATIVE_DECL Sys_GetSettingByte_416860(void);  // 0x00416860: `mov al,[0x53954B]; ret`
extern std::uint8_t NATIVE_DECL DynObj_GetByte_4C56D0(void);      // 0x004C56D0: `mov al,[0x547F3C]; ret`
extern std::uint8_t NATIVE_DECL DynObj_GetByte_4C56E0(void);      // 0x004C56E0: `mov al,[0x547F3D]; ret`
extern std::uint8_t NATIVE_DECL DynObj_GetByte_4C56F0(void);      // 0x004C56F0: `mov al,[0x547F3E]; ret`
extern std::uint8_t NATIVE_DECL Sys_GetGlobal8_4F4860(void);      // 0x004F4860: `mov al,[0x54E203]; ret`
extern std::uint8_t NATIVE_DECL Render_GetByte_4D14D0(void);     // 0x004D14D0: `mov al,[0x548440]; ret` (render/native_4d14d0.cpp)
extern std::uint8_t NATIVE_DECL QTask_PhaseIdGet548177_4CEA20(void); // 0x004CEA20: `mov al,[0x548177]; ret`
extern std::uint8_t NATIVE_DECL QTask_PhaseIdGet548176_4CEA10(void); // 0x004CEA10: `mov al,[0x548176]; ret`
extern void NATIVE_DECL QTask_NoOpHandler(void);                  // 0x004D3710: single `ret`
extern std::uint32_t Script_SetConstant_427F80(void* const arg0, std::uint32_t* const value); // 0x00427F80
extern void Script_QHashNodeRegister(void* arg1, void* value);    // 0x004B8810 (declared in native_402ff0.cpp)

void NATIVE_DECL HitZone_Register_427D90()
{
    NATIVE_LOG_ENTER("HitZone_Register_427D90", "0x00427D90");
    // 0x00427D90/0x00427D92: the two zeros are surplus for the argument-less parent getter and supply
    // 0x00401900's fourth argument. The parent getter is 0x004D9FA0, not 0x004018F0 and not 0x00424850.
    const std::uint16_t parent = Sys_GetGlobal16_4D9FA0();                    // 0x00427D94
    const std::uint16_t class_id =
        TaskType_Register("HitZone", 0x148U, parent, 0U);                     // 0x00427DA4
    igi::retail::core::g_53bb6c = class_id;                                   // 0x00427DB1
    QTask_LinkQTaskEventHandler(
        class_id, 0U, reinterpret_cast<igi::retail::core::FrameHandler>(0x00427F30)); // 0x00427DB7
    QTask_LinkQTaskEventHandlerNoContextFn(
        class_id, 1U, reinterpret_cast<igi::retail::core::FrameHandler>(&QTask_NoOpHandler)); // 0x00427DCA
    QTask_LinkQTaskEventHandlerNoContextFn(
        class_id, 2U, reinterpret_cast<igi::retail::core::FrameHandler>(&QTask_NoOpHandler)); // 0x00427DDE
    // The handler immediate is pre-pushed before each argument-less getter and is consumed by the link.
    QTask_LinkQTaskEventHandler(
        class_id, Sys_GetGlobal8_4D9FB0(),
        reinterpret_cast<igi::retail::core::FrameHandler>(&QTask_NoOpHandler)); // 0x00427DF9
    QTask_LinkQTaskEventHandler(
        class_id, Sys_GetSettingByte_416840(),
        static_cast<igi::retail::core::FrameHandler>(nullptr));               // 0x00427E10
    QTask_LinkQTaskEventHandler(
        class_id, Sys_GetSettingByte_416860(),
        static_cast<igi::retail::core::FrameHandler>(nullptr));               // 0x00427E28
    QTask_LinkQTaskEventHandler(
        class_id, DynObj_GetByte_4C56D0(),
        reinterpret_cast<igi::retail::core::FrameHandler>(0x00427F50));       // 0x00427E43
    QTask_LinkQTaskEventHandler(
        class_id, DynObj_GetByte_4C56E0(),
        reinterpret_cast<igi::retail::core::FrameHandler>(
            reinterpret_cast<void (*)()>(&Script_SetConstant_427F80)));       // 0x00427E5D
    QTask_LinkQTaskEventHandler(
        class_id, Sys_GetGlobal8_4F4860(),
        reinterpret_cast<igi::retail::core::FrameHandler>(0x00427F90));       // 0x00427E78
    // 0x00427E80: the pre-pushed 0 is the handler; the event is the getter's return.
    QTask_LinkQTaskEventHandler(
        class_id, Render_GetByte_4D14D0(),
        static_cast<igi::retail::core::FrameHandler>(nullptr));               // 0x00427E90
    // 0x00427E9B: literal event 9 with the handler pushed directly.
    QTask_LinkQTaskEventHandler(
        class_id, 9U, static_cast<igi::retail::core::FrameHandler>(nullptr)); // 0x00427EA0
    // 0x00427EAC: literal event 0xC with a fixed handler immediate.
    QTask_LinkQTaskEventHandler(
        class_id, 0xCU, reinterpret_cast<igi::retail::core::FrameHandler>(0x0045F920)); // 0x00427EB4
    QTask_LinkQTaskEventHandler(
        class_id, QTask_PhaseIdGet548177_4CEA20(),
        static_cast<igi::retail::core::FrameHandler>(nullptr));               // 0x00427ECC
    QTask_LinkQTaskEventHandler(
        class_id, QTask_PhaseIdGet548176_4CEA10(),
        static_cast<igi::retail::core::FrameHandler>(nullptr));               // 0x00427EE3
    QTask_LinkQTaskEventHandler(
        class_id, DynObj_GetByte_4C56F0(),
        static_cast<igi::retail::core::FrameHandler>(nullptr));               // 0x00427EFB
    // 0x00427F07: literal event 4 with the handler pushed directly.
    QTask_LinkQTaskEventHandler(
        class_id, 4U, static_cast<igi::retail::core::FrameHandler>(nullptr)); // 0x00427F0C
    // 0x00427F11: `mov eax,[0x53BB6C]; and eax,0xFFFF` then push the id, then push the name at 0x0053BB70.
    Script_QHashNodeRegister(const_cast<char*>("TASKTYPE_HITZONE"),
                             reinterpret_cast<void*>(static_cast<std::uintptr_t>(class_id))); // 0x00427F21
}

} // extern "C"
