// Native: BackgroundFX_ZeroField20_4197F0; retail VA 0x004197F0 (0x004197F0-0x004197FB, 12 bytes, 3 instructions, 1 block, 0 edges).
// Artifact: retail igi.exe SHA-256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; image base 0x00400000.
// Tools/evidence: build/retail-dis.txt (GNU objdump i386 byte+instruction listing, bytes 8b 44 24 04 c7 40 20 00 00 00 00 c3); radare2 6.2.0 af/afij (size 12, 1 block, 0 edges, 3 instructions); the caller 0x00419760 pushes this address as the phase-0 handler (`push $0x4197F0; push $0x0; push <class id>; call 0x00401400`).
// Behavior/CFG: one pointer argument in the stack slot, no branches: `mov 0x4(%esp),%eax` then `movl $0x0,0x20(%eax)`, i.e. store the dword 0 at object+0x20, and return with the value left in EAX.
// Status/limits: TRANSCRIBED (static only); runtime UNOBSERVED. No callees, no data references, so the file links by itself; the class association ("BackgroundFX", registered by 0x00419760) is the caller's evidence, not this body's.
//
// Retail statement order preserved: the argument load (0x004197F0) and the one store (0x004197F4) are the whole body;
// there is no stack cleanup because the caller cleans the argument slot.
#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// The 0x00419760 registrar installs this as the class's phase-0 handler. The retail table calls handlers with one
// pointer argument; the body only touches that pointer.
void NATIVE_DECL BackgroundFX_ZeroField20_4197F0(void* object)
{
    // 0x004197F0-0x004197F4: `mov 0x4(%esp),%eax; movl $0x0,0x20(%eax)`. The +0x20 dword is the same float field
    // the 0x00419800 handler advances by 1.0f, so the pair resets and reloads it.
    std::uint8_t* const target = reinterpret_cast<std::uint8_t*>(object);
    *reinterpret_cast<std::uint32_t*>(target + 0x20) = 0U;
}

} // extern "C"

#undef NATIVE_DECL
