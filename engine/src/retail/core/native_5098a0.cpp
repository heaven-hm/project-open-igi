// Native: Sys_GetWord16_5098A0; retail VA 0x005098A0 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x005098A0-0x005098A6 (7 bytes, 1 block, 0 edges, 2 instructions: 66a13ce85400 / c3).
// Tools/evidence: radare2 6.2.0 af/pdf and the raw bytes; section 22.3 of the evidence sheet.
// Behavior/CFG: returns the 16-bit word at 0x0054E83C; no branch, no call, no store, no argument.
// Status/limits: BUILD_VERIFIED; runtime UNOBSERVED. Deviation recorded: retail loads AX only and leaves EAX's upper half untouched, while a std::uint16_t return zero-extends EAX; equal for a 16-bit consumer, different for a reader of the upper half. The word at 0x0054E83C is defined here with its retail image value 0x0181 (file bytes 81 01); its only retail writer is the "AIGraph" registrar 0x00508FC7 at 0x00508FA0 (core/native_508fa0.cpp), which declares this definition extern.
#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// 0x0054E83C: the word this getter returns. .data image value 0x0181 (bytes 81 01 at 0x0054E83C), not zero-fill; the
// registrar 0x00508FA0 overwrites it with the "AIGraph" class id at 0x00508FC7.
std::uint16_t g_54e83c = 0x0181U;

std::uint16_t NATIVE_DECL Sys_GetWord16_5098A0(void)
{
    return ::g_54e83c;   // 0x005098A0: mov ax, word [0x0054E83C]
}   // 0x005098A6: ret

} // extern "C"

#undef NATIVE_DECL
