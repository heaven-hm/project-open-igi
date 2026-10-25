// Native: Task_ClearClassType_4199B0 (behavior-derived placeholder name; retail symbol unknown; the name deliberately omits the "AndDeleteList" half of core/native_4497d0.cpp's name because this body has no container to delete). retail VA 0x004199B0-0x004199C8 (25 bytes, 1 block, 0 edges, 6 instructions).
// Artifact: retail igi.exe SHA-256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; image base 0x00400000.
// Tools/evidence: radare2 6.2.0 af/afij at 0x004199B0 (25 bytes, 1 block, 0 edges, 6 instructions) plus NASM ndisasm over the exact extent; both agree on 6 instructions, which matters here because ndisasm prints the 8-byte `mov word [mem],imm16` as two output lines and a line count reports 7. Exact bytes 66 a1 f0 9a 53 00 50 e8 64 80 fe ff 83 c4 04 66 c7 05 f0 9a 53 00 81 01 c3 = `mov ax,[0x00539AF0]` / `push %eax` / `call 0x00401A20` / `add $0x4,%esp` / `mov word [0x00539AF0],0x0181` / `ret`. The call displacement resolves as v+5+rel32: 0x004199B7 + 5 + 0xFFFE8064 = 0x00401A20. The ret is at 0x004199C8 and the bytes 0x004199C9-0x004199CF are seven 0x90 alignment nops (not part of the routine). Caller: this body is one of the 23 distinct targets of the startup guard 0x00402930 that lie inside 0x00419000-0x0041FFFF.
// Behavior/CFG: one straight-line block, no branch, no frame and no saved register. It is the teardown counterpart of the BackgroundFX registrar core/native_419760.cpp: it clears the registered class type whose 16-bit id the registrar stored at 0x00539AF0, then writes the 0x0181 "not registered" sentinel back into that slot. THERE IS NO CONTAINER TO DELETE and consequently no call to 0x004C1830: the registrar allocates no task-parameter container, so this body has one call where its eleven siblings in the band have two. That asymmetry is not an omission - it is the only teardown of the twelve that does not appear in 0x004C1830's 164-site caller census, which is an independent confirmation from a list derived in the opposite direction. The `add $0x4,%esp` at 0x004199BC is the caller-side cleanup for the single cdecl argument pushed at 0x004199B6 and is emitted by the compiler for the call below.
// Status/limits: TRANSCRIBED; LINKABLE (two undefined symbols, 0x00401A20 and the id global, both already tree natives, so this file adds no new dependency); STRUCTURALLY_VERIFIED for the CFG only (1 block, 0 edges, no branch structure to disagree about); BUILD_VERIFIED (i686-w64-mingw32-g++ -std=c++20 -Wall -Wextra single-file compile clean, host clang++ -std=c++20 single-file compile clean); runtime UNOBSERVED. PAIRING EVIDENCE, NOT AN ASSUMPTION: 0x00539AF0 is defined by the BackgroundFX registrar core/native_419760.cpp, uint16_t, image value 0x0181, and that file records the same address as the slot it writes with the id returned by TaskType_Register("BackgroundFX", ...). This file declares it extern and defines nothing, so there is exactly one definition for the address. LINKAGE, MEASURED NOT ASSUMED: the tree does NOT use one linkage for these registrar id globals. native_419760.cpp defines g_539af0 inside its own `extern "C"` block, so its symbol is `_g_539af0`; core/native_4190e0.cpp defines g_539adc and g_57baf4 inside `namespace igi::retail::core`, so those symbols are `_ZN3igi6retail4core8g_539adcE` and `_ZN3igi6retail4core8g_57baf4E`. A namespaced declaration here compiled clean and was caught only by the link check, which reported `NOT LINKABLE - undefined in tree: ZN3igi6retail4core8g_539af0E`; the declaration below is therefore C-linkage to match its owner. The two globals of the teardown at 0x004192B0 and the one here are declared in different linkages for that reason, not by inconsistency. The upper 16 bits of EAX at 0x004199B6 are not zeroed by the 16-bit `mov ax` in retail, but the callee masks its argument to the low word (`and eax,0xffff` at 0x00401A25, recorded in core/native_401a20.cpp), so the port's zero-extended std::uint16_t cannot change behaviour. THE `mov word [..],0x0181` IS LOAD-BEARING: without it the class would stay marked registered after its type slot was cleared, and no build or link check can catch that. Domain: core/, because it is the teardown half of the guard 0x00402930 registrar family.
// Build evidence: partial link `i686-w64-mingw32-ld -r native_4199b0.o libdeps_4199b0.a` exit 0, and both symbols this file references are defined exactly once in the merged object: _TaskType_Clear_401A20 by core/native_401a20.cpp and _g_539af0 by the BackgroundFX registrar core/native_419760.cpp. The archive carries three objects (native_401a20.o, native_4b0b70.o for 0x00401A20's own QFile_ReleaseBlock dependency, and native_419760.o); archive /tmp/q218/link/libdeps_4199b0.a size 3070 bytes mtime 2026-10-25T15:43:05+05:30; object sha256 d8796f4eaee3a7cf10fce93dc6fc1832cf79635f47d2d590d1a3f3e7ae794eac; source sha256 e9412e3df4d2990c216c717df5fdc56c8cf5050227a4f8a4ed4f9b05faaf4a37, which is this file with this line removed (`sed '/^\/\/ Build evidence:/d' native_4199b0.cpp | shasum -a 256`). THE FIRST VERSION OF THIS FILE FAILED THE LINK CHECK: it declared 0x00539AF0 as `igi::retail::core::g_539af0`, compiled clean, and `check_native_linkable.py` reported `NOT LINKABLE - undefined in tree: ZN3igi6retail4core8g_539af0E` because the owner defines it with C linkage inside its own `extern "C"` block. The declaration above was corrected to `extern std::uint16_t g_539af0;` and the check now passes; the failure is recorded rather than erased because a clean compile did not catch it and only the link check did. Binary: build-win32-no-verify-gate/igi1.exe sha256 c0c02b8073fc9db17901c00db981c3650bfc5a2dba2b6c3aa3ff5b8fd5b49d90, size 14474881 bytes, mtime 2026-10-25T15:43:09, which PREDATES this file: `i686-w64-mingw32-nm build-win32-no-verify-gate/igi1.exe | grep -c ' [Tt] _Task_ClearClassType_4199B0$'` = 0, because this file is not yet in the build target's source list. That zero is expected and is not a defect in the file; it must be re-measured to 1 after the source-list addition, and this record updated then. `python3 scripts/check_native_linkable.py engine/src/retail/core/native_4199b0.cpp` = `linkable (2 undefined, all defined in tree)`.
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

// 0x00539AF0: the BackgroundFX class id word. DEFINED by the registrar engine/src/retail/core/native_419760.cpp inside
// that file's own `extern "C"` block, so the symbol is `_g_539af0`, NOT a namespace-mangled name. This teardown only
// reads and rewrites it, so it is declared extern here and never defined, keeping one definition per retail address.
extern std::uint16_t g_539af0;

// 0x00401A20: clears the type slot for a class id (engine/src/retail/core/native_401a20.cpp); masks its argument to the
// low 16 bits.
extern void NATIVE_DECL TaskType_Clear_401A20(std::uint16_t class_id);

void NATIVE_DECL Task_ClearClassType_4199B0()
{
    // 0x004199B0: `mov ax,[0x00539AF0]`; 0x004199B6: `push %eax`; 0x004199B7: `call 0x00401A20`. The `add $0x4,%esp` at
    // 0x004199BC is that one argument's cdecl cleanup and is emitted by the compiler. No 0x004C1830 call: this class
    // registers no task-parameter container.
    TaskType_Clear_401A20(::g_539af0);

    // 0x004199BF: `mov word [0x00539AF0],0x0181` - restore the "not registered" sentinel.
    ::g_539af0 = 0x0181U;
}

} // extern "C"
