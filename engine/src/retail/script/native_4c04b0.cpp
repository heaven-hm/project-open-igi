// Native: Script_QHashNodeLookup_4C04B0 (placeholder until VERIFIED); retail VA 0x004C04B0-0x004C052F (128 bytes, 12 blocks).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 complete listing (prologue 53 8b5c2410 55 56 8b742410 57 8b4604 50 53 e8.. 8b16 8d0c40 83c408 8b2c8a 8b7d00 85ff; tails 8a10 8aca 3a16 751c 84c9 7414 8a5001 8aca 3a5601 750e 83c002 83c602 84c9 75e0 / 33c0 eb05 / 1bc0 83d8ff / 85c0 7419 8bef 8b3f 85ff 75c6 / 8b4c2418 ... c70100000000 32c0 / 8b442418 ... 8928 b001); three call sites confirm 3 args (list, out, value); callee 0x4C0530 landed.
// Behavior/CFG: hashes value with 0x4C0530 against the bucket count at list+4, selects the 12-byte-stride bucket, takes node = bucket[3*hash], next = node->next, and if next is null reports a miss (*out = null, return 0). Otherwise it compares value against node->key (node+8) with retail's 2-byte unrolled byte compare and sbb sign trick, and on equality stores node into *out and returns 1; on inequality it advances node = next, next = next->next and repeats while next is non-null, else reports a miss.
// Status/limits: TRANSCRIBED (static only); retail quirks preserved - the guard tests next before comparing node, and a null next after advancing goes straight to the miss path without comparing the new node; runtime UNOBSERVED.
#include <cstdint>

extern "C" std::int32_t Script_Hash32_4C0530(char const* text, std::int32_t bucket_count);

extern "C" {

std::int32_t Script_QHashNodeLookup_4C04B0(void* const list, void** const out, char const* const value)
{
    std::int32_t const bucket_count = *reinterpret_cast<std::int32_t*>(static_cast<std::uint8_t*>(list) + 4); // 0x004C04BC
    std::int32_t const index = Script_Hash32_4C0530(value, bucket_count);                                   // 0x004C04C1
    std::uint8_t* const buckets = *reinterpret_cast<std::uint8_t**>(list);                                  // 0x004C04C6
    std::int32_t const slot = index * 3;                                                                    // 0x004C04C8: lea ecx,[eax+eax*2]
    std::uint8_t* node = *reinterpret_cast<std::uint8_t**>(buckets + slot * 4);                             // 0x004C04CE
    std::uint8_t* next = *reinterpret_cast<std::uint8_t**>(node);                                           // 0x004C04D1
    if (next == nullptr) {                                                                                  // 0x004C04D4: test edi,edi; je
        *out = nullptr;                                                                                     // 0x004C0519: mov dword [ecx],0
        return 0;                                                                                           // 0x004C0521: xor al,al
    }
    for (;;) {
        char const* key = *reinterpret_cast<char const**>(node + 8);         // 0x004C04D8: mov esi,[ebp+8]
        char const* left = value;                                            // 0x004C04DB: mov eax,ebx
        std::int32_t compare = 0;
        for (;;) {
            std::uint8_t const a0 = static_cast<std::uint8_t>(*left);         // 0x004C04DD: mov dl,[eax]
            std::uint8_t const b0 = static_cast<std::uint8_t>(*key);          // 0x004C04E1: cmp dl,[esi]
            if (a0 != b0) {                                                   // 0x004C04E3: jne
                compare = a0 < b0 ? -1 : 1;                                   // 0x004C0501: sbb eax,eax; sbb eax,0xFFFFFFFF
                break;
            }
            if (a0 == 0) { break; }                                           // 0x004C04E7: test cl,cl; je -> xor eax,eax
            std::uint8_t const a1 = static_cast<std::uint8_t>(*(left + 1));   // 0x004C04E9: mov dl,[eax+1]
            std::uint8_t const b1 = static_cast<std::uint8_t>(*(key + 1));    // 0x004C04EE: cmp dl,[esi+1]
            if (a1 != b1) {                                                   // 0x004C04F1: jne
                compare = a1 < b1 ? -1 : 1;
                break;
            }
            left += 2;                                                        // 0x004C04F3: add eax,2
            key += 2;                                                         // 0x004C04F6: add esi,2
            if (a1 == 0) { break; }                                           // 0x004C04F9: test cl,cl; jne loop; else equal
        }
        if (compare == 0) {                                                   // 0x004C0506: test eax,eax; je
            *out = node;                                                      // 0x004C0529: mov [eax],ebp
            return 1;                                                         // 0x004C052C: mov al,1
        }
        node = next;                                                          // 0x004C050A: mov ebp,edi
        next = *reinterpret_cast<std::uint8_t**>(next);                       // 0x004C050C: mov edi,[edi]
        if (next == nullptr) {                                                // 0x004C050E: test edi,edi; jne loop
            *out = nullptr;                                                   // 0x004C0519
            return 0;                                                         // 0x004C0521
        }
    }
}

} // extern "C"
