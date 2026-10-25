// Native: Sub_4168D0 (address-suffixed placeholder: this VA has no manifest record, and a two-instruction global load establishes no behaviour name); retail VA 0x004168D0-0x004168D5.
// Artifact: retail igi.exe SHA-256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; image base 0x00400000; .text VA 0x00401000 raw 0x1000.
// Tools/evidence: capstone 5.0.7 decode of the exact extent: 2 instructions, 6 bytes, one basic block, no branch, no call, `ret` at 0x004168D5, 10 `90` nops after (the next function starts at 0x004168E0). Exact bytes: A1 A8 BA 57 00 C3. ENTRY BOUNDARY: an image-wide arithmetic scan of every `E8` displacement finds 19 call sites, and the byte immediately before each call is a push of a register (for example `56` = push esi at 0x0042E16E, call at 0x0042E16F), so retail's callers pass one argument. The body reads none of it.
// Behavior/CFG: `mov eax,[0x0057BAA8]; ret` - one straight-line block, no frame, no saved registers, no branch. It returns the 32-bit global at 0x0057BAA8. That VA is above the image's file-backed data (reading it returns no file bytes), so the slot is zero-fill and is populated at runtime; this file defines it with the image's initial value 0, the same treatment the tree already gives the zero-fill table 0x00A976E0. The declared parameter is not read by the body: it is kept in the signature because retail's own call sites push it and the caller file declares it, and the fact is recorded here rather than silently dropped.
// Status/limits: TRANSCRIBED; LINKABLE (the file is self-contained: it calls nothing and defines the one global it loads). STRUCTURALLY_VERIFIED against the 2-instruction listing and the 19-site call census; BUILD_VERIFIED (i686-w64-mingw32-g++ -std=c++20 -O2 -Wall -Wextra single-file compile); runtime UNOBSERVED. What 0x0057BAA8 holds and why 19 callers read it is not established by this body - the name stays address-suffixed until something does. Domain: core/, because every one of its 19 callers sits in the 0x00402930/0x00449950 core startup area.

#include <cstdint>

// The project's NATIVE_DECL expands to the retail calling convention; task_types.hpp defines and then #undefs it, so it is
// re-established here for this file's own definition (the same guarded block the other retail transcriptions carry).
#ifndef NATIVE_DECL
#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif
#endif

extern "C" {

// 0x0057BAA8: the global this getter returns. Zero-fill in the image (the VA has no file-backed bytes), so it is
// populated at runtime; the image's initial value is therefore 0.
std::uint32_t g_57baa8 = 0U;

std::uint32_t NATIVE_DECL Sub_4168D0(void* object)
{
    (void)object; // 0x004168D0: the body is `mov eax,[0x57BAA8]` - no argument is read.
    return ::g_57baa8; // 0x004168D3: ret.
}

} // extern "C"

#undef NATIVE_DECL
