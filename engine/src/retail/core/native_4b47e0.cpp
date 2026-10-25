// Native: Sub_4B47E0 (address-suffixed placeholder; no manifest record for 0x004B47E0); retail VA 0x004B47E0-0x004B4811.
// Artifact: retail igi.exe SHA-256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; image base 0x00400000; .text VA 0x00401000 raw 0x1000.
// Tools/evidence: capstone 5.0.7 from entry 0x004B47E0: 20 instructions, 50 bytes, contiguous, extent 0x004B47E0-0x004B4812, one `ret` at 0x004B4811, ZERO conditional branches, one call. crosscheck_ndisasm(0x004B47E0) reports (20, 20, True, 'agree'). ARGUMENTS FROM THE STACK, NOT FROM PUSH COUNTS: the body saves esi (push at 0x004B47E0), so at 0x004B47E1 esp is E-4 and `mov esi,[esp+8]` reads E+4 = argument 1; after the call at 0x004B47E6 esp is back to E-4, so `mov eax,[esp+0x10]` at 0x004B47EB reads E+8 = argument 2. Two arguments, both dwords. The `add esp,4` at 0x004B4805 is the caller-side cleanup of the one push made for the inner call, not an argument of this function.
// Behavior/CFG: a three-word mixer over the object at argument 1, seeded by argument 2. It first calls 0x004B4720 with the SAME pointer (argument 1) to store the three initial words at +0x00, +0x04 and +0x08, then transforms them in place: word0 ^= value; word1 += (value << 15); word2 -= (value << 9). Single block, no edges, no branches. THE RETURN VALUE IS INCIDENTAL BUT EXACT: retail never sets EAX for a return, and after `shl eax,9` at 0x004B4802 nothing else writes EAX before the `ret`, so the caller sees value << 9. The one caller (core/native_4f8340.cpp, `call 0x004B47E0` at 0x004F835A) ignores the result entirely, so this is unobservable either way; it is reproduced rather than dropped.
// Status/limits: TRANSCRIBED; BUILD_VERIFIED (i686-w64-mingw32-g++ -std=c++20 -O2 -Wall -Wextra single-file compile); STRUCTURALLY_VERIFIED against the 20-instruction listing with both disassemblers agreeing; runtime UNOBSERVED. LIMITS: (a) what the three words mean and where 0x004B4720 gets them is not established by this body - it is 0x004B4720's behavior, transcribed elsewhere in the tree; this body only transforms what that call leaves. (b) The caller passes 0x19111911 as the value and 0x00A76CD0 as the target, a zero-fill global. Domain: core/, because its only caller is core/native_4f8340.cpp.

#include <cstdint>

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

// 0x004B4720: 1 slot (push esi 0x004B47E5). Named from its manifest record; defined elsewhere in the tree.
void Sys_StoreThreeWords_4B4720(void* target);

std::uint32_t NATIVE_DECL Sub_4B47E0(void* target, std::uint32_t value)
{
    std::uint32_t* const words = reinterpret_cast<std::uint32_t*>(target);
    Sys_StoreThreeWords_4B4720(target); // 0x004B47E5-0x004B47E6: push esi; call 0x4b4720
    words[0] = words[0] ^ value;             // 0x004B47EF-0x004B47F6: ecx=[esi]; ecx^=eax; [esi]=ecx
    words[1] = words[1] + (value << 15);     // 0x004B47F1,0x004B47F8,0x004B47FA,0x004B47FD,0x004B480A: edx=[esi+4]; ecx=eax<<15; edx+=ecx; [esi+4]=edx
    words[2] = words[2] - (value << 9);      // 0x004B47FF,0x004B4802,0x004B4808,0x004B480D: ecx=[esi+8]; eax<<=9; ecx-=eax; [esi+8]=ecx
    return value << 9U; // 0x004B4811: ret with EAX left as value<<9 by 0x004B4802 - incidental, reproduced exactly
}

} // extern "C"

#undef NATIVE_DECL
