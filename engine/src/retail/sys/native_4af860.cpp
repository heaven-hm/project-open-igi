// Native: Sys_StoreWord_4AF860; retail VA 0x004AF860 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004AF860-0x004AF869 (10 bytes, 1 block, 0 edges, 3 instructions).
// Tools/evidence: radare2 6.2.0 af/pdf and the raw bytes; section 22.3 of the evidence sheet (landability sweep rows).
// Behavior/CFG: stores its argument into 0x00936268 and returns. No branch.
// Status/limits: STRUCTURALLY_VERIFIED; runtime UNOBSERVED. Any word it touches and no other translation unit owns is declared here and starts zeroed.
#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// 0x00936268: the word this setter stores.
std::uint32_t g_936268;

void NATIVE_DECL Sys_StoreWord_4AF860(std::uint32_t value)
{
    ::g_936268 = value;   // 0x004AF860: mov eax,[esp+4]; mov dword [0x00936268], eax
}   // 0x004AF869: ret

} // extern "C"

#undef NATIVE_DECL
