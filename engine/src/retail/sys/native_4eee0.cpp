#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// Retail 0x0054DC30: read by this getter; no writer transcribed yet, so the storage is
// defined here and must become extern once its writer lands. Third and second of the
// three-sibling byte cluster at 0x0054DC30/31/32, alongside sys/native_4ee100.cpp.
extern "C" std::uint8_t g_54dc92 = 0xFFU;   // image value 0xFF

// Native: Sys_GetGlobal8_4EEE0; retail VA 0x004EE0E0.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Radare2 `pd` over the exact image; VA 0x004EE0E0..0x004EE0E5, 6 bytes, 1 block, 0 edges, bytes a0 30 dc 54 00 c3. Whole-file sweep for both the VA and the storage: the symbol was declared extern in native_42db00.cpp but had no definition anywhere, which is why this file exists.
// Behavior/CFG: `mov al,byte [0x0054DC30]; ret` -- one block, unconditional load, no branch and no side effect. Returns the byte in AL; the upper 24 bits of EAX are not cleared, which is what retail leaves. Callers: pushed as an event id into a 0x00401400 handler link.
// Status/limits: STRUCTURALLY_VERIFIED; literal one-block copy. No runtime observation in either binary. The storage has no transcribed writer, so it is defined here rather than bound to an owner.
std::uint8_t NATIVE_DECL Sys_GetGlobal8_4EEE0()
{
    return g_54dc92;
}

} // extern "C"
