// Native: Sys_GetGlobal32_4F0FE0; retail VA 0x004F0FE0 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004F0FE0-0x004F0FE5 (6 bytes, 1 block, 0 edges, 2 instructions).
// Tools/evidence: radare2 6.2.0 af/pdf and NASM ndisasm over the raw bytes (a1 88 18 a7 00 c3); the storage at 0x00A71888 has measured writers and readers across the image, listed in docs/fidelity/evidence/high-indegree-getters-20261025/.
// Behavior/CFG: loads 4 byte(s) from the static object at 0x00A71888 into %eax and returns; no arguments, no branch, no store, no callee. Only %eax is written, so the declared return type is exactly the access width.
// Status/limits: BUILD_VERIFIED; runtime UNOBSERVED. The value's meaning is not established by this body; the access width is the only proven fact about the object, and this file is its defining file.
#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// 0x00A71888: the 4-byte object this getter reads. No other owner in the tree; extent proven by the access width used
// by every reference to this address in the image.
std::uint32_t g_a71888;

std::uint32_t NATIVE_DECL Sys_GetGlobal32_4F0FE0(void)
{
    return g_a71888;                                                     // 0x004F0FE0: mov 0x00a71888,%eax; 0x004F0FE5: ret
}

} // extern "C"

#undef NATIVE_DECL
