// Native: Sys_TranslateWildcardPattern_4AFFB0 (behavior-derived placeholder name; retail symbol unknown); retail VA 0x004AFFB0.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004AFFB0-0x004B01EF (576 bytes, 41 blocks, 187 instructions, 97 edges; a 0x414-byte frame plus four saved registers, and the byte at 0x004B01F0 begins the dispatch data the switch compiles to).
// Tools/evidence: radare2 6.2.0 af/afbj/pdf over the exact image. The head dispatches on one character: `mov dl,[ecx+edx]`, `movsbl dl,ebx`, `lea ebp,[ebx-0x28]`, `cmp ebp,0x55`, `ja 0x4b0083`, then `mov bl,[0x4b0218+ebp]` and `jmp *[0x4b01f0+ebx*4]`. Both tables were read from the image: the 86-byte class table at 0x004B0218 covers characters 0x28-0x7D and assigns class 0 to "(", 1 to ")", 2 to "*", 3 to "+", 4 to "/" and "\", 5 to "?", 6 to "[", 7 to "{", 8 to "}" and 9 to the remaining in-range characters including "]"; the ten-entry jump table at 0x004B01F0 maps class 0..9 to 0x004B0076, 0x004B00B3, 0x004B0176, 0x004B00CE, 0x004B000B, 0x004B017C, 0x004B0022, 0x004B0197, 0x004B01B8 and 0x004B0083. The body makes no calls at all - zero direct and zero indirect call forms in 187 instructions - so its closure is empty. It is called at 0x004AF9E7 from 0x004AF9D0, whose second argument supplies the pattern and whose local at 0x130(%esp) supplies the destination.
// Behavior/CFG: rewrites the wildcard pattern in the second argument into the buffer in the first, returning 1 on success and 0 on failure; one outer loop tested at 0x004B008E, a ten-way switch, a group-index stack at [esp+0x24+n*4] with depth in [esp+0x14], and shared tails at 0x004B0089 (copy the current byte, advance both indexes) and 0x004B0185 (same plus clearing [esp+0x10]). "(" pushes the output index; ")" fails at depth zero and otherwise pops; "*" and "?" fail while the metacharacter flag is set; "+" fails while it is set, and otherwise either duplicates the group body and appends a literal "*", or when [esp+0x10] is zero appends "*" after copying from the saved index; "[" copies the bracket contents verbatim until "]" and fails on NUL; "{" requires depth zero and the flag clear then sets it; "}" requires the flag set; "/" and "\" copy the separator and then the next byte through the shared tail; the default class copies the byte. Failure returns 0 at 0x004B01CF; an empty pattern writes only the terminator and returns 1 at 0x004B01DC.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. Two arguments: the destination byte buffer (read at 0x004AFFC2 into esi) and the pattern byte string (read at 0x004AFFB6 into edx, re-read at 0x004AFFE3 and 0x004B008E from the push-shifted slot); returns 1 at 0x004B00A6 and 0x004B01E3 and 0 from 0x004B01D2. The frame slots keep retail offsets as names because only some roles are established: [esp+0x10] is both a flag and, at 0x004B013C-0x004B0152, a source pointer; [esp+0x14] is the group depth; [esp+0x18] a second flag; [esp+0x1C] the copy counter; [esp+0x20] the saved output index. The prologue clears only [esp+0x14] (0x004AFFCE) and [esp+0x18] (0x004AFFD9); [esp+0x10], [esp+0x1C] and [esp+0x20] start indeterminate in retail, and the zero initializers here are behaviour-preserving rather than invented: [esp+0x10] is read at 0x004B00D6 only when the flag is zero, and the flag is zeroed only in paths that also write it; a non-zero [esp+0x10] can only come from 0x004B0062 in the "[" case, which also writes [esp+0x20] at 0x004B0025 first. The index stack has NO bounds check in retail - 0x004B007A stores through it after only an increment - so the declared 252 entries are the frame's remaining capacity (0x414-0x24 bytes, four per entry) and no check was added. The switch is written over the characters the class table groups rather than through the table, which is the source form of the same dispatch and keeps every case body, edge and the 0x004B0083 default on its retail block; the case order follows the class numbering, the only order the tables establish. Domain: sys/, because 0x004AF9D0, its only caller, sits with 0x004AF960 and 0x004AF8F0 in the same list-record family.
#include <cstdint>
#include <cstddef>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

std::int32_t NATIVE_DECL Sys_TranslateWildcardPattern_4AFFB0(std::uint8_t* destination, char const* pattern)
{
    std::uint32_t out = 0;           // eax: output index
    std::uint32_t in = 0;            // ecx: pattern index
    std::uint32_t flag = 1;          // edi: 1 at entry, 0 once a literal has been copied
    std::uintptr_t local_10 = 0;     // [esp+0x10]: a flag, and a source pointer at 0x004B013C-0x004B0152
    std::uint32_t local_14 = 0;      // [esp+0x14]: group depth, cleared by the prologue
    std::uint32_t local_18 = 0;      // [esp+0x18]: second flag, cleared by the prologue
    std::uint32_t local_1c = 0;      // [esp+0x1C]: copy counter
    std::uint32_t local_20 = 0;      // [esp+0x20]: saved output index
    std::uint32_t group_index[0xFC]; // [esp+0x24+n*4]: the frame's remaining capacity, stored to without a bounds check

    char c = pattern[0];                        // 0x004AFFC0: mov bl,[edx]
    if (c == 0)                                 // 0x004AFFD2: test bl,bl
    {
        destination[out] = 0;                   // 0x004B01DC: mov byte [eax+esi],0
        return 1;                               // 0x004B01E3: mov eax,1
    }

    for (;;)
    {
        c = pattern[in];                        // 0x004AFFEA: mov dl,[ecx+edx]
        const std::int32_t signed_c = static_cast<std::int32_t>(static_cast<signed char>(c)); // 0x004AFFED: movsbl dl,ebx

        switch (signed_c)                       // 0x004AFFF0-0x004B0004: range check 0x004AFFF3/0x004AFFF6, class lookup block 0x004AFFFC-0x004AFFFE, jump table 0x004B01F0
        {
        case '(':                               // class 0 - 0x004B0076
            group_index[local_14] = out;        // 0x004B007A: mov [esp+0x24+edi*4],eax
            ++local_14;                         // 0x004B007E/0x004B007F: inc edi; mov [esp+0x14],edi
            flag = 0;                           // 0x004B0083: xor edi,edi
            local_10 = 0;                       // 0x004B0085: mov [esp+0x10],edi
            goto copy_char;                     // 0x004B0089

        case ')':                               // class 1 - 0x004B00B3
            if (local_14 == 0) goto fail;       // 0x004B00B9/0x004B00BF: cmp edi,ebx; je 0x4b01cf
            local_10 = 0;                       // 0x004B00BB: mov [esp+0x10],edi
            --local_14;                         // 0x004B00C5/0x004B00C7/0x004B00C8: mov ebp,ebx; dec ebp; mov [esp+0x14],ebp
            goto copy_char;                     // 0x004B00CC: jmp 0x4b0089

        case '*':                               // class 2 - 0x004B0176
            if (flag != 0) goto fail;           // 0x004B0178: test edi,edi; jne 0x4b01cf
            flag = 1;                           // 0x004B017A/0x004B0180: jmp 0x4b0180; mov edi,1
            goto copy_literal;                  // 0x004B0185

        case '+':                               // class 3 - 0x004B00CE
            if (flag != 0) goto fail;           // 0x004B00D0: jne 0x4b01cf
            if (local_10 == 0) goto plus_after_group; // 0x004B00DC: je 0x4b0112
            {
                const std::uint32_t count = out - local_20; // 0x004B00DE/0x004B00E2/0x004B00E4: ebp=local_20; edi=out; sub edi,ebp
                if (count > 0)                  // 0x004B00E6/0x004B00E8: test edi,edi; jle 0x4b00f8
                {
                    std::uint8_t* source = destination + local_20; // 0x004B00EA: add ebp,esi
                    std::uint32_t remaining = count;               // 0x004B00EC: mov edx,edi
                    do {
                        source[count] = *source;                   // 0x004B00EE/0x004B00F1: mov bl,[ebp]; mov [edi+ebp],bl
                        ++source;                                  // 0x004B00F4: inc ebp
                        --remaining;                               // 0x004B00F5: dec edx
                    } while (remaining != 0);                      // 0x004B00F6: jne 0x4b00ee
                }
                out += count;                   // 0x004B00F8: add eax,edi
            }
            flag = 1;                           // 0x004B00FA: mov edi,1
            local_10 = 0;                       // 0x004B00FF: mov dword [esp+0x10],0
            destination[out] = '*';             // 0x004B0107: mov byte [eax+esi],0x2a
            ++out;                              // 0x004B010B: inc eax
            ++in;                               // 0x004B010C: inc ecx
            goto loop_test;                     // 0x004B010D: jmp 0x4b008e

        case '/':                               // class 4 - 0x004B000B
        case '\\':
            flag = 0;                           // 0x004B000B: xor edi,edi
            destination[out] = static_cast<std::uint8_t>(c); // 0x004B000D: mov [eax+esi],dl
            ++out;                              // 0x004B0017: inc eax
            ++in;                               // 0x004B0018: inc ecx
            local_10 = 0;                       // 0x004B0019: mov [esp+0x10],edi
            c = pattern[in];                    // 0x004B001D: mov dl,[ecx+edx]
            goto copy_char;                     // 0x004B0020: jmp 0x4b0089

        case '?':                               // class 5 - 0x004B017C
            if (flag != 0) goto fail;           // 0x004B017E: test edi,edi; jne 0x4b01cf
            flag = 1;                           // 0x004B0180: mov edi,1
            goto copy_literal;                  // 0x004B0185

        case '[':                               // class 6 - 0x004B0022
            local_20 = out;                     // 0x004B0025: mov [esp+0x20],eax
            if (c == ']') goto bracket_end;     // 0x004B0022/0x004B0029: cmp dl,0x5d; je 0x4b005a
            for (;;)
            {
                if (c == 0) goto fail;          // 0x004B002B/0x004B002D: test dl,dl; je 0x4b01cf
                if (c == '/' || c == '\\')      // 0x004B0033/0x004B0038/0x004B003B: cmp dl,0x2f; cmp dl,0x5c; jne 0x4b0042
                {
                    destination[out] = static_cast<std::uint8_t>(c); // 0x004B003D: mov [eax+esi],dl
                    ++out;                      // 0x004B0040: inc eax
                    ++in;                       // 0x004B0041: inc ecx
                }
                ++out;                          // 0x004B0049: inc eax
                c = pattern[in];                // 0x004B004A: mov dl,[ecx+edi]
                ++in;                           // 0x004B004D: inc ecx
                destination[out - 1] = static_cast<std::uint8_t>(c); // 0x004B004E: mov [eax+esi-1],dl
                c = pattern[in];                // 0x004B0052: mov dl,[ecx+edi]
                if (c != ']') continue;         // 0x004B0055/0x004B0058: cmp dl,0x5d; jne 0x4b002b
                break;
            }
        bracket_end:                            // 0x004B005A
            ++out;                              // 0x004B0061: inc eax
            local_10 = 1;                       // 0x004B0062: mov dword [esp+0x10],1
            c = pattern[in];                    // 0x004B006A: mov dl,[ecx+edx]
            ++in;                               // 0x004B006D: inc ecx
            destination[out - 1] = static_cast<std::uint8_t>(c); // 0x004B006E: mov [eax+esi-1],dl
            flag = 0;                           // 0x004B0072: xor edi,edi
            goto loop_test;                     // 0x004B0074: jmp 0x4b008e

        case '{':                               // class 7 - 0x004B0197
            if (local_14 != 0) goto fail;       // 0x004B019D/0x004B019F: cmp edi,ebx; jne 0x4b01cf
            if (local_18 != 0) goto fail;       // 0x004B01A1/0x004B01A5: cmp edi,[esp+0x18]; jne 0x4b01cf
            local_10 = 0;                       // 0x004B01A7: mov [esp+0x10],edi
            local_18 = 1;                       // 0x004B01AB: mov dword [esp+0x18],1
            goto copy_char;                     // 0x004B01B3: jmp 0x4b0089

        case '}':                               // class 8 - 0x004B01B8
            if (local_18 == 0) goto fail;       // 0x004B01BE/0x004B01C0: cmp edi,ebx; je 0x4b01cf
            local_10 = 0;                       // 0x004B01C2: mov [esp+0x10],edi
            local_18 = 0;                       // 0x004B01C6: mov [esp+0x18],edi
            goto copy_char;                     // 0x004B01CA: jmp 0x4b0089

        default:                                // class 9, and every character outside 0x28-0x7D - 0x004B0083
            flag = 0;                           // 0x004B0083: xor edi,edi
            local_10 = 0;                       // 0x004B0085: mov [esp+0x10],edi
            goto copy_char;                     // 0x004B0089
        }

    copy_char:                                  // 0x004B0089
        destination[out] = static_cast<std::uint8_t>(c); // 0x004B0089: mov [eax+esi],dl
        ++out;                                  // 0x004B008C: inc eax
        ++in;                                   // 0x004B008D: inc ecx
        goto loop_test;                         // falls into 0x004B008E

    copy_literal:                               // 0x004B0185
        destination[out] = static_cast<std::uint8_t>(c); // 0x004B0185: mov [eax+esi],dl
        ++out;                                  // 0x004B0188: inc eax
        ++in;                                   // 0x004B0189: inc ecx
        local_10 = 0;                           // 0x004B018A: mov dword [esp+0x10],0
        goto loop_test;                         // 0x004B0192: jmp 0x4b008e

    loop_test:                                  // 0x004B008E
        if (pattern[in] != 0) continue;         // 0x004B0095/0x004B0099: cmpb [ecx+edx],0; jne 0x4affe3
        destination[out] = 0;                   // 0x004B009F: mov byte [eax+esi],0
        return 1;                               // 0x004B00A6: mov eax,1

    plus_after_group:                           // 0x004B0112
        flag = 1;                               // 0x004B011D: mov edi,1
        if (pattern[in - 1] != ')') goto copy_literal; // 0x004B0119/0x004B0122/0x004B0125: mov bl,[ecx+edi-1]; cmp bl,0x29; jne 0x4b0185
        {
            const std::uint32_t count = out - group_index[local_14]; // 0x004B0127/0x004B012B/0x004B012D/0x004B0131: edx=local_14; ebp=out; edx=group_index[local_14]; sub ebp,edx
            if (count > 0)                      // 0x004B0133/0x004B0135: test ebp,ebp; jle 0x4b0161
            {
                local_10 = reinterpret_cast<std::uintptr_t>(destination + group_index[local_14]); // 0x004B0137/0x004B013C: add edx,esi; mov [esp+0x10],edx
                std::uint8_t* target = destination + out; // 0x004B0139: lea ebx,[eax+esi]
                local_1c = count;               // 0x004B0140: mov [esp+0x1c],ebp
                do {                            // 0x004B0144-0x004B015F
                    *target = *reinterpret_cast<std::uint8_t*>(local_10); // 0x004B0148/0x004B014A: mov dl,[esp+0x10]; mov [ebx],dl
                    ++local_10;                 // 0x004B014C/0x004B0150/0x004B0152: inc edx; mov [esp+0x10],edx
                    ++target;                   // 0x004B0151: inc ebx
                    --local_1c;                 // 0x004B0156/0x004B015A/0x004B015B: mov edx,[esp+0x1c]; dec edx; mov [esp+0x1c],edx
                } while (local_1c != 0);        // 0x004B015F: jne 0x4b0144
            }
            out += count;                       // 0x004B0161: add eax,ebp
        }
        local_10 = 0;                           // 0x004B0163: mov dword [esp+0x10],0
        destination[out] = '*';                 // 0x004B016B: mov byte [eax+esi],0x2a
        ++out;                                  // 0x004B016F: inc eax
        ++in;                                   // 0x004B0170: inc ecx
        goto loop_test;                         // 0x004B0171: jmp 0x4b008e

    fail:                                       // 0x004B01CF
        return 0;                               // 0x004B01D2: xor eax,eax
    }
}

} // extern "C"

#undef NATIVE_DECL
