// Native: Sys_ResetSlotWord_4B0F70; retail VA 0x004B0F70 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004B0F70-0x004B0F7A (11 bytes, 1 block, 0 edges, 2 instructions: c705644aa900ffffffff / c3).
// Tools/evidence: radare2 6.2.0 af/pdf and the raw bytes; section 22.3 of the evidence sheet (landability sweep row 0x004B0F70).
// Behavior/CFG: stores 0xFFFFFFFF into the dword at 0x00A94A64 and returns; no branch, no call, no argument, no stack use.
// Status/limits: BUILD_VERIFIED; runtime UNOBSERVED. The word 0x00A94A64 already has an owner inside engine/ (the sweep marked it owner), so this file does not define it.
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

// 0x00A94A64: the word this routine resets. texture_loaders.cpp has a same-named static in an anonymous namespace, which
// is internal linkage and not this global; no external owner exists, so this file defines it.
std::int32_t g_a94a64;

void NATIVE_DECL Sys_ResetSlotWord_4B0F70(void)
{
    NATIVE_LOG_ENTER("Sys_ResetSlotWord_4B0F70", "0x004B0F70");
    ::g_a94a64 = -1;   // 0x004B0F70: mov dword [0x00A94A64], 0xFFFFFFFF
}   // 0x004B0F7A: ret

} // extern "C"

#undef NATIVE_DECL
