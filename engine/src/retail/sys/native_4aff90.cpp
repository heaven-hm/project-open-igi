// Native: Sys_InitRecordDefaults_4AFF90 (behavior-derived placeholder name; retail symbol unknown); retail VA 0x004AFF90.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004AFF90-0x004AFFAC (29 bytes, 1 block, 0 edges, 9 instructions: 57 / 8b7c2408 / b940000000 / b801010101 / f3ab / 8b44240c / 5f / c70001000000 / c3; the next byte 0x004AFFAD is nop padding).
// Tools/evidence: radare2 6.2.0 af/afbj/pdf and the raw bytes decode as `push edi`, `mov edi,[esp+8]` (first argument), `mov ecx,0x40`, `mov eax,0x01010101`, `rep stos dword [edi]` (0x40 dwords, i.e. 256 bytes each set to 0x01), `mov eax,[esp+0xc]` (second argument), `pop edi`, `mov dword [eax],1`, `ret`. r2 afvd recovers two arguments (arg_8h, arg_ch), matching the push-adjusted slots. This routine is called at 0x004AFA1E from 0x004AF9D0, whose complete callee map is just this address and 0x004AFFB0; sys/native_4f1db0.cpp also records reaching it from the list-record path.
// Behavior/CFG: fills 256 bytes at the first argument with 0x01 and then stores 1 in the dword at the second argument; one block, no branch, no loop, no call, no frame beyond the single saved register.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. The fill is 0x40 dwords of 0x01010101, so every one of the 256 bytes becomes 0x01, transcribed as std::memset(buffer, 0x01, 0x100) following the tree's convention for this idiom (core/native_00498480.cpp transcribes the same rep stos form the same way). The 256-byte extent and the flag are what the instructions show, not a declared C++ type: this body does not establish the caller's object layout or what the flag controls, and the second argument's store is unconditional. Domain: sys/, because 0x004AF960 and 0x004AF8F0, its neighbours in the same list-record family, are transcribed there. Codegen note: GCC does not emit retail's single `rep stos` for a constant 0x100-byte fill - it compiles std::memset to a two-way unrolled loop (`movl $0x1010101,0xf8(%edx)`, `movl $0x1010101,0xfc(%edx)`, then a body storing at (%edx,%eax) and 0x4(%edx,%eax) with `add $0x8,%eax; cmp $0xf8,%eax; jb`), 17 instructions and three blocks where retail has 9 instructions and one block. The bytes written are identical - all 256 become 0x01 - and the flag store is unchanged; core/native_00498480.cpp transcribes the same rep stos idiom with std::memset, so this follows the tree's convention rather than fighting the optimizer.
#include <cstdint>
#include <cstring>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

void NATIVE_DECL Sys_InitRecordDefaults_4AFF90(void* buffer, std::uint32_t* flag)
{
    std::memset(buffer, 0x01, 0x100);   // 0x004AFF95-0x004AFF9F: mov ecx,0x40; mov eax,0x01010101; rep stos
    *flag = 1U;                         // 0x004AFFA6: mov dword [eax],1
}

} // extern "C"

#undef NATIVE_DECL
