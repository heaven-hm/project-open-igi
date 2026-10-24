// Native: Script_Hash32_4C0530 (placeholder until VERIFIED); retail VA 0x004C0530-0x004C0557 (39 bytes, 3 blocks).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 full listing (8b542404 33c0 8a0a 84c9 7412 81e1ff000000 42 c1e005 03c1 8a0a 84c9 75ee 33d2 div ...); called from 0x4C04B0 as hash(value, [list+4]); no callees.
// Behavior/CFG: hash = (hash << 5) + (byte & 0xFF) for each byte up to NUL (entry test then loop tail test, matching retail's two exit checks), then returns hash % bucketCount by the same unsigned divide retail uses with edx cleared.
// Status/limits: TRANSCRIBED (static only); shift/and order and the unsigned modulo follow retail; runtime UNOBSERVED.
#include <cstdint>

extern "C" {

std::int32_t Script_Hash32_4C0530(char const* text, std::int32_t const bucket_count)
{
    std::uint32_t hash = 0;                                 // 0x004C0534: xor eax,eax
    char c = *text;                                         // 0x004C0536: mov cl,[edx]
    if (c != 0) {                                           // 0x004C0538: test cl,cl; je
        do {
            hash = (hash << 5) + (static_cast<std::uint32_t>(c) & 0xFFU); // 0x004C053C..0x004C0546
            text += 1;                                      // 0x004C0542: inc edx
            c = *text;                                      // 0x004C0548: mov cl,[edx]
        } while (c != 0);                                   // 0x004C054A: test cl,cl; jne
    }
    std::uint32_t const divisor = static_cast<std::uint32_t>(bucket_count); // 0x004C054E: xor edx,edx; div
    return static_cast<std::int32_t>(hash % divisor);        // remaining edx returned
}

} // extern "C"
