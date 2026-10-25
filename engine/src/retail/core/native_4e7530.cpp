// Native: Core_GetEventWord_4E7530; retail VA 0x004E7530 (placeholder name; retail symbol unknown, candidate label StatusScreen_GetEvent is unverified).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004E7530-0x004E7535 (6 bytes, 1 block, 2 instructions: a1 c8885400 / c3).
// Tools/evidence: radare2 6.2.0 pd; triaged as the highest-leverage leaf of the startup sequence 0x00402930 in evidence section 20 (34 dependent rows, 88 call sites).
// Behavior/CFG: returns the dword at 0x005488C8; no branch, no call, no other memory access, no stack use.
// Status/limits: BUILD_VERIFIED (1 block / 0 edges, emitted instructions identical); runtime UNOBSERVED. Returns the loaded dword because retail leaves it in EAX and callers may read it.
#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// 0x005488C8: the word this native reads. No translation unit owns it. The retail image holds 255 at the address, so the
// initialiser is 0xFF rather than 0 - the readers of this word depend on that initial value.
std::uint32_t g_5488c8 = 0xFFU;

std::uint32_t NATIVE_DECL Core_GetEventWord_4E7530()
{
    return ::g_5488c8;   // 0x004E7530: mov eax, dword [0x005488C8]
}                       // 0x004E7535: ret

} // extern "C"

#undef NATIVE_DECL
