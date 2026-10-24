// Native: Sys_GetGlobal16_424850; retail VA 0x00424850 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x00424850-0x00424855 (6 bytes, 1 block, 0 edges, 2 instructions).
// Tools/evidence: radare2 6.2.0 af/pdf and NASM ndisasm over the raw bytes (66 a1 0c b9 53 00 c3); the storage at 0x0053B90C has
// measured writers and readers across the image, listed in docs/fidelity/evidence/high-indegree-getters-20261025/.
// Behavior/CFG: loads 2 byte(s) from the static object at 0x0053B90C into %ax and returns; no arguments, no branch, no
// store, no callee. Only %ax is written, so the declared return type is exactly the access width.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. The value's meaning is not established by this body; the access width is the
// only proven fact about the object, and this file is its defining file.
#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// 0x0053B90C: the 2-byte object this getter reads. No other owner in the tree; extent proven by the access width used
// by every reference to this address in the image.
std::uint16_t g_53b90c;

std::uint16_t NATIVE_DECL Sys_GetGlobal16_424850(void)
{
    return g_53b90c;                                                     // 0x00424850: mov 0x0053b90c,%ax; 0x00424855: ret
}

} // extern "C"

#undef NATIVE_DECL
