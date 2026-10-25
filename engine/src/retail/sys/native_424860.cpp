// Native: Sys_GetGlobal32_424860 (placeholder name; retail symbol unknown). retail VA 0x00424860-0x00424865 (6 bytes, 1 block, 0 edges, 2 instructions).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 `pd` at 0x00424860 plus capstone 5.0.7 aligned from the verified entry; the two listings agree. The body is `a1 10 b9 53 00` = `mov eax,[0x0053B910]` at 0x00424860 and `c3` = `ret` at 0x00424865, so the extent is 6 bytes. The ten bytes 0x00424866-0x0042486F are 0x90 alignment padding and are NOT part of this routine; 0x00424870 is the next getter (`mov eax,[0x0053B914]; ret`), which is a separate native. No frame, no argument read, no branch, no callee, no store. Only EAX is written, so the declared return type is exactly the access width.
// Behavior/CFG: one straight-line block returning the 32-bit object at 0x0053B910. Its only CALL site in the image is 0x00417669 (`e8 f2 d1 00 00` inside the routine at 0x00417660, which tests its argument for zero and then uses the result in `lea eax,[eax+eax*2]`). 0x004247EA is a direct inline read of the same slot (`mov eax,[0x0053B910]`), not a call to this native; the routine that contains 0x004247EA is untranscribed and I have not established its entry address, so no boundary is claimed for it here. A caller census over 0x00401000-0x00532510 that decodes every 0xE8 as v+5+rel32 finds exactly one call to 0x00424860.
// Status/limits: TRANSCRIBED; LINKABLE (no callee, no handler, no undefined symbol of its own beyond the slot it reads). STRUCTURALLY_VERIFIED for the CFG only, which is one block with no edges. BUILD_VERIFIED (i686-w64-mingw32-g++ -std=c++20 -O2 -Wall -Wextra single-file compile clean). Runtime UNOBSERVED in both binaries. The value's meaning is not established by this body; the access width is the only proven fact about the object, and this file is not its defining file.
// Ownership note (per the Lead's two-variable trap): 0x0053B910 is DEFINED by engine/src/retail/core/native_423fc0.cpp, which writes it at 0x00423FD7 and is its only writer in .text. This file therefore DECLARES it as a C-linkage extern and must never define it: a second definition here would give the tree two distinct objects with no link error, and this getter would keep reading the copy nothing writes. If the definition ever moves out of native_423fc0.cpp, it must move as one change together with this declaration, or the slot goes silent. The image value is 0x000000FF.
// Domain: sys/, not core/. Rule 15 assigns the folder by subsystem, and this routine is the argument-less global getter of the same 0x0053B9xx block as 0x00424850, 0x00424870, 0x00424880, 0x00424890 and 0x00424B80 - all five of which already live in engine/src/retail/sys/ with this exact shape (`mov <reg>,[abs]; ret`, no arguments, no frame). Its band owner is core/ because the band is address-based; its own shape and its five siblings put it in sys/. Flagged for the Lead rather than left implicit.
#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// 0x0053B910: the four-byte object this getter reads. DEFINED in engine/src/retail/core/native_423fc0.cpp
// (0x00423FD7 is its only writer); declared here, not defined.
extern std::uint32_t g_53b910;

std::uint32_t NATIVE_DECL Sys_GetGlobal32_424860(void)
{
    return ::g_53b910;                                                       // 0x00424860: mov 0x0053b910,%eax; 0x00424865: ret
}

} // extern "C"

#undef NATIVE_DECL
