// Native: TaskEventTable_ClearSlot_4018E0 (behavior-derived placeholder name; retail symbol unknown; the VA suffix is required because 0x004018C0 carries the same operation under the same placeholder form at its own VA). retail VA 0x004018E0-0x004018EB (12 bytes, 1 block, 0 edges, 5 instructions).
// Artifact: retail igi.exe SHA-256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; image base 0x00400000.
// Tools/evidence: radare2 6.2.0 `pd` at 0x004018E0 plus capstone 5.0.7 aligned from the verified entry; both agree on 5 instructions. Exact bytes 8b 44 24 04 50 e8 d6 ff ff ff 59 c3 = `mov eax,[esp+4]` / `push %eax` / `call 0x004018C0` / `pop %ecx` / `ret`. The call displacement at 0x004018E7 is 0xFFFFFFD6 and resolves as v+5+rel32: 0x004018EA + 0xFFFFFFD6 = 0x004018C0. The ret is at 0x004018EB and the extent is 12 bytes. Call census from the whole image's 0xE8 enumeration (not from a decompiler's function list): 12 call sites, of which four are inside the guard teardown 0x00424790 (0x004247F0, 0x004247FC, 0x00424808, 0x00424813, the four dword slots 0x0053B910/14/18/1C) and the rest are 0x004D0CA8, 0x004E64D1, 0x004E7506, 0x004F0BDB, 0x004F0BE7, 0x004F0BF2, 0x004F1F9C, 0x004F1FA8.
// Behavior/CFG: one straight-line block, no branch, no frame and no saved register. It forwards its single dword argument to 0x004018C0 (TaskEventTable_ClearSlot, core/native_4018c0.cpp) and returns nothing; retail leaves EAX holding the callee's return value, and the callee is declared void. The callee masks the argument to its low 8 bits, so this entry point is behaviorally identical to 0x004018C0 - it is a distinct retail VA and both are transcribed as separate natives, because rule 13 gives one implementation per VA and rule 2 forbids folding a retail routine into its caller.
// Status/limits: TRANSCRIBED; LINKABLE (its only undefined symbol, 0x004018C0, is a tree native, so it adds no new dependency); STRUCTURALLY_VERIFIED for the CFG only (1 block, 0 edges, no branch structure to disagree about); BUILD_VERIFIED (i686-w64-mingw32-g++ -std=c++20 -O2 -Wall -Wextra single-file compile with zero warning lines); runtime UNOBSERVED. The call is a real symbol reference rather than a cast: 0x004018C0 is a transcribed callee, and the cast-integer ruling covers handler data, not code the port must call. It is declared with exactly the linkage and parameter type its definition carries (extern "C", std::uint32_t by value) so the two translation units agree on the ABI. Domain: core/, because it is the event-table slot clearer next to 0x004018C0 and it is called by the guard teardown family.

#include "dev_log.hpp"
#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// 0x004018C0: clears the event-table slot for an index (engine/src/retail/core/native_4018c0.cpp). It masks its
// argument to the low 8 bits and clears the byte at 0x0054F932 + index*6.
void TaskEventTable_ClearSlot(std::uint32_t const index);

void NATIVE_DECL TaskEventTable_ClearSlot_4018E0(std::uint32_t const index)
{
    NATIVE_LOG_ENTER("TaskEventTable_ClearSlot_4018E0", "0x004018C0");
    // 0x004018E5: `call 0x004018C0`, displacement 0xFFFFFFD6 at 0x004018E7; the pop %ecx at 0x004018EA is the cdecl
    // cleanup for the one argument dword pushed at 0x004018E4.
    TaskEventTable_ClearSlot(index);
}

} // extern "C"
