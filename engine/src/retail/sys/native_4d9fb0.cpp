// Native: Sys_GetGlobal8_4D9FB0; retail VA 0x004D9FB0 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004D9FB0-0x004D9FB5 (6 bytes, 1 block, 0 edges, 2 instructions).
// Tools/evidence: radare2 6.2.0 af/pdf and NASM ndisasm over the raw bytes (a0 4a 86 54 00 c3); the storage at 0x0054864A has measured writers and readers across the image, listed in docs/fidelity/evidence/high-indegree-getters-20261025/.
// Behavior/CFG: loads 1 byte(s) from the static object at 0x0054864A into %al and returns; no arguments, no branch, no store, no callee. Only %al is written, so the declared return type is exactly the access width.
// Status/limits: BUILD_VERIFIED; runtime UNOBSERVED. The value's meaning is not established by this body; the access width is the only proven fact about the object, and this file is its defining file.
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

// 0x0054864A: the 1-byte object this getter reads. No other owner in the tree; extent proven by the access width used
// by every reference to this address in the image.
std::uint8_t g_54864a;

std::uint8_t NATIVE_DECL Sys_GetGlobal8_4D9FB0(void)
{
    NATIVE_LOG_ENTER("Sys_GetGlobal8_4D9FB0", "0x004D9FB0");
    return g_54864a;                                                     // 0x004D9FB0: mov 0x0054864a,%al; 0x004D9FB5: ret
}

} // extern "C"

#undef NATIVE_DECL
