// Native: DynObj_GetByte_4C56E0; retail VA 0x004C56E0 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004C56E0-0x004C56E5 (6 bytes, 1 block, 0 edges, 2 instructions).
// Tools/evidence: radare2 6.2.0 af/pdf and the raw bytes; section 22.3 of the evidence sheet (landability sweep row).
// Behavior/CFG: returns the byte at 0x00547F3D; no branch, no call, no store, no argument.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. The word at 0x00547F3D has no other owner in the tree, so it is declared here and starts zeroed.
// Codegen note: retail writes only AL, leaving EAX's upper bytes as they were; a std::uint8_t return zero-extends
// EAX. Recorded as a documented deviation, the same one as 0x00413B20.
#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// 0x00547F3D: the word this getter returns.
std::uint8_t g_547f3d;

std::uint8_t NATIVE_DECL DynObj_GetByte_4C56E0(void)
{
    return ::g_547f3d;   // 0x004C56E0: mov al, byte [0x00547F3D]
}   // 0x004C56E5: ret

} // extern "C"

#undef NATIVE_DECL
