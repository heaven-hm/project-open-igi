// Native: GFXCapsViewer_Register_519710 (address-suffixed placeholder: this VA has no manifest record, and the name states only what the body establishes - it registers the task type "GFXCapsViewer"); retail VA 0x00519710-0x00519748.
// Artifact: retail igi.exe SHA-256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; image base 0x00400000; .text VA 0x00401000 raw 0x1000.
// Tools/evidence: capstone 5.0.7 decode of the 16-instruction extent: 1 block, 0 edges, 57 bytes, ret at 0x00519748, 7 `90` nops after (the next function starts at 0x0051974F). ENTRY BOUNDARY: the image-wide 0xE8/0xE9 enumeration finds exactly ONE reference to this VA, the tail jump `jmp 0x00519710` at 0x0051853D at the end of AIGraph_InitChain_5183D0 (core/native_5183d0.cpp), which is why it has no 0xE8 caller of its own. Exact bytes: 6A 00 6A 00 68 81 01 00 00 6A 20 68 2C ED 54 00 E8 DB 81 EE FF 83 C4 14 66 A3 28 20 BA 00 68 F0 96 51 00 E8 B8 7A FD FF 50 66 A1 28 20 BA 00 50 E8 BB 7C EE FF 83 C4 0C C3. ARGUMENT COUNTS FROM THE CLEANUPS, NOT FROM THE PUSHES: `add esp,0x14` is five dwords for 0x00401900, and the single `add esp,0xC` at 0x00519745 covers the three dwords of the 0x00401400 call - the `push 0x005196F0` at 0x0051972E belongs to that call and not to 0x004F11F0, which was re-read at its own VA as `mov al,[0x54DDA4]; ret`: two instructions, argument-less, ignoring anything the caller pushes. That is the same body shape as the tree's Sys_GetGlobal8_489610, so it is declared with the tree's naming convention for that shape. This call site is the same idiom as core/native_464260.cpp (register a task type, store the returned word, link a handler to an event id through 0x00401400), which is why the declarations and the handler cast below match that file.
// Behavior/CFG: one straight-line block, no branch, no frame, no local state. Registers the task type "GFXCapsViewer" (string byte-exact at 0x0054ED2C) with object size 0x20 and parent type 0x181, stores the returned 16-bit id in the word at 0x00BA2028, reads the event-id byte at 0x0054DDA4, and links that event to the raw code address 0x005196F0 through QTask_LinkQTaskEventHandler, leaving that call's result in EAX at the ret. The parent 0x181 and the 0x0054DDA4 event id are both already established elsewhere in this tree (0x004EE970 registers the base classes and allocates 0x0054DDA4's event id), so this is one more member of the 0x0181 parent cluster.
// Status/limits: TRANSCRIBED; NOT LINKABLE - undefined in tree: Sys_GetGlobal8_4F11F0 (0x004F11F0, no manifest record, no implementation; two instructions, measured above). The callback 0x005196F0 is passed as a raw code immediate, following core/native_464260.cpp's treatment of raw handler addresses; its own body (7 instructions, 2 calls, dispatching through the table at 0x00A94E84) is a separate native and is not transcribed here. g_ba2028 is defined in this file; g_54dda4 is declared exactly as script/native_4ee970.cpp declares it and defined in script/script_state_globals_4ee970.cpp. STRUCTURALLY_VERIFIED against the 16-instruction listing (1 block, 0 edges, 57 bytes, extent bounded by the ret); BUILD_VERIFIED (i686-w64-mingw32-g++ -std=c++20 -O2 -Wall -Wextra single-file compile); runtime UNOBSERVED. Domain: core/.

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

// 0x0054DDA4: the event id allocated for the script base classes by Script_InitTypeCatalogue (0x004EE970). Declared
// exactly as script/native_4ee970.cpp declares it, so the two files agree.
extern std::uint8_t g_54dda4;

// NATIVE GAP: 0x004F11F0 has no manifest record and no implementation under engine/. Measured body:
// `mov al,[0x54DDA4]; ret` - two instructions, argument-less, returning the event-id byte above.
extern "C" std::uint8_t Sys_GetGlobal8_4F11F0(void);

extern "C" {

// 0x00BA2028: the "GFXCapsViewer" task type id. Image value 0, overwritten by the 16-bit store at 0x00519728.
std::uint16_t g_ba2028 = 0U;

std::uint32_t NATIVE_DECL GFXCapsViewer_Register_519710(void)
{
    const std::uint16_t caps_viewer_id = TaskType_Register("GFXCapsViewer", // 0x0051971B: push 0x0054ED2C
                                                           0x20U,            // 0x00519719: push 0x20 (object size)
                                                           0x181U,           // 0x00519714: push 0x181 (parent type)
                                                           0U); // 0x00519712: push 0 (task-list argument); the 0x00519710 push is the fifth dword, which the callee ignores

    // 0x00519728: mov word [0xBA2028],ax (the id store; 0x00519725 is the add esp,0x14 that cleans the five pushes above).
    ::g_ba2028 = caps_viewer_id;

    // 0x0051972E: push 0x005196F0 (the handler, pre-pushed before the argument-less getter below);
    // 0x00519733: call 0x4F11F0; 0x00519738: push eax (the event id); 0x00519739: mov ax,[0xBA2028]; 0x0051973F: push eax (the id).
    return QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_ba2028), // 0x0051973F: push eax
                                       Sys_GetGlobal8_4F11F0(),                // 0x00519738: push eax (event id)
                                       reinterpret_cast<igi::retail::core::FrameHandler>(
                                           static_cast<std::uintptr_t>(0x005196F0U))); // 0x0051972E: push 0x005196F0
}

} // extern "C"

#undef NATIVE_DECL
