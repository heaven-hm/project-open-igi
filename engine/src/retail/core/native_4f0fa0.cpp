// Native: Core_GetRenderWordA_4F0FA0; retail VA 0x004F0FA0 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004F0FA0-0x004F0FA0 + 6 bytes, 1 block, 2 instructions: a1 (absolute load) / c3.
// Tools/evidence: radare2 6.2.0 pd; ranked as a self-contained leaf of the startup closure in evidence section 20.
// Behavior/CFG: return the dword at 0x00A70C7C; the retail image holds 0 there; no branch, no call, no stack use.
// Status/limits: BUILD_VERIFIED (1 block / 0 edges, emitted instructions identical); runtime UNOBSERVED. Returns the loaded dword because retail leaves it in EAX.
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

// 0xA70C7C: the word this native reads. No translation unit owns it; the retail image holds zero here.
std::uint32_t g_4f0fa0 = 0x0U;

std::uint32_t NATIVE_DECL Core_GetRenderWordA_4F0FA0()
{
    NATIVE_LOG_ENTER("Core_GetRenderWordA_4F0FA0", "0x004F0FA0");
    return ::g_4f0fa0;   // load from 0xA70C7C
}

} // extern "C"

#undef NATIVE_DECL
