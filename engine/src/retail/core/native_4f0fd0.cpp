// Native: Core_GetRenderWordB_4F0FD0; retail VA 0x004F0FD0 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004F0FD0-0x004F0FD0 + 6 bytes, 1 block, 2 instructions: a1 (absolute load) / c3.
// Tools/evidence: radare2 6.2.0 pd; ranked as a self-contained leaf of the startup closure in evidence section 20.
// Behavior/CFG: return the dword at 0x00A71884; the retail image holds 0 there; no branch, no call, no stack use.
// Status/limits: STRUCTURALLY_VERIFIED (1 block / 0 edges, emitted instructions identical); runtime UNOBSERVED. Returns the loaded dword because retail leaves it in EAX.
#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// 0xA71884: the word this native reads. No translation unit owns it; the retail image holds zero here.
std::uint32_t g_4f0fd0 = 0x0U;

std::uint32_t NATIVE_DECL Core_GetRenderWordB_4F0FD0()
{
    return ::g_4f0fd0;   // load from 0xA71884
}

} // extern "C"

#undef NATIVE_DECL
