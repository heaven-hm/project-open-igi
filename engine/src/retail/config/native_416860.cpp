// Native: Sys_GetSettingByte_416860; retail VA 0x00416860 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x00416860-0x00416865 (6 bytes, 1 block, 0 edges, 2 instructions).
// Tools/evidence: radare2 6.2.0 af/pdf and NASM ndisasm over the raw bytes (a0 4b 95 53 00 c3); the byte at 0x0053954B has three references in the image (writer 0x004155F1, readers here and one other).
// Behavior/CFG: loads byte 0x0B of the 0x00539540 setting block into al and returns; no arguments, no branch, no store.
// Status/limits: BUILD_VERIFIED; runtime UNOBSERVED. The extent of the block is the measured 0x0F bytes covering 0x00539540..0x0053954E; the setting's meaning is not established by this body.
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

std::uint8_t NATIVE_DECL Sys_GetSettingByte_416860(void)
{
    return ::g_539540[0x0BU];                                     // 0x00416860: mov 0x53954b,%al; 0x00416865: ret
}

} // extern "C"

#undef NATIVE_DECL
