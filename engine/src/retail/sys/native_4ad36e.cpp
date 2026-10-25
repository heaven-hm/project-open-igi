// Native: Sys_ReturnZero_4AD36E; retail VA 0x004AD36E (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004AD36E-0x004AD370 (3 bytes, 1 block, 0 edges, 2 instructions).
// Tools/evidence: radare2 6.2.0 af/pdf and NASM ndisasm over the raw bytes (33 c0 c3); reached from the 0x00402930 chain as a callback.
// Behavior/CFG: clears eax and returns; no arguments, no stores, no callee.
// Status/limits: BUILD_VERIFIED; runtime UNOBSERVED. The next entry at 0x004AD371 starts a different routine's prologue, so this body is exactly the two instructions.
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

std::int32_t NATIVE_DECL Sys_ReturnZero_4AD36E(void)
{
    NATIVE_LOG_ENTER("Sys_ReturnZero_4AD36E", "0x004AD36E");
    return 0;                                                          // 0x004AD36E: xor eax,eax; 0x004AD370: ret
}

} // extern "C"

#undef NATIVE_DECL
