// Native: Core_GetFlagByte_413B40; retail VA 0x00413B40 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x00413B40-0x00413B45 (6 bytes, 1 block, 2 instructions: a0 b0915300 / c3).
// Tools/evidence: radare2 6.2.0 af/afbj/pdf over the exact image; afbj reports 1 block / 2 instructions and the raw bytes decode as `mov al, byte [0x005391B0]` then `ret`. Third byte of the same three-byte flag block as 0x00413B20 (0x005391AE) and 0x00413B30 (0x005391AF), each a separate routine at its own VA.
// Behavior/CFG: returns the byte at 0x005391B0; no branch, no call, no stack use.
// Status/limits: BUILD_VERIFIED; runtime UNOBSERVED. Same codegen deviation as 0x00413B20: retail writes only AL and leaves EAX bits 8-31 as the caller left them, while a C++ byte return makes the compiler zero-extend into EAX; the byte-level result is identical for every caller that consumes AL or a uint8_t. The byte's meaning is not established by this body; the image holds 255 at 0x005391B0.
#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// 0x005391B0: the byte this native reads. No translation unit owns it; the retail image holds 255 here.
std::uint8_t g_5391b0 = 0xFFU;

std::uint8_t NATIVE_DECL Core_GetFlagByte_413B40()
{
    return ::g_5391b0;   // 0x00413B40: mov al, byte [0x005391B0]
}                        // 0x00413B45: ret

} // extern "C"

#undef NATIVE_DECL
