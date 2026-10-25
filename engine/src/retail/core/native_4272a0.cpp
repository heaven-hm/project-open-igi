// Native: HeliDoor_Register_4272A0 (placeholder name; retail symbol unknown; the class-name string it registers is "HeliDoor" at 0x0053BB30). retail VA 0x004272A0-0x004273AA (267 bytes, 1 block, 0 edges, 71 instructions).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 `pd` over the exact image plus capstone 5.0.7 aligned from the verified entry 0x004272A0; the two listings agree. Extent: entry begins directly with `push 0; push 0; call 0x004D9FA0` - there is NO `sub esp` prologue - and `c3` sits at 0x004273AA, with 0x90 from 0x004273AB onward, so the extent is 267 bytes. Push accounting for a FRAMELESS body, replayed with esp simulated and reconciling with zero slack: `add esp,0x38` at 0x004272F3 = 2 (the two surplus zero words) + 3 (0x00401900 with its pre-pushed fourth argument) + 3 + 3 + 3 (links 1-3); `add esp,0xC` at each of 0x0042730E, 0x00427328, 0x00427340, 0x0042735B and 0x00427375 = 3 dwords each, being the handler immediate PRE-PUSHED before an argument-less getter, plus that getter's return, plus the class id; `add esp,0x14` at 0x004273A7 = 5 dwords = the last handler immediate + its getter's return + the id + the id again + the 0x0053BB1C name for 0x004B8810. Cleaned in total 34 dwords, pushed in total 34. Call census from the 0xE8 enumeration: 0x004D9FA0 x1, 0x00401900 x1, 0x00401400 x7, 0x00401530 x2, 0x004D9FB0 x1, 0x00416840 x1, 0x00416860 x1, 0x004C56D0 x1, 0x004C56E0 x1, 0x00489610 x1, 0x004B8810 x1; 18 call instructions, no branch of any kind.
// Behavior/CFG: one straight-line block, no branch, no loop. The parent getter is 0x004D9FA0 (`mov ax,[0x00548648]; ret`), the third parent source. It registers "HeliDoor" with a 0x148-byte object through 0x00401900 and stores the 16-bit id at 0x0053BB18 (image word 0x0181, the not-registered sentinel). Then nine handler links on that id, in retail order: 0x00401400 event 0 -> 0x00426740; 0x00401530 event 1 -> 0x004273B0; 0x00401530 event 2 -> 0x00426860; 0x00401400 the byte from 0x004D9FB0 -> 0x004274E0; 0x00401400 the byte from 0x00416840 -> 0x0042D9B0; 0x00401400 the byte from 0x00416860 -> handler literal 0; 0x00401400 the byte from 0x004C56D0 -> 0x004277C0; 0x00401400 the byte from 0x004C56E0 -> 0x0051BAA0; 0x00401400 the byte from 0x00489610 -> 0x00426D30. The handler immediate is pushed BEFORE each argument-less getter, so it is not that getter's argument; it stays on the stack and the link consumes it as its third argument, which is why those five cleanups are 3 dwords and not 2. Finally it registers the byte-identical name "TASKTYPE_HELIDOOR" (0x0053BB1C) against the id through 0x004B8810, masking the id to 16 bits first (`and edx,0xFFFF`).
// CROSS-FILE RELATIONSHIP, measured, not assumed: this routine and 0x00426630 ("CarDoor") are structural twins - same object size 0x148, same nine-link shape, the same seven getters in the same order, the same three fixed events (0, 1, 2) and the same NULL handler on the 0x00416860 link. They differ in exactly four places: the class name, the id slot, the script name, and the two handlers 0x004274E0 and 0x004277C0, which replace 0x00426880 and 0x00426CD0. Every other handler address is shared between the two. That sharing is why several of these addresses have references in unrelated files, and it is the reason a "new handler" in one registrar is often not new at all.
// MEASURED NULL HANDLER: link 6 passes the literal immediate 0 as the handler, exactly as in 0x00426630 - `push 0; call 0x00416860` leaves the 0 on the stack and the link at 0x0042733B reads it as its third argument. It is a NULL handler retail really registers, not an untranscribed address, and the port must keep it as a null pointer.
// STRUCTURAL ABSENCE, measured, not an omission: NO FRAME (no `sub esp` at any point; the stack is cleaned incrementally at seven sites) and NO parameter container (never calls 0x004F1950 or 0x004F1A80). Its only script-facing act is the single 0x004B8810 name registration.
// Status/limits: TRANSCRIBED; LINKABLE. Of the nine handler-link addresses, seven are raw code immediates with no implementation anywhere in the tree - 0x00426740, 0x004273B0, 0x00426860, 0x004274E0, 0x0042D9B0, 0x004277C0, 0x00426D30 - so they are passed as cast integers and create no link dependency; they stay recorded gaps, not stubs. 0x0051BAA0 IS transcribed and is referenced as a symbol, so it is a real link edge. STRUCTURALLY_VERIFIED for the CFG only, which is one block with no edges. BUILD_VERIFIED (i686-w64-mingw32-g++ -std=c++20 -O2 -Wall -Wextra single-file compile, zero warning lines). Runtime UNOBSERVED in both binaries.
// ABI NOTE, carried: `Script_SetConstant_51BAA0` is `std::uint32_t (void* const, std::uint32_t* const)` and reads its SECOND argument, so it is not a `FrameHandler`. The symbol reference is kept by casting through `void (*)()` first, which compiles with zero warnings and preserves the link edge; the dispatcher's argument count remains UNRESOLVED (see the Lead's ruling).
// Ownership note (per the Lead's two-variable trap): 0x0053BB18 is owned by this file and emitted with C++ linkage inside `namespace igi::retail::core`. No other file in the tree references it, and the image value is the sentinel word 0x0181, not zero.
// Domain: core/, per the Lead's ruling that these files are task-type registrars of the guard 0x00402930 family. This file is one of the 24 registrars the guard calls in descending address order at 0x00402B79-0x00402C32; its teardown is 0x00427810.
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

// 0x0053BB18: the HeliDoor class id. The retail image holds the word 0x0181 here, the "not registered"
// sentinel, NOT zero.
std::uint16_t g_53bb18 = 0x0181U; // 0x0053BB18

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
extern std::uint32_t Script_SetConstant_51BAA0(void* const arg0, std::uint32_t* const value); // 0x0051BAA0
extern void Script_QHashNodeRegister(void* arg1, void* value);    // 0x004B8810 (declared in native_402ff0.cpp)

void NATIVE_DECL HeliDoor_Register_4272A0()
{
    NATIVE_LOG_ENTER("HeliDoor_Register_4272A0", "0x004272A0");
    // 0x004272A0/0x004272A2: the two zeros are surplus for the argument-less parent getter and supply
    // 0x00401900's fourth argument. The parent getter is 0x004D9FA0, not 0x004018F0 and not 0x00424850.
    const std::uint16_t parent = Sys_GetGlobal16_4D9FA0();                    // 0x004272A4
    const std::uint16_t class_id =
        TaskType_Register("HeliDoor", 0x148U, parent, 0U);                    // 0x004272B4
    igi::retail::core::g_53bb18 = class_id;                                   // 0x004272C1
    QTask_LinkQTaskEventHandler(
        class_id, 0U, reinterpret_cast<igi::retail::core::FrameHandler>(0x00426740)); // 0x004272C7
    QTask_LinkQTaskEventHandlerNoContextFn(
        class_id, 1U, reinterpret_cast<igi::retail::core::FrameHandler>(0x004273B0)); // 0x004272DA
    QTask_LinkQTaskEventHandlerNoContextFn(
        class_id, 2U, reinterpret_cast<igi::retail::core::FrameHandler>(0x00426860)); // 0x004272EE
    // The handler immediate is pre-pushed before each argument-less getter and is consumed by the link.
    QTask_LinkQTaskEventHandler(
        class_id, Sys_GetGlobal8_4D9FB0(),
        reinterpret_cast<igi::retail::core::FrameHandler>(0x004274E0));       // 0x00427309
    QTask_LinkQTaskEventHandler(
        class_id, Sys_GetSettingByte_416840(),
        reinterpret_cast<igi::retail::core::FrameHandler>(0x0042D9B0));       // 0x00427323
    // 0x0042732B: the pre-pushed value is the literal 0, so this link registers a NULL handler.
    QTask_LinkQTaskEventHandler(
        class_id, Sys_GetSettingByte_416860(),
        static_cast<igi::retail::core::FrameHandler>(nullptr));               // 0x0042733B
    QTask_LinkQTaskEventHandler(
        class_id, DynObj_GetByte_4C56D0(),
        reinterpret_cast<igi::retail::core::FrameHandler>(0x004277C0));       // 0x00427356
    QTask_LinkQTaskEventHandler(
        class_id, DynObj_GetByte_4C56E0(),
        reinterpret_cast<igi::retail::core::FrameHandler>(
            reinterpret_cast<void (*)()>(&Script_SetConstant_51BAA0)));       // 0x00427370
    QTask_LinkQTaskEventHandler(
        class_id, Sys_GetGlobal8_489610(),
        reinterpret_cast<igi::retail::core::FrameHandler>(0x00426D30));       // 0x0042738B
    // 0x00427390: `mov edx,[0x53BB18]; and edx,0xFFFF` then push the id, then push the name at 0x0053BB1C.
    Script_QHashNodeRegister(const_cast<char*>("TASKTYPE_HELIDOOR"),
                             reinterpret_cast<void*>(static_cast<std::uintptr_t>(class_id))); // 0x004273A2
}

} // extern "C"
