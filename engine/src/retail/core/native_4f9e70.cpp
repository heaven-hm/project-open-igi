// Native: GraphStream_HashName_4F9E70; retail VA 0x004F9E70 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004F9E70-0x004F9EAC (60 bytes, 6 blocks, 23 instructions).
// Tools/evidence: build/retail-dis.txt (GNU objdump) 0x004F9E70-0x004F9EAC; radare2 6.2.0 afij (60 bytes, 6 blocks, 23 instructions); called by 0x004F9DF0 at 0x004F9DF6.
// Behavior/CFG: six blocks, one loop. Folds a NUL-terminated name into a 32-bit hash: ASCII 'a'..'z' (signed compare, so bytes >= 0x80 are not folded) are shifted by +0xE0 first, each character is sign-extended and multiplied by a 16-bit multiplier that starts at 1 and grows by 0x13D (masked to 16 bits) per character, and the products are summed with 32-bit wrap.
// Status/limits: TRANSCRIBED; STRUCTURALLY_VERIFIED against the 23-instruction listing; runtime UNOBSERVED. No call, no global, no store; the only argument is the name pointer and the return value is the hash.
#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

std::uint32_t NATIVE_DECL GraphStream_HashName_4F9E70(char const* name)
{
    std::uint32_t hash = 0U;                                                  // 0x004F9E75: xor %eax,%eax
    std::uint32_t multiplier = 1U;                                            // 0x004F9E77: mov $0x1,%edx
    signed char character = static_cast<signed char>(*name);                  // 0x004F9E7C: mov (%esi),%cl
    while (character != 0)                                                    // 0x004F9E7E/0x004F9E80 and 0x004F9EA6/0x004F9EA8
    {
        ++name;                                                               // 0x004F9E82: inc %esi
        if (character >= 0x61 && character <= 0x7A)                           // 0x004F9E83..0x004F9E8B: signed cmp 'a'/'z'
        {
            character = static_cast<signed char>(character + 0xE0);           // 0x004F9E8D: add cl,0xe0 (= subtract 0x20)
        }
        // 0x004F9E90: movsbl %cl,%ecx; 0x004F9E93: imul %edx,%ecx; 0x004F9E96: add %ecx,%eax.
        hash += static_cast<std::uint32_t>(static_cast<std::int32_t>(character)) * multiplier;
        character = static_cast<signed char>(*name);                          // 0x004F9E98: mov (%esi),%cl
        multiplier = (multiplier + 0x13DU) & 0xFFFFU;                         // 0x004F9E9A..0x004F9EA0: add $0x13d,%edx; and $0xffff,%edx
    }
    return hash;                                                              // 0x004F9EAA: pop %esi; 0x004F9EAB: ret
}
} // extern "C"

#undef NATIVE_DECL
