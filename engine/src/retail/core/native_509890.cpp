// Native: Sys_TestWordMask_509890; retail VA 0x00509890 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x00509890-0x0050989E (15 bytes, 1 block, 0 edges, 5 instructions).
// Tools/evidence: radare2 6.2.0 af/pdf and the raw bytes 8b4c2404 8b442408 854130 0f95c0 c3; section 22.3 of the evidence sheet.
// Behavior/CFG: tests the first argument's +0x30 dword against the second argument and returns 1 when the AND is non-zero, 0 otherwise. No branch in retail: the setne is the branch-free form, so there is no block split.
// Status/limits: STRUCTURALLY_VERIFIED; runtime UNOBSERVED. Deviation recorded: retail writes only AL (setne al) and leaves EAX's upper bytes untouched, while a bool return zero-extends EAX; equal for a boolean consumer, different for a reader of the upper bits.
#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

bool NATIVE_DECL Sys_TestWordMask_509890(const void* object, std::uint32_t mask)
{
    return (*reinterpret_cast<const std::uint32_t*>(
                reinterpret_cast<const std::uint8_t*>(object) + 0x30U) & mask) != 0U;   // 0x00509898: test dword [ecx+0x30], eax; setne al
}   // 0x0050989E: ret

} // extern "C"

#undef NATIVE_DECL
