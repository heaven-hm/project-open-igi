// Native: Sys_GetSettingByte_416880; retail VA 0x00416880 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x00416880-0x00416885 (6 bytes, 1 block, 0 edges, 2 instructions).
// Tools/evidence: radare2 6.2.0 af/pdf and NASM ndisasm over the raw bytes (a0 4d 95 53 00 c3); the reads of 0x00539540..0x0053954E and 0x005690D8 are measured across the whole image (43 references to the first region, 5 to the second).
// Behavior/CFG: loads one byte into al and returns; no arguments, no branch, no store, no callee. Only al is written, so the upper three bytes of eax are whatever the caller left there - the declared return type is therefore byte-sized.
// Status/limits: BUILD_VERIFIED; runtime UNOBSERVED. Static structure matches 1 block / 0 edges, but the strict gate requires an authorized runtime contract and paired observations, which the candidate cannot produce while it exits at the 0x00402930 boundary, so this record is not promoted. The routine returns the setting byte at 0x00539540+0x0D; the region's meaning and its writers are not established by this body.
#include "igi/retail/config/config_bytes.hpp"

#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

std::uint8_t NATIVE_DECL Sys_GetSettingByte_416880(void)
{
    return ::g_539540[0x0DU];                                                   // 0x00416880: mov 0x0053954D,%al
}

} // extern "C"

#undef NATIVE_DECL
