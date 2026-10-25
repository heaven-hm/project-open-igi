// Native: Task_ClearClassTypeAndDeleteList_4192B0 (behavior-derived placeholder name; retail symbol unknown; the same behavior already carries this name in this tree at core/native_4497d0.cpp, so the form is reused deliberately and the VA suffix disambiguates). retail VA 0x004192B0-0x004192D4 (37 bytes, 1 block, 0 edges, 9 instructions).
// Artifact: retail igi.exe SHA-256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; image base 0x00400000.
// Tools/evidence: radare2 6.2.0 af/afij at 0x004192B0 (37 bytes, 1 block, 0 edges, 9 instructions) plus NASM ndisasm over the exact extent; both agree on 9 instructions, which matters here because ndisasm prints the 8-byte `mov word [mem],imm16` as two lines and a line-counting measurement reports 10. Exact bytes a1 f4 ba 57 00 50 e8 75 85 0a 00 66 8b 0d dc 9a 53 00 51 e8 58 87 fe ff 83 c4 08 66 c7 05 dc 9a 53 00 81 01 c3 = `mov eax,[0x0057BAF4]` / `push %eax` / `call 0x004C1830` / `mov cx,[0x00539ADC]` / `push %ecx` / `call 0x00401A20` / `add $0x8,%esp` / `mov word [0x00539ADC],0x0181` / `ret`. Both call displacements resolve as v+5+rel32: 0x004192B6 + 5 + 0x000A8575 = 0x004C1830 and 0x004192C3 + 5 + 0xFFFE8758 = 0x00401A20. The ret is at 0x004192D4, the bytes 0x004192D5-0x004192D8 are four 0x90 alignment nops (not part of the routine), and 0x004192D9 is the next function. Caller: this body is reached from the startup guard 0x00402930's direct call set (it is one of the 23 distinct guard targets inside 0x00419000-0x0041FFFF), and the guard's call site is inside 0x00402930's extent.
// Behavior/CFG: one straight-line block, no branch, no frame and no saved register. It is the teardown counterpart of the MenuLoader registrar core/native_4190e0.cpp: it deletes the task-parameter container whose handle the registrar stored at 0x0057BAF4 through 0x004C1830 (QTaskList_Delete, this tree's core/native_4c1830.cpp), then clears the registered class type whose 16-bit id the registrar stored at 0x00539ADC through 0x00401A20 (TaskType_Clear), and finally writes the 0x0181 "not registered" sentinel back into the id slot at 0x00539ADC. The `add $0x8,%esp` at 0x004192C8 is the caller-side cleanup for the two cdecl arguments this body pushed - the container push at 0x004192B5 (0x004C1830 cleans only its own internal push, so the outer push is still live at the call) and the id push at 0x004192C2 - and is emitted by the compiler for the two calls below rather than written by hand.
// Status/limits: TRANSCRIBED; LINKABLE (two undefined symbols, 0x004C1830 and 0x00401A20, both already tree natives, so this file adds no new dependency); STRUCTURALLY_VERIFIED for the CFG only (1 block, 0 edges, no branch structure to disagree about); BUILD_VERIFIED (i686-w64-mingw32-g++ -std=c++20 -Wall -Wextra single-file compile clean, host clang++ -std=c++20 single-file compile clean); runtime UNOBSERVED. PAIRING EVIDENCE, NOT AN ASSUMPTION: the two slots this body touches are the two slots core/native_4190e0.cpp writes - it defines igi::retail::core::g_539adc (0x00539ADC, uint16_t, image value 0x0181) and igi::retail::core::g_57baf4 (0x0057BAF4, uint32_t, past the raw extent of .data, image value 0) - and its provenance records the same two addresses. This file only reads and rewrites them; it declares them extern and defines nothing, so there is exactly one definition per address. The upper 16 bits of ECX at 0x004192BB are not zeroed by the 16-bit `mov cx` in retail, but the callee masks its argument to the low word (`and eax,0xffff` at 0x00401A25, recorded in core/native_401a20.cpp), so the port's zero-extended uint16_t cannot change behaviour. THE `mov word [..],0x0181` IS LOAD-BEARING: without it the class would stay marked registered after its container was deleted, and no build or link check can catch that. Domain: core/, because it is the teardown half of the guard 0x00402930 registrar family.
// Build evidence: partial link `i686-w64-mingw32-ld -r native_4192b0.o libdeps_4192b0.a` exit 0, and all 4 symbols this file references are defined exactly once in the merged object: _QTaskList_Delete_4C1830 by core/native_4c1830.cpp, _TaskType_Clear_401A20 by core/native_401a20.cpp, and the two namespace globals __ZN3igi6retail4core8g_539adcE and __ZN3igi6retail4core8g_57baf4E by the MenuLoader registrar core/native_4190e0.cpp. The archive carries four objects (native_4c1830.o, native_401a20.o, native_4190e0.o, native_4b0b70.o - the last for 0x00401A20's own QFile_ReleaseBlock dependency); archive /tmp/q218/link/libdeps_4192b0.a size 4046 bytes mtime 2026-10-25T15:37:08+05:30; object sha256 c5708a344fb790492f5716998da433bf51c07b94a4c1de0ae171c602ec10459b; source sha256 66b78c5f11ff0f2f256974d3be5f2d2b4669666619283f0ad2b5ac255cbfc8f5, which is this file with this line removed (`sed '/^\/\/ Build evidence:/d' native_4192b0.cpp | shasum -a 256`). Binary: build-win32-no-verify-gate/igi1.exe sha256 51fd67ab9ac3fea0cac27941545c2ddbb055155cd5a9a1b4d172fbd000de446d, size 14474387 bytes, mtime 2026-10-25T15:34:34, which PREDATES this file: `i686-w64-mingw32-nm build-win32-no-verify-gate/igi1.exe | grep -c ' [Tt] _Task_ClearClassTypeAndDeleteList_4192B0$'` = 0, because this file is not yet in the build target's source list. That zero is expected and is not a defect in the file; it must be re-measured to 1 after the source-list addition, and this record updated then. `python3 scripts/check_native_linkable.py engine/src/retail/core/native_4192b0.cpp` = `linkable (4 undefined, all defined in tree)`.
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

// 0x00539ADC and 0x0057BAF4 are DEFINED by the MenuLoader registrar engine/src/retail/core/native_4190e0.cpp
// (namespace-scope globals, no header declaration exists). This teardown only reads and rewrites them, so they are
// declared extern here and never defined, keeping one definition per retail address.
extern std::uint16_t g_539adc; // 0x00539ADC: the MenuLoader class id (image value 0x0181)
extern std::uint32_t g_57baf4; // 0x0057BAF4: the MenuLoader task-parameter container handle (image value 0)

} // namespace igi::retail::core

extern "C" {

// 0x004C1830: deletes a task list (engine/src/retail/core/native_4c1830.cpp); takes its one argument cdecl-style and
// cleans up only its own internal push, which is why the caller's push at 0x004192B5 survives to the `add esp,8`.
extern void QTaskList_Delete_4C1830(std::uint32_t list);

// 0x00401A20: clears the type slot for a class id (engine/src/retail/core/native_401a20.cpp); masks its argument to the
// low 16 bits.
extern void NATIVE_DECL TaskType_Clear_401A20(std::uint16_t class_id);

void NATIVE_DECL Task_ClearClassTypeAndDeleteList_4192B0()
{
    // 0x004192B0: `mov eax,[0x0057BAF4]`; 0x004192B5: `push %eax`; 0x004192B6: `call 0x004C1830`.
    QTaskList_Delete_4C1830(igi::retail::core::g_57baf4);

    // 0x004192BB: `mov cx,[0x00539ADC]`; 0x004192C2: `push %ecx`; 0x004192C3: `call 0x00401A20`.
    TaskType_Clear_401A20(igi::retail::core::g_539adc);

    // 0x004192CB: `mov word [0x00539ADC],0x0181` - restore the "not registered" sentinel. The 0x004192C8 `add $0x8,%esp`
    // between the call and this store is the cdecl cleanup for the two pushed arguments and is emitted by the compiler.
    igi::retail::core::g_539adc = 0x0181U;
}

} // extern "C"
