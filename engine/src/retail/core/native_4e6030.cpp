// Native: Native_004E6030; retail VA 0x004E6030; original symbol unknown.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 instruction reads; fresh BN/r2/angr capture pending under build/native-analysis/.
// Behavior/CFG: 17 bytes; store argument at 0x00A5E5FC, call 0x00495CC0 with same word, return callee EAX.
// Status/limits: BUILD_VERIFIED; no paired typed runtime acceptance; no inferred original names.
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
std::uint32_t g_a5e5fc = 0; // Retail BSS word; meaning unresolved.
std::uint32_t NATIVE_DECL Native_00495CC0(std::uint32_t);
std::uint32_t NATIVE_DECL Native_004E6030(std::uint32_t value)
{
    NATIVE_LOG_ENTER("Native_004E6030", "0x004E6030");
    g_a5e5fc = value;
    return Native_00495CC0(value);
}
}

#undef NATIVE_DECL
