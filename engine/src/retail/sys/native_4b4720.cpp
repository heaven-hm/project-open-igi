// Native: Sys_StoreThreeWords_4B4720; retail VA 0x004B4720 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004B4720-0x004B4738 (25 bytes, 1 block, 0 edges, 5 instructions).
// Tools/evidence: radare2 6.2.0 af/pdf and NASM ndisasm over the raw bytes (8b 44 24 04 c7 00 07 1a fb c8 c7 40 04 56 77 ea 39 c7 40 08 cd 3c f1 7f c3); nine nop bytes follow.
// Behavior/CFG: stores three immediate dwords at argument+0x00, +0x04 and +0x08. No branch, no callee, no global; the argument pointer is returned in eax as a leftover of the load, so no return value is declared and the caller must not rely on one.
// Status/limits: STRUCTURALLY_VERIFIED; runtime UNOBSERVED. The three constants are bit patterns of unknown meaning (possibly floats or hashes); they are stored as raw dwords exactly as retail's immediates are, and nothing here interprets them.
#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

void NATIVE_DECL Sys_StoreThreeWords_4B4720(void* target)
{
    std::uint8_t* const out = static_cast<std::uint8_t*>(target);      // 0x004B4724: mov eax,[esp+4]
    *reinterpret_cast<std::uint32_t*>(out + 0x00U) = 0xC8FB1A07U;      // 0x004B4724: movl bashxC8FB1A07,(%eax)
    *reinterpret_cast<std::uint32_t*>(out + 0x04U) = 0x39EA7756U;      // 0x004B472A: movl bashx39EA7756,0x4(%eax)
    *reinterpret_cast<std::uint32_t*>(out + 0x08U) = 0x7FF13CCDU;      // 0x004B4731: movl bashx7FF13CCD,0x8(%eax)
}   // 0x004B4738: ret

} // extern "C"

#undef NATIVE_DECL
