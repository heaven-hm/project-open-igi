// Native: Sub_4E5360 (address-suffixed placeholder; no manifest record for 0x004E5360); retail VA 0x004E5360-0x004E5365.
// Artifact: retail igi.exe SHA-256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; image base 0x00400000; .text VA 0x00401000 raw 0x1000.
// Tools/evidence: capstone 5.0.7 and ndisasm agree exactly: 2 instructions, 6 bytes, contiguous, extent 0x004E5360-0x004E5366. Bytes A0 5E 87 54 00 C3 = `mov al,byte ptr [0x54875e]` then `ret`; two 0x90 padding bytes follow. No branch, no call, no stack access, so the body is a leaf byte getter and its extent is not ambiguous.
// Behavior/CFG: returns the byte at 0x0054875E in AL, nothing else. Single block, no edges, no calls, no writes. The global is file-backed .data with image value 0xFF. The retail instruction is `mov al,[...]`, which replaces only AL and leaves EAX's upper 24 bits as they were; the one caller (core/native_52c0c0.cpp, `call 0x004E5360` at 0x0052C181) pushes the result as an event id, and 0x00401400 masks the event word to its low byte, so those leftover upper bits cannot be observed. The function reads NO stack argument: there is no [esp+N] access anywhere in the body, so the descriptor pointer the caller pushes (0x0052BFE0) is ignored. The one unnamed parameter below records that the call site pushes one dword; it is not read.
// Status/limits: TRANSCRIBED; BUILD_VERIFIED (i686-w64-mingw32-g++ -std=c++20 -O2 -Wall -Wextra single-file compile); STRUCTURALLY_VERIFIED against the 2-instruction listing with both disassemblers agreeing; runtime UNOBSERVED. LIMITS: what 0x0054875E means is not established by this body - it is read, never written here, and its image value is 0xFF. Its writer is outside this function. Domain: core/, because its only caller is core/native_52c0c0.cpp.

#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// 0x0054875E is file-backed .data, image value 0xFF.
std::uint8_t NATIVE_DECL Sub_4E5360(std::uint32_t)
{
    return *reinterpret_cast<const std::uint8_t*>(0x0054875EU); // 0x004E5360: mov al,byte ptr [0x54875e]; 0x004E5365: ret
}

} // extern "C"
