// Native: RocketPod_Register_427830 (placeholder name; retail symbol unknown; the class-name string it registers is "RocketPod" at 0x0053BB54). retail VA 0x00427830-0x0042793A (267 bytes, 1 block, 0 edges, 71 instructions).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 `pd` over the exact image plus capstone 5.0.7 aligned from the verified entry 0x00427830; the two listings agree. Extent: entry begins directly with `push 0; push 0; call 0x004D9FA0` - there is NO `sub esp` prologue - and `c3` sits at 0x0042793A, with 0x90 from 0x0042793B onward, so the extent is 267 bytes. Push accounting for a FRAMELESS body, replayed with esp simulated and reconciling with zero slack: `add esp,0x38` at 0x00427883 = 2 (the two surplus zero words) + 3 (0x00401900 with its pre-pushed fourth argument) + 3 + 3 + 3 (links 1-3); `add esp,0xC` at each of 0x0042789E, 0x004278B8, 0x004278D0, 0x004278EB and 0x00427905 = 3 dwords each, being the handler immediate PRE-PUSHED before an argument-less getter, plus that getter's return, plus the class id; `add esp,0x14` at 0x00427937 = 5 dwords = the last handler immediate + its getter's return + the id + the id again + the 0x0053BB40 name for 0x004B8810. Cleaned in total 34 dwords, pushed in total 34. Call census from the 0xE8 enumeration: 0x004D9FA0 x1, 0x00401900 x1, 0x00401400 x7, 0x00401530 x2, 0x004D9FB0 x1, 0x00416840 x1, 0x00416860 x1, 0x004C56D0 x1, 0x004C56E0 x1, 0x00489610 x1, 0x004B8810 x1; 18 call instructions, no branch of any kind.
// Behavior/CFG: one straight-line block, no branch, no loop. The parent getter is 0x004D9FA0 (`mov ax,[0x00548648]; ret`), the third parent source, fourth consecutive file. It registers "RocketPod" with a 0x140-byte object through 0x00401900 and stores the 16-bit id at 0x0053BB3C (image word 0x0181, the not-registered sentinel). Then nine handler links on that id, in retail order: 0x00401400 event 0 -> 0x00427940; 0x00401530 event 1 -> 0x00427A70; 0x00401530 event 2 -> 0x00426860; 0x00401400 the byte from 0x004D9FB0 -> 0x00426FA0; 0x00401400 the byte from 0x00416840 -> 0x0042D9B0; 0x00401400 the byte from 0x00416860 -> handler literal 0; 0x00401400 the byte from 0x004C56D0 -> 0x00427D40; 0x00401400 the byte from 0x004C56E0 -> 0x00427D70; 0x00401400 the byte from 0x00489610 -> 0x00426D30. The handler immediate is pushed BEFORE each argument-less getter, so it is not that getter's argument; it stays on the stack and the link consumes it as its third argument, which is why those five cleanups are 3 dwords and not 2. Finally it registers the byte-identical name "TASKTYPE_ROCKETPOD" (0x0053BB40) against the id through 0x004B8810, masking the id to 16 bits first (`and edx,0xFFFF`).
// CROSS-FILE RELATIONSHIP, measured: this is the third member of the nine-link twin family - 0x00426630 ("CarDoor", size 0x148), 0x004272A0 ("HeliDoor", size 0x148) and this one (size 0x140). All three share the same getter order, the same three fixed events, the same NULL handler on the 0x00416860 link, and the same seven handler addresses except for the first, fourth, seventh and eighth links. Only 0x00427940, 0x00427A70 and 0x00427D40 are new here; the other six handler addresses were already recorded in files 11-13.
// MEASURED NULL HANDLER: link 6 registers the literal immediate 0, third file with the same null-handler slot and the same 3-dword cleanup.
// STRUCTURAL ABSENCE, measured, not an omission: NO FRAME (no `sub esp` at any point; the stack is cleaned incrementally at seven sites) and NO parameter container (never calls 0x004F1950 or 0x004F1A80). Its only script-facing act is the single 0x004B8810 name registration.
// Status/limits: TRANSCRIBED; LINKABLE. Of the nine handler-link addresses, six are raw code immediates with no implementation anywhere in the tree - 0x00427940, 0x00427A70, 0x00426860, 0x00426FA0, 0x0042D9B0, 0x00427D40 - plus 0x00426D30, already recorded; they are passed as cast integers and create no link dependency, and stay recorded gaps, not stubs. 0x00427D70 IS transcribed and is referenced as a symbol, so it is a real link edge. STRUCTURALLY_VERIFIED for the CFG only, which is one block with no edges. BUILD_VERIFIED (i686-w64-mingw32-g++ -std=c++20 -O2 -Wall -Wextra single-file compile, zero warning lines). Runtime UNOBSERVED in both binaries.
// ABI NOTE, third sample of the same population: `Script_SetConstant_427D70` is `std::uint32_t (void* const, std::uint32_t* const)`, reads its SECOND argument and stores 0x4 through it. Its own provenance records that it is "one of the 84 handler pointers registered by 0x004EE970", i.e. this family is BUILT to take a second argument from a two-argument dispatcher. Its appearance here as a `FrameHandler` link is therefore a genuine signature mismatch between two dispatch conventions, NOT evidence that frame handlers generally take two arguments. The symbol reference is kept by casting through `void (*)()` first, which compiles with zero warning lines and preserves the link edge; the dispatcher's argument count remains UNRESOLVED.
// Ownership note (per the Lead's two-variable trap): 0x0053BB3C is owned by this file and emitted with C++ linkage inside `namespace igi::retail::core`. No other file in the tree references it, and the image value is the sentinel word 0x0181, not zero.
// Domain: core/, per the Lead's ruling that these files are task-type registrars of the guard 0x00402930 family. This file is one of the 24 registrars the guard calls in descending address order at 0x00402B79-0x00402C32; its teardown is 0x00427D80.
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

// 0x0053BB3C: the RocketPod class id. The retail image holds the word 0x0181 here, the "not registered"
// sentinel, NOT zero.
std::uint16_t g_53bb3c = 0x0181U; // 0x0053BB3C

} // namespace igi::retail::core

extern "C" {

// All six of these are single-instruction argument-less readers; names and signatures match the
// declarations already used elsewhere in the tree so the symbols resolve.
extern std::uint16_t NATIVE_DECL Sys_GetGlobal16_4D9FA0(void);    // 0x004D9FA0: `mov ax,[0x548648]; ret` (parent type)
extern std::uint8_t NATIVE_DECL Sys_GetGlobal8_4D9FB0(void);      // 0x004D9FB0: `mov al,[0x54864A]; ret`
extern std::uint8_t NATIVE_DECL Sys_GetSettingByte_416840(void);  // 0x00416840: `mov al,[0x539549]; ret`
extern std::uint8_t NATIVE_DECL Sys_GetSettingByte_416860(void);  // 0x00416860: `mov al,[0x53954B]; ret`
extern std::uint8_t NATIVE_DECL DynObj_GetByte_4C56D0(void);      // 0x004C56D0: `mov al,[0x547F3C]; ret`
extern std::uint8_t NATIVE_DECL DynObj_GetByte_4C56E0(void);      // 0x004C56E0: `mov al,[0x547F3D]; ret`
extern std::uint8_t NATIVE_DECL Sys_GetGlobal8_489610(void);      // 0x00489610: `mov al,[0x54127C]; ret`
extern std::uint32_t Script_SetConstant_427D70(void* const arg0, std::uint32_t* const value); // 0x00427D70
extern void Script_QHashNodeRegister(void* arg1, void* value);    // 0x004B8810 (declared in native_402ff0.cpp)

void NATIVE_DECL RocketPod_Register_427830()
{
    NATIVE_LOG_ENTER("RocketPod_Register_427830", "0x00427830");
    // 0x00427830/0x00427832: the two zeros are surplus for the argument-less parent getter and supply
    // 0x00401900's fourth argument. The parent getter is 0x004D9FA0, not 0x004018F0 and not 0x00424850.
    const std::uint16_t parent = Sys_GetGlobal16_4D9FA0();                    // 0x00427834
    const std::uint16_t class_id =
        TaskType_Register("RocketPod", 0x140U, parent, 0U);                   // 0x00427844
    igi::retail::core::g_53bb3c = class_id;                                   // 0x00427851
    QTask_LinkQTaskEventHandler(
        class_id, 0U, reinterpret_cast<igi::retail::core::FrameHandler>(0x00427940)); // 0x00427857
    QTask_LinkQTaskEventHandlerNoContextFn(
        class_id, 1U, reinterpret_cast<igi::retail::core::FrameHandler>(0x00427A70)); // 0x0042786A
    QTask_LinkQTaskEventHandlerNoContextFn(
        class_id, 2U, reinterpret_cast<igi::retail::core::FrameHandler>(0x00426860)); // 0x0042787E
    // The handler immediate is pre-pushed before each argument-less getter and is consumed by the link.
    QTask_LinkQTaskEventHandler(
        class_id, Sys_GetGlobal8_4D9FB0(),
        reinterpret_cast<igi::retail::core::FrameHandler>(0x00426FA0));       // 0x00427899
    QTask_LinkQTaskEventHandler(
        class_id, Sys_GetSettingByte_416840(),
        reinterpret_cast<igi::retail::core::FrameHandler>(0x0042D9B0));       // 0x004278B3
    // 0x004278BB: the pre-pushed value is the literal 0, so this link registers a NULL handler.
    QTask_LinkQTaskEventHandler(
        class_id, Sys_GetSettingByte_416860(),
        static_cast<igi::retail::core::FrameHandler>(nullptr));               // 0x004278CB
    QTask_LinkQTaskEventHandler(
        class_id, DynObj_GetByte_4C56D0(),
        reinterpret_cast<igi::retail::core::FrameHandler>(0x00427D40));       // 0x004278E6
    QTask_LinkQTaskEventHandler(
        class_id, DynObj_GetByte_4C56E0(),
        reinterpret_cast<igi::retail::core::FrameHandler>(
            reinterpret_cast<void (*)()>(&Script_SetConstant_427D70)));       // 0x00427900
    QTask_LinkQTaskEventHandler(
        class_id, Sys_GetGlobal8_489610(),
        reinterpret_cast<igi::retail::core::FrameHandler>(0x00426D30));       // 0x0042791B
    // 0x00427920: `mov edx,[0x53BB3C]; and edx,0xFFFF` then push the id, then push the name at 0x0053BB40.
    Script_QHashNodeRegister(const_cast<char*>("TASKTYPE_ROCKETPOD"),
                             reinterpret_cast<void*>(static_cast<std::uintptr_t>(class_id))); // 0x00427932
}

} // extern "C"
