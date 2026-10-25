// Native: Core_GetGlobal32_460C50; retail VA 0x00460C50 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x00460C50-0x00460C55
//   (6 bytes, 1 block, 2 instructions: a1 18 dc 5b 00 / c3).
// Tools/evidence: radare2 6.2.0 af/afbj/pdf over the exact image; afbj reports 1 block / 2 instructions and the raw bytes decode as
//   `mov eax, dword [0x005BDC18]` then ret. No other translation unit owns 0x005BDC18; the retail image holds 0.
// Behavior/CFG: returns the dword at 0x005BDC18; no branch, no call, no stack use.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. The whole EAX is written, so the full 32-bit value is returned.
#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

std::uint32_t g_5bdc18 = 0U; // retail 0x005BDC18: the dword this native reads (image value 0)
std::uint32_t NATIVE_DECL Core_GetGlobal32_460C50()
{
    return ::g_5bdc18;   // 0x00460C50: `mov eax, dword [0x005BDC18]`
}                        // 0x00460C55: ret

} // extern "C"

#undef NATIVE_DECL
