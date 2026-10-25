// Native: Sys_GetSettingByte_416870; retail VA 0x00416870 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x00416870-0x00416875 (6 bytes, 1 block, 0 edges, 2 instructions).
// Tools/evidence: radare2 6.2.0 af/afbj/pdf over the exact image; the byte address 0x0053954C is the mov immediate and lies inside the 0x00539540-0x0053954E setting region that the sibling 0x00416850 transcription documents (43 references image-wide), at offset 0x0C.
// Behavior/CFG: loads the one byte at 0x0053954C into al and returns it; no arguments, no branch, no store, no callee. Only al is written, so the upper three bytes of eax are whatever the caller left there - the declared return type is therefore byte-sized.
// Status/limits: BUILD_VERIFIED; runtime UNOBSERVED. The value's meaning is not established by this body; it is the setting byte at offset 0x0C of the region declared in config_bytes.hpp, and the writers of that region are outside this routine.
#include "dev_log.hpp"
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

std::uint8_t NATIVE_DECL Sys_GetSettingByte_416870(void)
{
    NATIVE_LOG_ENTER("Sys_GetSettingByte_416870", "0x00416870");
    return ::g_539540[0x0CU];                                                   // 0x00416870: mov 0x0053954C,%al
}

} // extern "C"

#undef NATIVE_DECL
