// Native: Sys_GetGlobal8_46AB20; retail VA 0x0046AB20 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x0046AB20-0x0046AB26 (6 bytes, 1 block, 0 edges, 2 instructions).
// Tools/evidence: radare2 6.2.0 af/pdf and NASM ndisasm over the raw bytes (a0 60 f7 53 00 c3).
// Behavior/CFG: loads the one-byte value at 0x0053F760 into al and returns it; no argument, no branch, no store, no call.
// Status/limits: BUILD_VERIFIED; runtime UNOBSERVED. One-byte is the width this load proves; the value's meaning is not established.
#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// 0x0053F760: the one-byte object this getter reads. This file does NOT own it. The definition belongs to the registrar that
// writes it, core/native_4696b0.cpp (0x004696B0, Task_DefineComputerMap), which stores QTask_AllocateEvent(0) there and
// initialises it to 0xFF - the image byte at 0x0053F760 is ff 00 00 00, so the object is .data, not .bss. This file previously
// defined it as a zero-initialised std::uint8_t, which was wrong twice over: it duplicated the definition (the linker rejected
// the pair with "multiple definition of g_53f760") and it declared the wrong storage class, since a zero image value is what
// would put it in .bss. One owner per retail address, and the owner is the writer, not the reader.
extern std::uint8_t g_53f760;

std::uint8_t NATIVE_DECL Sys_GetGlobal8_46AB20(void)
{
    return ::g_53f760;                                                       // 0x0046AB20: mov 0x0053f760,%al; 0x0046AB25: ret
}

} // extern "C"

#undef NATIVE_DECL
