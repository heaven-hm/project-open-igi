// Native: Sys_InitFloatPair_4F1CB0; retail VA 0x004F1CB0 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004F1CB0-0x004F1CC9 (26 bytes, 1 block, 0 edges).
// Tools/evidence: radare2 6.2.0 af/afbj/pdf with bin.relocs.apply over the exact image (1 block / 4 instructions / 26 bytes; `pd` stops at the ret at
//   0x004F1CC9 and the next instruction at 0x004F1CCA is a nop). objdump listing build/retail-dis.txt covers the same four instructions.
// Behavior/CFG: stores the dword 0 at 0x00A76BE8 and the dword 0x42C80000 (100.0f) at 0x00A76BEC, then returns the address 0x00A76BE8 in eax. No
//   arguments, no branch, no callee, no other memory reference. This is the structural sibling of the already transcribed
//   Sys_InitFloatPair_4F1C90 (0x004F1C90), which does the same two stores at 0x00A76BF0/0x00A76BF4 with 0 and 0x40000000 (2.0f) and returns
//   0x00A76BF0; the two routines differ only in the global pair and the second dword's bit pattern.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. The two dwords are written as raw bit patterns because that is what retail stores; nothing in the
//   routine establishes whether callers read the second dword as a float (the sibling's 0x40000000 suggests the float reading is intended, and
//   0x42C80000 is 100.0f, but the routine itself only performs stores). The returned pointer is the address of the first dword. 0x00A76BE8 and
//   0x00A76BEC have no other owner in the tree, so this file defines them, zeroed as the loader leaves them. Domain: sys/, because the pair is a
//   sys-level static initialised by its own routine and the neighbouring 0x004F1C90 lives there too.
#include "dev_log.hpp"
#include <cstdint>

#define NATIVE_DECL __attribute__((cdecl))

extern "C" {

std::uint32_t g_a76be8 = 0U; // retail 0x00A76BE8: first dword of the pair (image value 0)
std::uint32_t g_a76bec = 0U; // retail 0x00A76BEC: second dword of the pair (image value 0)

void* NATIVE_DECL Sys_InitFloatPair_4F1CB0(void)
{
    NATIVE_LOG_ENTER("Sys_InitFloatPair_4F1CB0", "0x004F1CB0");
    ::g_a76be8 = 0U;          // 0x004F1CB0: mov dword [0x00A76BE8],0
    ::g_a76bec = 0x42C80000U; // 0x004F1CBA: mov dword [0x00A76BEC],0x42C80000 (100.0f as stored)
    return &::g_a76be8;       // 0x004F1CC4: mov eax,0x00A76BE8; 0x004F1CC9: ret
}

} // extern "C"

#undef NATIVE_DECL
