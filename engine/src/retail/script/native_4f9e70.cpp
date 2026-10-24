// Native: Script_StringHash (placeholder until VERIFIED); retail VA 0x004F9E70-0x004F9EAB (60 bytes, 6 blocks).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 disassembly (56 8b742408 33c0 ba01000000 8a0e 84c9 7428 46 80f961 7c08 80f97a 7f03 80c1e0 0fbec9 0fafca 03c1 8a0e 81c23d010000 81e2ffff0000 84c9 ...); no callees.
// Behavior/CFG: lower-case folding hash over a C string: bitwise hash += (signed char)c * multiplier, multiplier = (multiplier + 0x13D) & 0xFFFF per character; characters in 'a'..'z' have 0xE0 added first; stops at NUL. Returns the accumulated 32-bit hash.
// Status/limits: TRANSCRIBED (static only); the lower-case fold, the 0x13D increment masked to 16 bits and the signed byte multiply are retail; runtime UNOBSERVED.
#include <cstdint>

extern "C" {

std::int32_t Script_StringHash(char const* text)
{
    std::int32_t hash = 0;                                   // 0x004F9E75: xor eax,eax
    std::uint32_t multiplier = 1;                            // 0x004F9E77: mov edx,1
    char c = *text;                                          // 0x004F9E7C: mov cl,[esi]
    while (c != 0) {                                         // 0x004F9E7E: test cl,cl; je
        text += 1;                                           // 0x004F9E82: inc esi
        if (c >= 'a' && c <= 'z')                            // 0x004F9E83..0x004F9E8B
            c = static_cast<char>(c + 0xE0);                 // 0x004F9E8D: add cl,0xe0
        hash += static_cast<std::int32_t>(c) * static_cast<std::int32_t>(multiplier); // 0x004F9E90..0x004F9E96: movsx; imul; add
        c = *text;                                           // 0x004F9E98: mov cl,[esi]
        multiplier = (multiplier + 0x013DU) & 0xFFFFU;       // 0x004F9E9A..0x004F9EA0: add edx,0x13d; and edx,0xffff
    }
    return hash;                                             // 0x004F9EAA: pop esi; ret
}

} // extern "C"
