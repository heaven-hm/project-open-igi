// Native: Sys_GetBlock5C8A4C_4871B0; retail VA 0x004871B0 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004871B0-0x004871B6 (6 bytes, 1 block, 0 edges, 2 instructions).
// Tools/evidence: radare2 6.2.0 af/pdf and NASM ndisasm over the raw bytes (b8 4c 8a 5c 00 c3). The block 0x005C8A4C extent is not inferred here: 0x00486994-0x004869A6 copies exactly 0x28 bytes into it (mov $0xA,%ecx; mov $0x540B70,%esi; rep movsl), which fixes the object at 0x28 bytes (docs/fidelity/evidence/0x402930-chain-waves.md).
// Behavior/CFG: returns the address of the 0x005C8A4C block in eax; no argument, no branch, no memory access, no call.
// Status/limits: BUILD_VERIFIED; runtime UNOBSERVED. This routine only publishes the block address; the block's meaning is not established, only its exact 0x28-byte extent from the copy that fills it.
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

// 0x005C8A4C: exactly 0x28 bytes, fixed by the rep movsl count at 0x00486994.
std::uint8_t g_5c8a4c[0x28];

std::uint8_t* NATIVE_DECL Sys_GetBlock5C8A4C_4871B0(void)
{
    NATIVE_LOG_ENTER("Sys_GetBlock5C8A4C_4871B0", "0x004871B0");
    return ::g_5c8a4c;                                                   // 0x4871B0: mov $0x005C8A4C,%eax; 0x4871B5: ret
}

} // extern "C"

#undef NATIVE_DECL
