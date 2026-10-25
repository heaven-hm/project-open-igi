// Native: SimBoneDynCubeObj_Register_52CA30 (address-suffixed behaviour name: this VA has no manifest record, and the name states what the body registers); retail VA 0x0052CA30-0x0052CA52.
// Artifact: retail igi.exe SHA-256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; image base 0x00400000; .text VA 0x00401000 raw 0x1000.
// Tools/evidence: capstone 5.0.7 decode of the 10-instruction extent: 1 block, 0 edges, 35 bytes, ret at 0x0052CA52, 13 `90` nops after (the next function starts at 0x0052CA60). ENTRY BOUNDARY: the image-wide 0xE8/0xE9 enumeration finds exactly ONE reference to this VA, the call at 0x00518402 inside AIGraph_InitChain_5183D0 (core/native_5183d0.cpp). Exact bytes: 6A 00 6A 00 E8 D7 CB FA FF 50 68 40 01 00 00 68 04 F7 54 00 E8 B7 4E ED FF 83 C4 14 66 A3 00 F7 54 00 C3. ARGUMENT COUNT FROM THE CLEANUP: `add esp,0x14` at 0x0052CA49 is five dwords for 0x00401900 - the same 5-push pattern the tree documents for TaskType_Register, whose callee reads four (task_types.hpp). IMAGE VALUES READ FROM THE FILE, NOT ASSUMED: the word at 0x0054F700 holds 385 = 0x0181 before this body runs, and the name literal at 0x0054F704 is the bytes `SimBoneDynCubeObj\0`.
// Behavior/CFG: one straight-line block, no branch, no frame, no local state. Registers the task type "SimBoneDynCubeObj" (string byte-exact at 0x0054F704) with object size 0x140 and the parent returned by 0x004D9610, then stores the returned 16-bit id in the word at 0x0054F700, which the image pre-fills with 0x0181. This is the same idiom as core/native_464260.cpp and core/native_519710.cpp (register a type, store the returned word). The parent 0x0181 is the same cluster member those two files record.
// NAME COORDINATION (not mine to resolve): 0x004D9610 is `mov ax,[0x548618]; ret`, and the manifest record for that VA names it BoneDynCubeObj_GetQTaskType while core/native_464260.cpp declares it as Sys_GetGlobal16_4D9610. This file uses the manifest name, because the manifest is the naming authority for a VA that has a record and because the parent of "SimBoneDynCubeObj" is BoneDynCubeObj by construction. The manifest owner must pick one name for 0x004D9610 and update the other file; until then the two callers declare the same VA under different symbols and only one of them can resolve.
// Status/limits: TRANSCRIBED; NOT LINKABLE - undefined in tree: BoneDynCubeObj_GetQTaskType (0x004D9610, no file, no implementation; body measured above). g_54f700 is defined in this file with the image value 0x0181, because nothing else in the tree defines 0x0054F700 (checked). STRUCTURALLY_VERIFIED against the 10-instruction listing (1 block, 0 edges, 35 bytes, extent bounded by the ret); BUILD_VERIFIED (i686-w64-mingw32-g++ -std=c++20 -O2 -Wall -Wextra single-file compile); runtime UNOBSERVED. Domain: core/ (a startup registrar reached from the AI-graph init chain, like core/native_464260.cpp).

#include "igi/retail/core/task_types.hpp"

#include <cstdint>

// task_types.hpp defines NATIVE_DECL for its own declarations and #undefs it again, so the macro is re-established
// here for this file's own definition (the same guarded block the other retail transcriptions carry).
#ifndef NATIVE_DECL
#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif
#endif

// NATIVE GAP: 0x004D9610 has no file and no implementation under engine/. Measured body: `mov ax,[0x548618]; ret` -
// two instructions, argument-less, returning the word at 0x00548618 (image value 385 = 0x0181), the BoneDynCubeObj
// task type id, used here as the parent type. Declared under the manifest's name for this VA.
extern "C" std::uint16_t BoneDynCubeObj_GetQTaskType(void);

extern "C" {

// 0x0054F700: the "SimBoneDynCubeObj" task type id. Image value 385 = 0x0181, overwritten by the 16-bit store at 0x0052CA4C.
std::uint16_t g_54f700 = 0x0181U;

void NATIVE_DECL SimBoneDynCubeObj_Register_52CA30(void)
{
    const std::uint16_t sim_bone_dyn_cube_obj_id = TaskType_Register("SimBoneDynCubeObj", // 0x0052CA3F: push 0x0054F704
                                                                     0x140U,               // 0x0052CA3A: push 0x140 (object size)
                                                                     BoneDynCubeObj_GetQTaskType(), // 0x0052CA34: call 0x004D9610 (parent type)
                                                                     0U); // 0x0052CA32: push 0 (task-list argument); the 0x0052CA30 push is the fifth dword, which the callee ignores

    // 0x0052CA49: add esp,0x14 (cleans the five pushes above); 0x0052CA4C: mov word [0x54F700],ax.
    ::g_54f700 = sim_bone_dyn_cube_obj_id;
}

} // extern "C"

#undef NATIVE_DECL
