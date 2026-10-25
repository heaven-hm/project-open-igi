// Native: Str_FindSubstring_4A5BB0 (address-suffixed placeholder: no manifest record exists for this VA; the measured behavior is a substring search, the CRT's strstr shape); retail VA 0x004A5BB0-0x004A5C2F (128 bytes, 18 blocks, 28 edges, 66 instructions).
// Artifact: retail igi.exe SHA-256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; image base 0x00400000; .text VA 0x00401000 raw 0x1000.
// Tools/evidence: capstone 5.0.7 reachability decode from the verified entry (fallthrough plus every branch target, no fallthrough past a ret). BOUND USED: the `ret` at 0x004A5C2F, and there is NO alignment padding after it - the very next byte at 0x004A5C30 is 0x55 (`push %ebp`), so the extent is bounded by the ret alone rather than by a nop run. Exact bytes 8B 4C 24 08 57 53 56 8A 11 8B 7C 24 10 84 D2 74 69 8A 71 01 84 F6 74 4F 8B F7 8B 4C 24 14 8A 07 46 38 D0 74 15 84 C0 74 0B 8A 06 46 38 D0 74 0A 84 C0 75 F5 5E 5B 5F 33 C0 C3 8A 06 46 38 F0 75 EB 8D 7E FF 8A 61 02 84 E4 74 28 8A 06 83 C6 02 38 E0 75 C4 8A 41 03 84 C0 74 18 8A 66 FF 83 C1 02 38 E0 74 DF EB B1 33 C0 5E 5B 5F 8A C2 E9 83 44 00 00 8D 47 FF 5E 5B 5F C3 8B C7 5E 5B 5F C3. Call census from the image's 0xE8 enumeration: TWELVE call sites image-wide (0x004922CC, 0x004A6728, 0x004C48EC, 0x004CEC4E, 0x004CED8E, 0x004EC07C, 0x004EDFEC, 0x004F1574, 0x0051DCF3, 0x0051DD1F, 0x00523435, 0x00523616), so it is a shared string routine and not a private helper. Its only tail jump, at 0x004A5C1E (`E9 83 44 00 00` = 0x004A5C23 + 0x4483), targets 0x004AA0A6 and is the ONLY reference to that address anywhere in the image; 0x004AA0A6 is the BODY of the routine whose entry is 0x004AA0A0 (`xor %eax,%eax; mov 0x8(%esp),%al`), so the jump enters past the entry with the character already zero-extended in EAX.
// Behavior/CFG: two cdecl pointer arguments (haystack, needle) and a pointer result. An empty needle returns the haystack unchanged (0x004A5BBF -> 0x004A5C2A). A one-character needle tail-jumps into the strchr-style routine at 0x004AA0A0 with EAX = needle[0] (0x004A5C17-0x004A5C1E). Otherwise it scans for the needle's first character with the second character tested at the same time: the entry block reads a byte, the unrolled block advances one byte at a time, and on a first-character hit it tests the second character at 0x004A5BEA, falling back into the scan at 0x004A5BDC - which re-tests that same byte against the first character, because a byte that failed the second-character test can still start a new match. On a two-character hit it saves the candidate and compares the remaining needle bytes two at a time (0x004A5BF4-0x004A5C15), returning at 0x004A5C23 with `lea -1(%edi),%eax` where edi was saved as `esi-1` at the two-character hit - so the returned pointer is the match position, one byte before the saved candidate. The block at 0x004A5BE4 returns null. A mismatch of the remaining bytes restarts the scan at 0x004A5BC8, which re-reads the pattern pointer from the stack, exactly as retail does. All 18 blocks and 28 edges are represented by the labelled flow below.
// Status/limits: TRANSCRIBED; NOT LINKABLE - undefined in tree, measured: Str_FindChar_4AA0A0 (0x004AA0A0, the tail target's real entry, no implementation and no manifest record; reported as a newly discovered routine) and nothing else. DOMAIN: filed under script/ as assigned; note that the routine is a shared string search whose only measured callers are spread across the image, so if it belongs in sys/ rather than script/ the file moves before it enters the build. STRUCTURALLY_VERIFIED against the 66-instruction listing (18 blocks, 28 edges); BUILD_VERIFIED (i686-w64-mingw32-g++ -std=c++20 -O2 -Wall -Wextra single-file compile, rc=0, zero warnings); runtime UNOBSERVED.

#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// NATIVE GAP: 0x004AA0A0 has no implementation under engine/ and no manifest record. Measured interface: it loads
// the character from its second stack argument (`xor %eax,%eax; mov 0x8(%esp),%al` at 0x004AA0A0/0x004AA0A2), scans
// the first argument for it, and returns a pointer to the match or null. Retail's tail jump at 0x004A5C1E enters
// the body at 0x004AA0A6, skipping that load because EAX already holds the zero-extended character; calling this
// entry with (haystack, first) makes it derive the same EAX from the same value.
char* NATIVE_DECL Str_FindChar_4AA0A0(char const* string, int character);

char* NATIVE_DECL Str_FindSubstring_4A5BB0(char const* haystack, char const* needle)
{
    // 0x004A5BB0/0x004A5BB7: mov 0x8(%esp),%ecx; mov (%ecx),%dl - the needle's first byte.
    std::uint8_t const* const pattern_base = reinterpret_cast<std::uint8_t const*>(needle);
    std::uint8_t const first = pattern_base[0];

    // 0x004A5BB9/0x004A5BBF: mov 0x10(%esp),%edi; test %dl,%dl; je 0x004A5C2A - an empty needle returns the
    // haystack itself, with no scan at all.
    if (first == 0)
    {
        return const_cast<char*>(haystack);
    }

    // 0x004A5BC1/0x004A5BC4/0x004A5BC6: mov 0x1(%ecx),%dh; test %dh,%dh; je 0x004A5C17.
    std::uint8_t const second = pattern_base[1];
    if (second == 0)
    {
        // 0x004A5C17-0x004A5C1E: xor %eax,%eax; mov %dl,%al; jmp 0x004AA0A6 - a tail jump into the strchr-style
        // body, with EAX zero-extended before AL is loaded so the replicated-character idiom inside is fed a clean
        // value. Retail's stack at the jump still holds (haystack, needle); the body reads only the haystack.
        return Str_FindChar_4AA0A0(haystack, first);
    }

    std::uint8_t const* candidate = reinterpret_cast<std::uint8_t const*>(haystack); // %edi
    std::uint8_t const* scan = nullptr;                                              // %esi
    std::uint8_t const* pattern = nullptr;                                           // %ecx
    std::uint8_t ch = 0;                                                             // %al

scan_top:                                            // 0x004A5BC8
    scan = candidate;                                // mov %edi,%esi
    pattern = pattern_base;                          // mov 0x14(%esp),%ecx
    ch = candidate[0];                               // mov (%edi),%al
    ++scan;                                          // inc %esi
    if (ch == first)                                 // cmp %dl,%al
    {
        goto compare_second;                         // je 0x004A5BEA
    }
    if (ch == 0)                                     // test %al,%al
    {
        goto not_found;                              // je 0x004A5BE4
    }

scan_unrolled:                                       // 0x004A5BD9
    ch = scan[0];                                    // mov (%esi),%al
    ++scan;                                          // inc %esi
scan_continue:                                       // 0x004A5BDC
    if (ch == first)                                 // cmp %dl,%al
    {
        goto compare_second;                         // je 0x004A5BEA
    }
    if (ch != 0)                                     // test %al,%al
    {
        goto scan_unrolled;                          // jne 0x004A5BD9
    }

not_found:                                           // 0x004A5BE4
    return nullptr;                                  // xor %eax,%eax

compare_second:                                      // 0x004A5BEA
    ch = scan[0];                                    // mov (%esi),%al
    ++scan;                                          // inc %esi
    if (ch != second)                                // cmp %dh,%al
    {
        goto scan_continue;                          // jne 0x004A5BDC - this byte may itself start a new match
    }
    candidate = scan - 1;                            // lea -1(%esi),%edi

compare_rest:                                        // 0x004A5BF4
    {
        std::uint8_t const next = pattern[2];        // mov 0x2(%ecx),%ah
        if (next == 0)                               // test %ah,%ah
        {
            goto matched;                            // je 0x004A5C23 - the needle ended here
        }
        ch = scan[0];                                // mov (%esi),%al
        scan += 2;                                   // add $0x2,%esi
        if (ch != next)                              // cmp %ah,%al
        {
            goto scan_top;                           // jne 0x004A5BC8
        }
        std::uint8_t const after = pattern[3];       // mov 0x3(%ecx),%al
        if (after == 0)                              // test %al,%al
        {
            goto matched;                            // je 0x004A5C23 - the needle ended here
        }
        std::uint8_t const previous = scan[-1];      // mov -1(%esi),%ah
        pattern += 2;                                // add $0x2,%ecx
        if (after == previous)                       // cmp %ah,%al
        {
            goto compare_rest;                       // je 0x004A5BF4
        }
        goto scan_top;                               // jmp 0x004A5BC8
    }

matched:                                             // 0x004A5C23
    // lea -1(%edi),%eax - candidate was saved as (scan-1) at the two-character hit, so this is the match position.
    return const_cast<char*>(reinterpret_cast<char const*>(candidate - 1));
}

} // extern "C"

#undef NATIVE_DECL
