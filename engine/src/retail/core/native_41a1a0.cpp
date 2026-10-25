// Native: Task_ClearClassTypeAndDeleteList_41A1A0 (behavior-derived placeholder name; retail symbol unknown; the same behavior already carries this name in this tree at core/native_4497d0.cpp). retail VA 0x0041A1A0-0x0041A1C4 (37 bytes, 1 block, 0 edges, 9 instructions).
// Artifact: retail igi.exe SHA-256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; image base 0x00400000.
// Tools/evidence: radare2 6.2.0 af/afij at 0x0041A1A0 (37 bytes, 1 block, 0 edges, 9 instructions) plus NASM ndisasm over the exact extent; both agree on 9 instructions, which matters here because ndisasm prints the 8-byte `mov word [mem],imm16` as two output lines and a line count reports 10. Exact bytes a1 88 bb 57 00 50 e8 85 76 0a 00 66 8b 0d 28 9c 53 00 51 e8 68 78 fe ff 83 c4 08 66 c7 05 28 9c 53 00 81 01 c3 = `mov eax,[0x0057BB88]` / `push %eax` / `call 0x004C1830` / `mov cx,[0x00539C28]` / `push %ecx` / `call 0x00401A20` / `add $0x8,%esp` / `mov word [0x00539C28],0x0181` / `ret`. Both call displacements resolve as v+5+rel32: 0x0041A1A6 + 5 + 0x000A7685 = 0x004C1830 and 0x0041A1B3 + 5 + 0xFFFE7868 = 0x00401A20. The ret is at 0x0041A1C4 and the bytes 0x0041A1C5-0x0041A1C8 are four 0x90 alignment nops (not part of the routine). Caller: this body is one of the 23 distinct targets of the startup guard 0x00402930 that lie inside 0x00419000-0x0041FFFF.
// Behavior/CFG: one straight-line block, no branch, no frame and no saved register. It is the teardown counterpart of the DialogWindow registrar core/native_4199f0.cpp: it deletes the task-parameter container whose handle the registrar stored at 0x0057BB88 through 0x004C1830 (QTaskList_Delete, core/native_4c1830.cpp), then clears the registered class type whose 16-bit id the registrar stored at 0x00539C28 through 0x00401A20 (TaskType_Clear), and finally writes the 0x0181 "not registered" sentinel back into the id slot. The `add $0x8,%esp` at 0x0041A1B8 is the caller-side cleanup for the two cdecl arguments this body pushed - the container push at 0x0041A1A5 (0x004C1830 cleans only its own internal push) and the id push at 0x0041A1B2 - and is emitted by the compiler for the two calls below rather than written by hand.
// Status/limits: TRANSCRIBED; LINKABLE (four undefined symbols, all already tree natives, so this file adds no new dependency); STRUCTURALLY_VERIFIED for the CFG only (1 block, 0 edges, no branch structure to disagree about); BUILD_VERIFIED (i686-w64-mingw32-g++ -std=c++20 -Wall -Wextra single-file compile clean, host clang++ -std=c++20 -Wall -Wextra compile clean); runtime UNOBSERVED. PAIRING EVIDENCE, NOT AN ASSUMPTION: the two slots this body touches are two of the three globals the DialogWindow registrar core/native_4199f0.cpp defines - g_539c28 (0x00539C28, uint16_t, image value 0x0181) and g_57bb88 (0x0057BB88, uint32_t, past the raw extent of .data, image value 0). That registrar also defines g_57bb80 as a two-element uint32_t array for its arg6 pre-push, so the container is taken from the registrar's own recorded container slot and not from whichever global happens to sit nearest in address order. This file declares both globals extern and defines nothing, so there is exactly one definition per address. LINKAGE, MEASURED NOT ASSUMED: the DialogWindow registrar defines both globals with C linkage (symbols `_g_539c28` and `_g_57bb88`, read from `i686-w64-mingw32-nm --defined-only` on its compiled object), which is the form used here; the MenuLoader registrar core/native_4190e0.cpp is the one owner in this band that instead uses `namespace igi::retail::core`, so the two teardowns that pair with those two owners need different declarations for what looks like the same situation. The upper 16 bits of ECX at 0x0041A1AB are not zeroed by the 16-bit `mov cx` in retail, but the callee masks its argument to the low word (`and eax,0xffff` at 0x00401A25, recorded in core/native_401a20.cpp), so the port's zero-extended std::uint16_t cannot change behaviour. THE `mov word [..],0x0181` IS LOAD-BEARING: without it the class would stay marked registered after its container was deleted, and no build or link check can catch that. Domain: core/, because it is the teardown half of the guard 0x00402930 registrar family.
// Build evidence: partial link `i686-w64-mingw32-ld -r native_41a1a0.o libdeps_41a1a0.a` exit 0, and all 4 symbols this file references are defined exactly once in the merged object: _QTaskList_Delete_4C1830 by core/native_4c1830.cpp, _TaskType_Clear_401A20 by core/native_401a20.cpp, and _g_539c28 and _g_57bb88 by the DialogWindow registrar core/native_4199f0.cpp. The archive carries four objects (native_4c1830.o, native_401a20.o, native_4b0b70.o for 0x00401A20's own QFile_ReleaseBlock dependency, and native_4199f0.o); archive /tmp/q218/link/libdeps_41a1a0.a size 4890 bytes mtime 2026-10-25T15:45:09+05:30; object sha256 603a75d20d51e4748597506039f93d5d4f500bb1038c81482e8b274be10bc9a4; source sha256 1e22abd3dd71041cd69828dbacab21172e8d08a7f8b615a8db62d787d83dac8e, which is this file with this line removed (`sed '/^\/\/ Build evidence:/d' native_41a1a0.cpp | shasum -a 256`). The globals are declared with C linkage because the owner's compiled object defines `_g_539c28` and `_g_57bb88` with C linkage; the linkage was read from `i686-w64-mingw32-nm --defined-only native_4199f0.o`, not inferred from the source text, after the sibling teardown 0x004199B0 failed the link check on exactly this point. Binary: build-win32-no-verify-gate/igi1.exe sha256 c568d249e5269eaf3aac3fe6f75d336c190dfd0e44fa931499d53f4b7c13c9e0, size 14479926 bytes, mtime 2026-10-25T15:45:04, which PREDATES this file: `i686-w64-mingw32-nm build-win32-no-verify-gate/igi1.exe | grep -c ' [Tt] _Task_ClearClassTypeAndDeleteList_41A1A0$'` = 0, because this file is not yet in the build target's source list. That zero is expected and is not a defect in the file; it must be re-measured to 1 after the source-list addition, and this record updated then. `python3 scripts/check_native_linkable.py engine/src/retail/core/native_41a1a0.cpp` = `linkable (4 undefined, all defined in tree)`.
#include "igi/retail/core/task_types.hpp"

#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// 0x00539C28 and 0x0057BB88 are DEFINED by the DialogWindow registrar engine/src/retail/core/native_4199f0.cpp, both
// with C linkage. This teardown only reads and rewrites them, so they are declared extern here and never defined,
// keeping one definition per retail address.
extern std::uint16_t g_539c28; // 0x00539C28: the DialogWindow class id (image value 0x0181)
extern std::uint32_t g_57bb88; // 0x0057BB88: the DialogWindow task-parameter container handle (image value 0)

// 0x004C1830: deletes a task list (engine/src/retail/core/native_4c1830.cpp); takes its one argument cdecl-style and
// cleans up only its own internal push, which is why the caller's push at 0x0041A1A5 survives to the `add esp,8`.
extern void QTaskList_Delete_4C1830(std::uint32_t list);

// 0x00401A20: clears the type slot for a class id (engine/src/retail/core/native_401a20.cpp); masks its argument to the
// low 16 bits.
extern void NATIVE_DECL TaskType_Clear_401A20(std::uint16_t class_id);

void NATIVE_DECL Task_ClearClassTypeAndDeleteList_41A1A0()
{
    // 0x0041A1A0: `mov eax,[0x0057BB88]`; 0x0041A1A5: `push %eax`; 0x0041A1A6: `call 0x004C1830`.
    QTaskList_Delete_4C1830(::g_57bb88);

    // 0x0041A1AB: `mov cx,[0x00539C28]`; 0x0041A1B2: `push %ecx`; 0x0041A1B3: `call 0x00401A20`.
    TaskType_Clear_401A20(::g_539c28);

    // 0x0041A1BB: `mov word [0x00539C28],0x0181` - restore the "not registered" sentinel. The 0x0041A1B8 `add $0x8,%esp`
    // between the call and this store is the cdecl cleanup for the two pushed arguments and is emitted by the compiler.
    ::g_539c28 = 0x0181U;
}

} // extern "C"
