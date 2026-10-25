// Native: Script_QHashSymbolRemove_4B8920 (behavior-derived placeholder name; retail symbol unknown; the form follows the tree's own name for the lookup it depends on, Script_QHashSymbolLookup_4BAB80). retail VA 0x004B8920-0x004B892B (12 bytes, 1 block, 0 edges, 5 instructions).
// Artifact: retail igi.exe SHA-256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; image base 0x00400000.
// Tools/evidence: radare2 6.2.0 `pd` at 0x004B8920 plus capstone 5.0.7 aligned from the verified entry; both agree on 5 instructions. Exact bytes 8b 44 24 04 50 e8 06 22 00 00 59 c3 = `mov eax,[esp+4]` / `push %eax` / `call 0x004BAB30` / `pop %ecx` / `ret`. The call displacement at 0x004B8927 is 0x00002206 and resolves as v+5+rel32: 0x004B892A + 0x00002206 = 0x004BAB30. The ret is at 0x004B892B and the extent is 12 bytes. Call census from the whole image's 0xE8 enumeration: 127 call sites, so this is a widely used routine and not a teardown-only helper; among them are the four in the guard teardown 0x004213E0 (0x004213F0, 0x004213FA, 0x00421404, 0x0042140E, the four TEXT_* names), the two in 0x00423F90 (0x00423F95, 0x00423F9F, the two ControlsMenu_Convert* names) and the nine in 0x00424790 (0x00424795-0x004247E5, the nine LINEUPMODE_* names).
// Behavior/CFG: one straight-line block, no branch, no frame and no saved register. It forwards its single argument to 0x004BAB30 and returns nothing; retail leaves EAX holding the callee's return value. WHAT THE CALLEE DOES IS MEASURED, NOT ASSUMED, because it is not transcribed: 0x004BAB30 takes the same one argument, calls Script_QHashSymbolLookup_4BAB80(name, &name_slot) with the address of its own argument slot, and on a failed lookup reports the string at 0x005476FC - "Undefined symbol: '%s'" - through 0x004AF7B0 and halts in a self-jump; on a successful lookup it removes the found node with 0x004C0460(table at 0x00A43E88, node), 0x004BAA90(node) and Mem_DeAllocFn(node). The node reaches those three calls because the lookup overwrites the argument slot with the node pointer. That is why the name records a removal and not a lookup, and it is why every guard teardown calls it with exactly the name string its registrar registered: a lookup whose result the caller discards would do nothing. The three call sites in this band push the name and never read EAX.
// Status/limits: TRANSCRIBED; NOT LINKABLE YET - 0x004BAB30 is a NATIVE GAP declared here (76 bytes to its self-jump at 0x004BAB7A, 3 blocks, 3 edges, 27 instructions) and has no implementation file, so this file cannot link until it lands. It is not a handler, so the cast-integer ruling does not reach it, and a cast call is never the answer here: it would convert a build error into a runtime crash. 0x004BAB30 is itself a DOUBLE GAP: three of its own callees, 0x004C0460, 0x004BAA90 and 0x004AF7B0, also have no implementation file (its other two, 0x004BAB80 and 0x004B0D10, are tree natives), so the removal path cannot be transcribed cleanly until those three land. STRUCTURALLY_VERIFIED for the CFG only (1 block, 0 edges); BUILD_VERIFIED (i686-w64-mingw32-g++ -std=c++20 -O2 -Wall -Wextra single-file compile with zero warning lines); runtime UNOBSERVED. THE NAME IS A PLACEHOLDER: it is derived from the measured callee behavior and from the tree's existing name for 0x004BAB80, not from a retail symbol, and it must be revisited when 0x004BAB30 is transcribed. Domain: script/, because the callee chain is the script symbol table at 0x00A43E88 and the arguments are script symbol names.

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

// NATIVE GAP: 0x004BAB30 has no implementation file in this tree. It is declared here so the call below is a real
// symbol reference rather than a cast, and THIS FILE CANNOT LINK UNTIL IT LANDS. Measured: 76 bytes to the self-jump
// at 0x004BAB7A (3 blocks, 3 edges, 27 instructions); it looks the name up through 0x004BAB80, reports the
// "Undefined symbol: '%s'" string at 0x005476FC through 0x004AF7B0 when the lookup fails, and otherwise removes the
// node through 0x004C0460, 0x004BAA90 and 0x004B0D10 (Mem_DeAllocFn). Its own name is a placeholder too.
void NATIVE_DECL Script_QHashSymbolRemove_4BAB30(char const* name);

void NATIVE_DECL Script_QHashSymbolRemove_4B8920(char const* name)
{
    NATIVE_LOG_ENTER("Script_QHashSymbolRemove_4B8920", "0x004B8920");
    // 0x004B8925: `call 0x004BAB30`, displacement 0x00002206 at 0x004B8927; the pop %ecx at 0x004B892A is the cdecl
    // cleanup for the one argument dword pushed at 0x004B8924. The callee writes the found node pointer back into
    // this argument slot (0x004BAB35: `lea 0x8(%esp),%eax`), and the slot is discarded on return, exactly as retail does.
    Script_QHashSymbolRemove_4BAB30(name);
}

} // extern "C"
