// Native: HeliWheel_Register_426D90 (placeholder name; retail symbol unknown; the class-name string it registers is "HeliWheel" at 0x0053BB0C). retail VA 0x00426D90-0x00426E82 (243 bytes, 1 block, 0 edges, 64 instructions).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 `pd` over the exact image plus capstone 5.0.7 aligned from the verified entry 0x00426D90; the two listings agree. Extent: entry begins directly with `push 0; push 0; call 0x004D9FA0` - there is NO `sub esp` prologue - and `c3` sits at 0x00426E82, with 0x90 from 0x00426E83 onward, so the extent is 243 bytes. Push accounting for a FRAMELESS body, replayed with esp simulated and reconciling with zero slack: `add esp,0x38` at 0x00426DE3 = 2 (the two surplus zero words) + 3 (0x00401900 with its pre-pushed fourth argument) + 3 + 3 + 3 (links 1-3); `add esp,0xC` at each of 0x00426DFE, 0x00426E18, 0x00426E33 and 0x00426E4E = 3 dwords each, being the handler immediate PRE-PUSHED before an argument-less getter, plus that getter's return, plus the class id; `add esp,0x14` at 0x00426E7F = 5 dwords = the last handler immediate + its getter's return + the id + the id again + the 0x0053BAF8 name for 0x004B8810. Cleaned in total 31 dwords, pushed in total 31. Call census from the 0xE8 enumeration: 0x004D9FA0 x1, 0x00401900 x1, 0x00401400 x6, 0x00401530 x2, 0x004D9FB0 x1, 0x00416840 x1, 0x00416860 x1, 0x004C56D0 x1, 0x004C56E0 x1, 0x004B8810 x1; 16 call instructions, no branch of any kind.
// Behavior/CFG: one straight-line block, no branch, no loop. The parent getter is 0x004D9FA0 (`mov ax,[0x00548648]; ret`) - the third parent source, as in 0x00426630. It registers "HeliWheel" with a 0x140-byte object through 0x00401900 and stores the 16-bit id at 0x0057BC7C (past the file-backed end of .data, so the image value is zero). Then eight handler links on that id, in retail order: 0x00401400 event 0 -> 0x00426E90; 0x00401530 event 1 -> 0x004D3710; 0x00401530 event 2 -> 0x00426860; 0x00401400 the byte from 0x004D9FB0 -> 0x00426FA0; 0x00401400 the byte from 0x00416840 -> 0x004D3710; 0x00401400 the byte from 0x00416860 -> 0x004D3710; 0x00401400 the byte from 0x004C56D0 -> 0x00427240; 0x00401400 the byte from 0x004C56E0 -> 0x00427F80. The handler immediate is pushed BEFORE each argument-less getter, so it is not that getter's argument; it stays on the stack and the link consumes it as its third argument - which is why those four cleanups are 3 dwords and not 2. Two of the eight handlers are transcribed and are referenced as symbols: 0x004D3710 (`QTask_NoOpHandler`, used THREE times, on events 1 and on the two settings bytes) and 0x00427F80 (`Script_SetConstant_427F80`). Finally it registers the byte-identical name "TASKTYPE_HELIWHEEL" (0x0053BAF8) against the id through 0x004B8810, masking the id to 16 bits first (`and ecx,0xFFFF`).
// STRUCTURAL ABSENCE, measured, not an omission: this routine has NO FRAME (no `sub esp` at any point; the stack is cleaned incrementally at six sites) and NO parameter container (it never calls 0x004F1950 or 0x004F1A80). Its only script-facing act is the single 0x004B8810 name registration.
// Status/limits: TRANSCRIBED; LINKABLE. Of the eight handler-link addresses, six are raw code immediates with no implementation anywhere in the tree - 0x00426E90, 0x00426860, 0x00426FA0, 0x00427240 (and the two 0x004D3710 uses) - so the four unwritten ones are passed as cast integers and create no link dependency; they stay recorded gaps, not stubs. 0x004D3710 and 0x00427F80 ARE transcribed and are referenced as symbols, so both are real link edges. STRUCTURALLY_VERIFIED for the CFG only, which is one block with no edges. BUILD_VERIFIED (i686-w64-mingw32-g++ -std=c++20 -O2 -Wall -Wextra single-file compile, zero warning lines). Runtime UNOBSERVED in both binaries.
// ABI NOTE, carried from 0x00426630 and re-checked here: `Script_SetConstant_427F80` is declared `std::uint32_t (void* const, std::uint32_t* const)` and its body reads its SECOND argument (`mov eax,[esp+8]; mov dword [eax],8`), so it is not a `FrameHandler` = `void(*)(void*)`. A single-step `reinterpret_cast` to FrameHandler emits `-Wcast-function-type`; the symbol reference is kept by casting through `void (*)()` first, which GCC treats as matching everything. Whether retail's dispatcher passes one argument or two is a property of 0x00401400 and the event walker, not of this routine, and it remains UNRESOLVED.
// Ownership note (per the Lead's two-variable trap): 0x0057BC7C is owned by this file and emitted with C++ linkage inside `namespace igi::retail::core`. No other file in the tree references it, and it is past the file-backed end of .data, so the image value is zero.
// Domain: core/, per the Lead's ruling that these files are task-type registrars of the guard 0x00402930 family. This file is one of the 24 registrars the guard calls in descending address order at 0x00402B79-0x00402C32; its teardown is 0x00427280.
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

// 0x0057BC7C: the HeliWheel class id. Past the raw extent of .data, so the image value is zero.
std::uint16_t g_57bc7c = 0U; // 0x0057BC7C

} // namespace igi::retail::core

extern "C" {

// All five of these are single-instruction argument-less readers; names and signatures match the
// declarations already used elsewhere in the tree so the symbols resolve.
extern std::uint16_t NATIVE_DECL Sys_GetGlobal16_4D9FA0(void);    // 0x004D9FA0: `mov ax,[0x548648]; ret` (parent type)
extern std::uint8_t NATIVE_DECL Sys_GetGlobal8_4D9FB0(void);      // 0x004D9FB0: `mov al,[0x54864A]; ret`
extern std::uint8_t NATIVE_DECL Sys_GetSettingByte_416840(void);  // 0x00416840: `mov al,[0x539549]; ret`
extern std::uint8_t NATIVE_DECL Sys_GetSettingByte_416860(void);  // 0x00416860: `mov al,[0x53954B]; ret`
extern std::uint8_t NATIVE_DECL DynObj_GetByte_4C56D0(void);      // 0x004C56D0: `mov al,[0x547F3C]; ret`
extern std::uint8_t NATIVE_DECL DynObj_GetByte_4C56E0(void);      // 0x004C56E0: `mov al,[0x547F3D]; ret`
extern void NATIVE_DECL QTask_NoOpHandler(void);                  // 0x004D3710: single `ret`
extern std::uint32_t Script_SetConstant_427F80(void* const arg0, std::uint32_t* const value); // 0x00427F80
extern void Script_QHashNodeRegister(void* arg1, void* value);    // 0x004B8810 (declared in native_402ff0.cpp)

void NATIVE_DECL HeliWheel_Register_426D90()
{
    NATIVE_LOG_ENTER("HeliWheel_Register_426D90", "0x00426D90");
    // 0x00426D90/0x00426D92: the two zeros are surplus for the argument-less parent getter and supply
    // 0x00401900's fourth argument. The parent getter is 0x004D9FA0, not 0x004018F0 and not 0x00424850.
    const std::uint16_t parent = Sys_GetGlobal16_4D9FA0();                    // 0x00426D94
    const std::uint16_t class_id =
        TaskType_Register("HeliWheel", 0x140U, parent, 0U);                   // 0x00426DA4
    igi::retail::core::g_57bc7c = class_id;                                   // 0x00426DB1
    QTask_LinkQTaskEventHandler(
        class_id, 0U, reinterpret_cast<igi::retail::core::FrameHandler>(0x00426E90)); // 0x00426DB7
    QTask_LinkQTaskEventHandlerNoContextFn(
        class_id, 1U, reinterpret_cast<igi::retail::core::FrameHandler>(&QTask_NoOpHandler)); // 0x00426DCA
    QTask_LinkQTaskEventHandlerNoContextFn(
        class_id, 2U, reinterpret_cast<igi::retail::core::FrameHandler>(0x00426860)); // 0x00426DDE
    // The handler immediate is pre-pushed before each argument-less getter and is consumed by the link.
    QTask_LinkQTaskEventHandler(
        class_id, Sys_GetGlobal8_4D9FB0(),
        reinterpret_cast<igi::retail::core::FrameHandler>(0x00426FA0));       // 0x00426DF9
    QTask_LinkQTaskEventHandler(
        class_id, Sys_GetSettingByte_416840(),
        reinterpret_cast<igi::retail::core::FrameHandler>(&QTask_NoOpHandler)); // 0x00426E13
    QTask_LinkQTaskEventHandler(
        class_id, Sys_GetSettingByte_416860(),
        reinterpret_cast<igi::retail::core::FrameHandler>(&QTask_NoOpHandler)); // 0x00426E2E
    QTask_LinkQTaskEventHandler(
        class_id, DynObj_GetByte_4C56D0(),
        reinterpret_cast<igi::retail::core::FrameHandler>(0x00427240));       // 0x00426E49
    QTask_LinkQTaskEventHandler(
        class_id, DynObj_GetByte_4C56E0(),
        reinterpret_cast<igi::retail::core::FrameHandler>(
            reinterpret_cast<void (*)()>(&Script_SetConstant_427F80)));       // 0x00426E63
    // 0x00426E68: `mov ecx,[0x57BC7C]; and ecx,0xFFFF` then push the id, then push the name at 0x0053BAF8.
    Script_QHashNodeRegister(const_cast<char*>("TASKTYPE_HELIWHEEL"),
                             reinterpret_cast<void*>(static_cast<std::uintptr_t>(class_id))); // 0x00426E7A
}

} // extern "C"
