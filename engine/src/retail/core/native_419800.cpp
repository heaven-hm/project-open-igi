// Native: BackgroundFX_AdvanceField20_419800; retail VA 0x00419800 (0x00419800-0x00419810, 17 bytes, 5 instructions, 1 block, 0 edges).
// Artifact: retail igi.exe SHA-256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; image base 0x00400000.
// Tools/evidence: build/retail-dis.txt (GNU objdump i386 byte+instruction listing, bytes 8b 44 24 04 d9 40 20 d8 05 e0 33 53 00 d9 58 20 c3); radare2 6.2.0 af/afij (size 17, 1 block, 0 edges, 5 instructions); the 1.0f constant at 0x005333E0 is verified by its bytes (00 00 80 3f) and by the existing note in engine/src/retail/terrain/terrain_mipmaps.cpp:108; the caller 0x00419760 pushes this address as event-1 handler (`push $0x419800; push $0x1; push <class id>; call 0x00401530`).
// Behavior/CFG: one pointer argument, no branches: `mov 0x4(%esp),%eax; flds 0x20(%eax); fadds 0x5333e0; fstps 0x20(%eax)` - load the float at object+0x20, add the float constant 1.0f, store it back as a 32-bit float.
// Status/limits: BUILD_VERIFIED (static only); runtime UNOBSERVED. No callees; the only data reference is the read-only 1.0f constant at 0x005333E0, reproduced as the literal 1.0F. Retail statement order preserved: the argument load (0x00419800), the load (0x00419804), the add (0x00419807) and the store (0x0041980D) are the whole body.
#include "dev_log.hpp"
#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// The 0x00419760 registrar installs this as event-1 handler. The retail body is the x87 float add below; the
// constant is the .rdata 1.0f at 0x005333E0, not a computed or guessed value.
void NATIVE_DECL BackgroundFX_AdvanceField20_419800(void* object)
{
    NATIVE_LOG_ENTER("BackgroundFX_AdvanceField20_419800", "0x00419800");
    std::uint8_t* const target = reinterpret_cast<std::uint8_t*>(object);
    // 0x00419804-0x00419810: `flds 0x20(%eax); fadds 0x5333e0; fstps 0x20(%eax)`, the 0x005333E0 constant being 1.0f.
    float* const field = reinterpret_cast<float*>(target + 0x20);
    *field = *field + 1.0F;
}

} // extern "C"

#undef NATIVE_DECL
