// Native: String_FindCharacter_4AA0A0; retail VA 0x004AA0A0 (placeholder name; retail symbol unknown; the catalog record for the inner entry 0x004AA0A6 is named MathSys_Invoke19, a harvest label that this body does not adopt).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; the code occupies 0x004AA090-0x004AA15B (204 bytes); callers enter at the six-byte adaptor 0x004AA0A0, the body proper begins at 0x004AA0A6 and the first-byte match tail sits *before* the entry at 0x004AA090, all reached from the body.
// Tools/evidence: radare2 6.2.0 af/afij/pdf and the raw bytes (33c0 8a442408 ... 8d42ff 5b c3), cross-decoded with NASM ndisasm; catalog record 0x004AA0A6 in docs/fidelity/native-verification.json plus docs/fidelity/evidence/0x004AA0A6-*.json.
// Behavior/CFG: strchr-shaped CRT helper, x86 cdecl (const char* text, int32_t character) -> const char*. Zero-extends the character byte, walks the unaligned prefix byte by byte, then scans whole dwords with the 0x7efefeff / 0x81010100 null-byte and repeated-character trick and tests the four bytes of each suspected dword in order. Returns the address of the first matching byte or null. No global, no callee.
// Status/limits: STRUCTURALLY_VERIFIED; runtime UNOBSERVED. The trailing block at 0x004AA15C (push 4; push 0; push [esp+0xc]; call 0x004AA16D) is not reachable from this code and starts the next routine, so it is outside the extent. The catalog's 0x004AA0A6 candidate covers only the inner entry and not the 0x004AA0A0 adaptor or the 0x004AA090 tail, so it cannot be reused as-is. radare2 reports 25 blocks / 35 edges for the region entered at 0x004AA0A0.
#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

const char* NATIVE_DECL String_FindCharacter_4AA0A0(const char* text, std::int32_t character)
{
    const std::uint32_t value = static_cast<std::uint8_t>(character);  // 0x004AA0A0-0x004AA0A2: xor eax,eax; mov al,[esp+8]
    const std::uint8_t byte_value = static_cast<std::uint8_t>(value);  // 0x004AA0A7: mov ebx,eax (bl = the searched byte)
    const std::uint8_t* cursor = reinterpret_cast<const std::uint8_t*>(text); // 0x004AA0AC: mov edx,[esp+8]
    const std::uint32_t shifted = value << 8U;                         // 0x004AA0A9: shl eax,8

    if ((reinterpret_cast<std::uintptr_t>(cursor) & 3U) != 0U)         // 0x004AA0B0-0x004AA0B6: test edx,3; jz 0x004AA0CB
    {
        for (;;)
        {
            const std::uint8_t current = *cursor;                      // 0x004AA0B8: mov cl,[edx]
            ++cursor;                                                  // 0x004AA0BA: inc edx
            if (current == byte_value)                                 // 0x004AA0BB-0x004AA0BD: cmp cl,bl; jz 0x004AA090
            {
                return reinterpret_cast<const char*>(cursor) - 1;      // 0x004AA090-0x004AA094: lea eax,[edx-1]; pop ebx; ret
            }
            if (current == 0U)                                         // 0x004AA0BF-0x004AA0C1: test cl,cl; jz 0x004AA114
            {
                return nullptr;                                        // 0x004AA114-0x004AA117: pop ebx; xor eax,eax; ret
            }
            if ((reinterpret_cast<std::uintptr_t>(cursor) & 3U) == 0U) // 0x004AA0C3-0x004AA0C9: test edx,3; jnz 0x004AA0B8
            {
                break;
            }
        }
    }

    const std::uint32_t repeated = value | shifted;                    // 0x004AA0CB: or ebx,eax
    const std::uint32_t pattern = repeated | (repeated << 16U);        // 0x004AA0CE-0x004AA0D4: mov eax,ebx; shl ebx,0x10; or ebx,eax

    for (;;)                                                           // 0x004AA0D6: mov ecx,[edx]
    {
        const std::uint32_t word = *reinterpret_cast<const std::uint32_t*>(cursor); // 0x004AA0D6
        const std::uint32_t mixed = word ^ pattern;                    // 0x004AA0E1: xor ecx,ebx
        const std::uint32_t word_scan = 0x7efefeffU + word;            // 0x004AA0DD/0x004AA0E3: mov eax,ecx; add esi,eax
        const std::uint32_t mixed_scan = 0x7efefeffU + mixed;          // 0x004AA0D8/0x004AA0E5: mov edi,0x7efefeff; add edi,ecx
        const std::uint32_t word_test = (~mixed) ^ mixed_scan;         // 0x004AA0E7/0x004AA0ED: xor ecx,-1; xor ecx,edi
        const std::uint32_t zero_test = (~word) ^ word_scan;           // 0x004AA0EA/0x004AA0EF: xor eax,-1; xor eax,esi
        cursor += 4U;                                                  // 0x004AA0F1: add edx,4

        if ((word_test & 0x81010100U) != 0U)                           // 0x004AA0F4-0x004AA0FA: and ecx,0x81010100; jnz 0x004AA118
        {
            const std::uint32_t bytes =                                // 0x004AA118: mov eax,[edx-4]
                *reinterpret_cast<const std::uint32_t*>(cursor - 4U);
            const std::uint8_t first = static_cast<std::uint8_t>(bytes);
            if (first == byte_value)                                   // 0x004AA11B-0x004AA11D: cmp al,bl; jz 0x004AA155
            {
                return reinterpret_cast<const char*>(cursor) - 4;      // 0x004AA155-0x004AA15B: lea eax,[edx-4]; pop esi; pop edi; pop ebx; ret
            }
            if (first == 0U)                                           // 0x004AA11F-0x004AA121: test al,al; jz 0x004AA112
            {
                return nullptr;
            }
            const std::uint8_t second = static_cast<std::uint8_t>(bytes >> 8U);
            if (second == byte_value)                                  // 0x004AA123-0x004AA125: cmp ah,bl; jz 0x004AA14E
            {
                return reinterpret_cast<const char*>(cursor) - 3;      // 0x004AA14E-0x004AA154: lea eax,[edx-3]; pop esi; pop edi; pop ebx; ret
            }
            if (second == 0U)                                          // 0x004AA127-0x004AA129: test ah,ah; jz 0x004AA112
            {
                return nullptr;
            }
            const std::uint8_t third = static_cast<std::uint8_t>(bytes >> 16U);
            if (third == byte_value)                                   // 0x004AA12B-0x004AA130: shr eax,0x10; cmp al,bl; jz 0x004AA147
            {
                return reinterpret_cast<const char*>(cursor) - 2;      // 0x004AA147-0x004AA14D: lea eax,[edx-2]; pop esi; pop edi; pop ebx; ret
            }
            if (third == 0U)                                           // 0x004AA132-0x004AA134: test al,al; jz 0x004AA112
            {
                return nullptr;
            }
            const std::uint8_t fourth = static_cast<std::uint8_t>(bytes >> 24U);
            if (fourth == byte_value)                                  // 0x004AA136-0x004AA138: cmp ah,bl; jz 0x004AA140
            {
                return reinterpret_cast<const char*>(cursor) - 1;      // 0x004AA140-0x004AA146: pop esi; pop edi; lea eax,[edx-1]; pop ebx; ret
            }
            if (fourth == 0U)                                          // 0x004AA13A-0x004AA13C: test ah,ah; jz 0x004AA112
            {
                return nullptr;
            }
            continue;                                                  // 0x004AA13E: jmp 0x004AA0D6
        }

        if ((zero_test & 0x81010100U) == 0U)                           // 0x004AA0FC-0x004AA101: and eax,0x81010100; jz 0x004AA0D6
        {
            continue;
        }
        if ((zero_test & 0x01010100U) != 0U)                           // 0x004AA103-0x004AA108: and eax,0x1010100; jnz 0x004AA112
        {
            return nullptr;
        }
        if ((word_scan & 0x80000000U) == 0U)                           // 0x004AA10A-0x004AA110: and esi,0x80000000; jz 0x004AA112
        {
            return nullptr;
        }
    }
}

} // extern "C"

#undef NATIVE_DECL
