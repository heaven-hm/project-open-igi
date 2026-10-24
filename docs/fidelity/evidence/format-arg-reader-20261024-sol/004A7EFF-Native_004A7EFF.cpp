#include "004A7EFF-Native_004A7EFF.hpp"

// Native: Native_004A7EFF; retail VA 0x004A7EFF; x86 cdecl, pointer to a DWORD cursor, raw DWORD result.
// Artifact: igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja 6.0.10601, Radare2 6.2.0, angr CFGFast; docs/fidelity/evidence/format-arg-reader-20261024-sol/.
// CFG/behavior: one block; advance the cursor cell by four bytes BEFORE loading the DWORD at the previous address, preserving alias/order and ADD flags.
// Status/limits: TRANSCRIBED candidate for native-only replay; original identifiers unknown; no natural candidate formatter call flow or production intake is claimed.
#if defined(_WIN32) && defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
// Explicit function assembly retains the 13-byte retail boundary; no compiler
// epilogue, flag-changing pointer conversion, or unreachable UD2 is introduced.
__asm__(
    ".text\n"
    ".globl _Native_004A7EFF\n"
    ".def _Native_004A7EFF; .scl 2; .type 32; .endef\n"
    "_Native_004A7EFF:\n"
    "movl 4(%esp), %eax\n"
    "addl $4, (%eax)\n"
    "movl (%eax), %eax\n"
    "movl -4(%eax), %eax\n"
    "ret\n");
#else
#error "Native_004A7EFF requires the Windows x86 compiler"
#endif
