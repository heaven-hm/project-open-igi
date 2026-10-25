// Native: CarDoor_Register_426630 (placeholder name; retail symbol unknown; the class-name string it registers is "CarDoor" at 0x0053BAD4). retail VA 0x00426630-0x0042673A (267 bytes, 1 block, 0 edges, 71 instructions).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 `pd` over the exact image plus capstone 5.0.7 aligned from the verified entry 0x00426630; the two listings agree. Extent: entry begins directly with `push 0; push 0; call 0x004D9FA0` - there is NO `sub esp` prologue - and `c3` sits at 0x0042673A, with 0x90 from 0x0042673B onward, so the extent is 267 bytes. Push accounting for a FRAMELESS body, replayed with esp simulated and reconciling with zero slack: `add esp,0x38` at 0x00426683 = 2 (the two surplus zero words) + 3 (0x00401900 with its pre-pushed fourth argument) + 3 + 3 + 3 (links 1-3); `add esp,0xC` at each of 0x0042669E, 0x004266B8, 0x004266D0, 0x004266EB and 0x00426705 = 3 dwords each, being the handler immediate that was PRE-PUSHED before an argument-less getter, plus that getter's return, plus the class id; `add esp,0x14` at 0x00426737 = 5 dwords = the last handler immediate + its getter's return + the id + the id again + the 0x0053BAC0 name for 0x004B8810. Cleaned in total 34 dwords, pushed in total 34. Call census from the 0xE8 enumeration: 0x004D9FA0 x1, 0x00401900 x1, 0x00401400 x7, 0x00401530 x2, 0x004D9FB0 x1, 0x00416840 x1, 0x00416860 x1, 0x004C56D0 x1, 0x004C56E0 x1, 0x00489610 x1, 0x004B8810 x1; 18 call instructions, no branch of any kind.
// Behavior/CFG: one straight-line block, no branch, no loop. The parent getter is 0x004D9FA0 (`mov ax,[0x00548648]; ret`) - a THIRD parent getter, distinct from both 0x004018F0 and 0x00424850, so the band does not have two parent sources but three. It registers "CarDoor" with a 0x148-byte object through 0x00401900 and stores the 16-bit id at 0x0053BABC (image word 0x0181, the not-registered sentinel). Then nine handler links on that id, in retail order: 0x00401400 event 0 -> 0x00426740; 0x00401530 event 1 -> 0x004273B0; 0x00401530 event 2 -> 0x00426860; 0x00401400 the byte from 0x004D9FB0 -> 0x00426880; 0x00401400 the byte from 0x00416840 -> 0x0042D9B0; 0x00401400 the byte from 0x00416860 -> handler literal 0; 0x00401400 the byte from 0x004C56D0 -> 0x00426CD0; 0x00401400 the byte from 0x004C56E0 -> 0x0051BAA0; 0x00401400 the byte from 0x00489610 -> 0x00426D30. Every one of those six getters is a single-instruction argument-less reader (`mov al,[abs]; ret`), so the handler immediate pushed before it is NOT its argument - it survives on the stack and is consumed by the link as its third argument. Finally it registers the byte-identical name "TASKTYPE_CARDOOR" (0x0053BAC0) against the id through 0x004B8810, masking the id to 16 bits first (`and edx,0xFFFF`).
// MEASURED NULL HANDLER: link 6 passes the literal immediate 0 as the handler - `push 0; call 0x00416860` leaves the 0 on the stack, and the link at 0x004266CB reads it as its third argument. The zero is both the getter's pre-pushed value and the handler, exactly as the accounting requires (that link's cleanup is 3 dwords, not 2). This is a NULL handler retail really registers, not an untranscribed address, and the port must keep it as a null pointer.
// STRUCTURAL ABSENCE, measured, not an omission: this routine has NO FRAME (no `sub esp` at any point; the stack is cleaned incrementally at seven sites) and NO parameter container (it never calls 0x004F1950 or 0x004F1A80). Its only script-facing act is the single 0x004B8810 name registration. A later reader must not "restore" a frame or a container here.
// Status/limits: TRANSCRIBED; LINKABLE. Of the nine handler-link addresses, seven are raw code immediates with no implementation anywhere in the tree - 0x00426740, 0x004273B0, 0x00426860, 0x00426880, 0x0042D9B0, 0x00426CD0, 0x00426D30 - so they are passed as cast integers and create no link dependency; they stay recorded gaps, not stubs. 0x0051BAA0 IS transcribed (`Script_SetConstant_51BAA0`, engine/src/retail/script/native_51baa0.cpp) and is referenced as a symbol, so it is a real link edge. STRUCTURALLY_VERIFIED for the CFG only, which is one block with no edges. BUILD_VERIFIED (i686-w64-mingw32-g++ -std=c++20 -O2 -Wall -Wextra single-file compile clean). Runtime UNOBSERVED in both binaries.
// ABI NOTE (the limit the Lead recorded): `Script_SetConstant_51BAA0` is declared `std::uint32_t (void* const, std::uint32_t* const)` in its own file, which is NOT `FrameHandler` = `void(*)(void*)` - its body reads its SECOND argument (`mov eax,[esp+8]`), so a frame handler called with one argument would have it dereference whatever sits below the return address. A single-step `reinterpret_cast` to `FrameHandler` therefore produced a real `-Wcast-function-type` warning on the first compile of this file; the warning is the evidence, not noise. The symbol reference is kept by casting through `void (*)()` first, which GCC treats as matching everything, so the link edge survives with zero warnings, and the ABI disagreement is recorded here instead of being hidden by a wrapper (rule 2 forbids a wrapper). Whether retail's dispatcher passes one argument or two is a property of 0x00401400 and the event walker, not of this routine, and it is UNRESOLVED: `FrameHandler` may be an under-specified typedef for this link.
// Ownership note (per the Lead's two-variable trap): 0x0053BABC is owned by this file and emitted with C++ linkage inside `namespace igi::retail::core`. No other file in the tree references it, and the image value is the sentinel word 0x0181, not zero. A future `extern "C"` definition of the same address would create a second, silent copy.
// Domain: core/, per the Lead's ruling that these files are task-type registrars of the guard 0x00402930 family and belong in one folder whatever subsystem the class they register serves. This file is one of the 24 registrars the guard calls in descending address order at 0x00402B79-0x00402C32; its teardown is 0x00426D60.
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

// 0x0053BABC: the CarDoor class id. The retail image holds the word 0x0181 here, the "not registered"
// sentinel, NOT zero.
std::uint16_t g_53babc = 0x0181U; // 0x0053BABC

} // namespace igi::retail::core

extern "C" {

// All six of these are single-instruction argument-less readers; the names and signatures match the
// declarations already used elsewhere in the tree (native_42d2b0.cpp and others) so the symbols resolve.
extern std::uint16_t NATIVE_DECL Sys_GetGlobal16_4D9FA0(void);    // 0x004D9FA0: `mov ax,[0x548648]; ret` (parent type)
extern std::uint8_t NATIVE_DECL Sys_GetGlobal8_4D9FB0(void);      // 0x004D9FB0: `mov al,[0x54864A]; ret`
extern std::uint8_t NATIVE_DECL Sys_GetSettingByte_416840(void);  // 0x00416840: `mov al,[0x539549]; ret`
extern std::uint8_t NATIVE_DECL Sys_GetSettingByte_416860(void);  // 0x00416860: `mov al,[0x53954B]; ret`
extern std::uint8_t NATIVE_DECL DynObj_GetByte_4C56D0(void);      // 0x004C56D0: `mov al,[0x547F3C]; ret`
extern std::uint8_t NATIVE_DECL DynObj_GetByte_4C56E0(void);      // 0x004C56E0: `mov al,[0x547F3D]; ret`
extern std::uint8_t NATIVE_DECL Sys_GetGlobal8_489610(void);      // 0x00489610: `mov al,[0x54127C]; ret`
extern std::uint32_t Script_SetConstant_51BAA0(void* const arg0, std::uint32_t* const value); // 0x0051BAA0
extern void Script_QHashNodeRegister(void* arg1, void* value);    // 0x004B8810 (declared in native_402ff0.cpp)

void NATIVE_DECL CarDoor_Register_426630()
{
    NATIVE_LOG_ENTER("CarDoor_Register_426630", "0x00426630");
    // 0x00426630/0x00426632: the two zeros are surplus for the argument-less parent getter and supply
    // 0x00401900's fourth argument. The parent getter is 0x004D9FA0, not 0x004018F0 and not 0x00424850.
    const std::uint16_t parent = Sys_GetGlobal16_4D9FA0();                    // 0x00426634
    const std::uint16_t class_id =
        TaskType_Register("CarDoor", 0x148U, parent, 0U);                     // 0x00426644
    igi::retail::core::g_53babc = class_id;                                   // 0x00426651
    QTask_LinkQTaskEventHandler(
        class_id, 0U, reinterpret_cast<igi::retail::core::FrameHandler>(0x00426740)); // 0x00426657
    QTask_LinkQTaskEventHandlerNoContextFn(
        class_id, 1U, reinterpret_cast<igi::retail::core::FrameHandler>(0x004273B0)); // 0x0042666A
    QTask_LinkQTaskEventHandlerNoContextFn(
        class_id, 2U, reinterpret_cast<igi::retail::core::FrameHandler>(0x00426860)); // 0x0042667E
    // From here on the handler immediate is pushed BEFORE the argument-less getter and is consumed by the
    // link as its third argument - the same pre-push pattern the Lead recorded for 0x004223D0.
    QTask_LinkQTaskEventHandler(
        class_id, Sys_GetGlobal8_4D9FB0(),
        reinterpret_cast<igi::retail::core::FrameHandler>(0x00426880));       // 0x00426699
    QTask_LinkQTaskEventHandler(
        class_id, Sys_GetSettingByte_416840(),
        reinterpret_cast<igi::retail::core::FrameHandler>(0x0042D9B0));       // 0x004266B3
    // 0x004266BB: the pre-pushed value is the literal 0, so this link registers a NULL handler.
    QTask_LinkQTaskEventHandler(
        class_id, Sys_GetSettingByte_416860(),
        static_cast<igi::retail::core::FrameHandler>(nullptr));               // 0x004266CB
    QTask_LinkQTaskEventHandler(
        class_id, DynObj_GetByte_4C56D0(),
        reinterpret_cast<igi::retail::core::FrameHandler>(0x00426CD0));       // 0x004266E6
    // 0x00426700: the handler is the transcribed 0x0051BAA0. Its own signature is
    // `std::uint32_t (void* const, std::uint32_t* const)`, so it cannot be cast to FrameHandler in one
    // step without asserting a call contract the type cannot express; the two-step cast through
    // `void (*)()` keeps the symbol reference (a real link edge) and compiles with zero warnings.
    QTask_LinkQTaskEventHandler(
        class_id, DynObj_GetByte_4C56E0(),
        reinterpret_cast<igi::retail::core::FrameHandler>(
            reinterpret_cast<void (*)()>(&Script_SetConstant_51BAA0)));       // 0x00426700
    QTask_LinkQTaskEventHandler(
        class_id, Sys_GetGlobal8_489610(),
        reinterpret_cast<igi::retail::core::FrameHandler>(0x00426D30));       // 0x0042671B
    // 0x00426720: `mov edx,[0x53BABC]; and edx,0xFFFF` then push the id, then push the name at 0x0053BAC0.
    // 0x004B8810 takes (arg1 = name, value = id); the name is the byte-identical literal "TASKTYPE_CARDOOR".
    Script_QHashNodeRegister(const_cast<char*>("TASKTYPE_CARDOOR"),
                             reinterpret_cast<void*>(static_cast<std::uintptr_t>(class_id))); // 0x00426732
}

} // extern "C"
