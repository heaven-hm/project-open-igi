// Native: Sys_ClearSlotByte_4978E0 (behavior-derived placeholder name; retail symbol unknown; the VA suffix is required because 0x004B0EB0, the routine this body forwards to, carries the same placeholder form at its own VA). retail VA 0x004978E0-0x004978EB (12 bytes, 1 block, 0 edges, 5 instructions).
// Artifact: retail igi.exe SHA-256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; image base 0x00400000.
// Tools/evidence: radare2 6.2.0 `pd` at 0x004978E0 plus capstone 5.0.7 aligned from the verified entry; both agree on 5 instructions. Exact bytes 8b 44 24 04 50 e8 c6 95 01 00 59 c3 = `mov eax,[esp+4]` / `push %eax` / `call 0x004B0EB0` / `pop %ecx` / `ret`. The call displacement at 0x004978E7 is 0x000195C6 and resolves as v+5+rel32: 0x004978EA + 0x000195C6 = 0x004B0EB0. The ret is at 0x004978EB and the extent is 12 bytes. Call census from the whole image's 0xE8 enumeration: 14 call sites, of which two are the guard teardowns 0x0042C4E0 (0x0042C4F3) and 0x0042DAA0 (0x0042DAB3); the rest are 0x00466E16, 0x0049B2BC, 0x004A4F66, 0x004A4F72, 0x004A4F7E, 0x004A4F89, 0x004B73E6, 0x00512926, 0x00512932, 0x00530216, 0x005321D3, 0x005321DF.
// Behavior/CFG: one straight-line block, no branch, no frame and no saved register. It forwards its single dword argument to 0x004B0EB0 (Sys_ClearSlotByte_4B0EB0, sys/native_4b0eb0.cpp) and returns nothing; retail leaves EAX holding the callee's return value, and the callee is declared void. The callee clears the byte at 0x00A94A84 + index*8, so this body is the second entry point to that slot-clearing operation at a distinct retail VA. THE TWO GUARD TEARDOWNS THAT CALL IT ARE THE MEASURED DISTINCTION THAT MADE THIS FILE NECESSARY: 0x0042C4E0 and 0x0042DAA0 free their containers through 0x004978E0 instead of 0x004C1830, which is a real difference between teardowns and is recorded rather than normalised away.
// Status/limits: TRANSCRIBED; LINKABLE (its only undefined symbol, 0x004B0EB0, is a tree native, so it adds no new dependency); STRUCTURALLY_VERIFIED for the CFG only (1 block, 0 edges, no branch structure to disagree about); BUILD_VERIFIED (i686-w64-mingw32-g++ -std=c++20 -O2 -Wall -Wextra single-file compile with zero warning lines); runtime UNOBSERVED. The call is a real symbol reference rather than a cast: 0x004B0EB0 is a transcribed callee, and the cast-integer ruling covers handler data, not code the port must call. It is declared with exactly the linkage and parameter type its definition carries (extern "C", std::int32_t by value) so the two translation units agree on the ABI. Domain: sys/, because 0x004978E0 sits in the slot-name subsystem next to 0x004978D0 and forwards into the sys slot table at 0x00A94A84.

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

// 0x004B0EB0: clears one byte of the slot table (engine/src/retail/sys/native_4b0eb0.cpp); it writes zero to
// 0x00A94A84 + index*8.
void NATIVE_DECL Sys_ClearSlotByte_4B0EB0(std::int32_t index);

void NATIVE_DECL Sys_ClearSlotByte_4978E0(std::int32_t index)
{
    NATIVE_LOG_ENTER("Sys_ClearSlotByte_4978E0", "0x004978E0");
    // 0x004978E5: `call 0x004B0EB0`, displacement 0x000195C6 at 0x004978E7; the pop %ecx at 0x004978EA is the cdecl
    // cleanup for the one argument dword pushed at 0x004978E4.
    Sys_ClearSlotByte_4B0EB0(index);
}

} // extern "C"
