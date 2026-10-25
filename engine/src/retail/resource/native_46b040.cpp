// Native: Sys_GetPointer5BE2B0_46B040; retail VA 0x0046B040 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x0046B040-0x0046B046 (6 bytes, 1 block, 0 edges, 2 instructions).
// Tools/evidence: radare2 6.2.0 af/pdf and NASM ndisasm over the raw bytes (a1 b0 e2 5b 00 c3). 0x005BE2B0 is read as a four-byte value, which is the width this body proves; the caller at 0x0046B4E0 then stores through the returned pointer at +0x38.
// Behavior/CFG: loads the dword at 0x005BE2B0 into eax and returns it; no argument, no branch, no store, no call.
// Status/limits: BUILD_VERIFIED; runtime UNOBSERVED. The routine publishes a stored pointer value; what the pointed-to object is and when it is assigned are not established here, only the four-byte load.
#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// 0x005BE2B0: the stored dword. Four bytes, from the dword load this body performs.
std::uint32_t g_5be2b0;

std::uint32_t NATIVE_DECL Sys_GetPointer5BE2B0_46B040(void)
{
    return ::g_5be2b0;                                                  // 0x46B040: mov 0x005BE2B0,%eax; 0x46B045: ret
}

} // extern "C"

#undef NATIVE_DECL
