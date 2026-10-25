// Native: Core_OrGlobalFlags_401F60; retail VA 0x00401F60-0x00401F72 (19 bytes, 1 block, 0 edges).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 `af`/`pd` at 0x00401F60 - 19-byte body, ret at 0x00401F72, NOP
//   padding 0x00401F73-0x00401F7F, next function at 0x00401F80. The body has no `sub esp` and no
//   callee-saved pushes, so there is no stack frame and no 0x004A54A0 stack-probe site; r2's `afi`
//   `stackframe` field is a derived default and was NOT used to establish this.
// Behavior/CFG: single straight-line block, no calls, no branches, no string literals. Reads the
//   dword at [esp+4] (cdecl arg1), ORs it into the global at 0x00567C5C, and stores the result back -
//   a read-modify-write OR, exactly the instruction sequence 8b442404 8b0d5c7c5600 0bc8 890d5c7c5600 c3.
// Status/limits: STRUCTURALLY_VERIFIED (static only); runtime UNOBSERVED. The global 0x00567C5C had
//   no definition anywhere in this tree before this change; it is defined in
//   engine/src/retail/core/qhash_slot_table_globals.cpp, which owns the contiguous 0x00567Cxx block.
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
void NATIVE_DECL Core_OrGlobalFlags_401F60(std::uint32_t flags)
{
    NATIVE_LOG_ENTER("Core_OrGlobalFlags_401F60", "0x00401F60");
    igi::retail::core::g_567c5c |= flags; // 0x00401F6A: or ecx,eax; 0x00401F6C: mov [0x567c5c],ecx
}

} // extern "C"
