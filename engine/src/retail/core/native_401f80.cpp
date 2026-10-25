// Native: Core_AndNotGlobalFlags_401F80; retail VA 0x00401F80-0x00401F94 (21 bytes, 1 block, 0 edges).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 `af`/`pd` at 0x00401F80 - 21-byte body, ret at 0x00401F94, NOP
//   padding 0x00401F95-0x00401F96, next body at 0x00401F97+. No `sub esp`, no pushes, no calls, no
//   strings, so no stack frame and no 0x004A54A0 probe site.
// Behavior/CFG: single straight-line block. Reads the dword at [esp+4] (cdecl arg1), complements it
//   with `not`, ANDs the complement into the global at 0x00567C5C and stores the result back, i.e. it
//   clears the bits named by arg1. Instruction sequence 8b442404 8b0d5c7c5600 f7d0 23c8 890d5c7c5600 c3.
// Status/limits: STRUCTURALLY_VERIFIED (static only); runtime UNOBSERVED. This body is the exact
//   complement of 0x00401F60 and shares its global; it is NOT a 0x00402930 guard callee (zero
//   mentions in startup-402930-call-sequence.md) - it was identified by adjacency, four bytes of NOP
//   padding after a body that is on the critical path, the same adjacency that produced 0x00402070.
#include "dev_log.hpp"
#include "igi/retail/core/qhash_slot_table.hpp"

#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {
void NATIVE_DECL Core_AndNotGlobalFlags_401F80(std::uint32_t flags)
{
    NATIVE_LOG_ENTER("Core_AndNotGlobalFlags_401F80", "0x00401F80");
    igi::retail::core::g_567c5c &= ~flags; // 0x00401F8A: not eax; 0x00401F8C: and ecx,eax
}

} // extern "C"
