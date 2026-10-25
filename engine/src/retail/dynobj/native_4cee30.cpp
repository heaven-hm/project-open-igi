// Native: DynObj_LookupPackageItem_4CEE30 (address-suffixed placeholder chosen because the body CONTRADICTS the manifest name - see below); retail VA 0x004CEE30-0x004CEE9A (107 bytes, 14 blocks, 23 edges, 54 instructions).
// Artifact: retail igi.exe SHA-256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; image base 0x00400000; .text VA 0x00401000 raw 0x1000.
// Tools/evidence: capstone 5.0.7 reachability decode from the verified entry (fallthrough plus every branch target, no fallthrough past a ret). BOUND USED: the `ret` at 0x004CEE9A, followed by FIVE `90` nops (read byte-by-byte from the image: 90 90 90 90 90), after which 0x004CEEA0 begins the next routine. Exact bytes A1 60 EC A4 00 53 55 56 8B 18 57 85 DB 74 4E 8B 68 28 8B 7D 00 85 FF 74 3C 8B 75 08 8B 4C 24 14 8A 01 8A D0 3A 06 75 1C 84 D2 74 14 8A 41 01 8A D0 3A 46 01 75 0E 83 C1 02 83 C6 02 84 D2 75 E0 33 C9 EB 05 1B C9 83 D9 FF 85 C9 74 17 8B EF 8B 3F 85 FF 75 C4 8B C3 8B 1B 85 DB 75 B2 5F 5E 5D 33 C0 5B C3 5F 8B C5 5E 5D 5B C3. The single absolute reference is `mov 0x00A4EC60,%eax` at 0x004CEE30 (the dword AT that address, i.e. the head node). Call census from the image's 0xE8 enumeration: exactly ONE call site image-wide, 0x004CEDB8 (dynobj/native_4ced50.cpp), so this is a private helper of the package loader rather than a shared routine. It makes no calls of its own.
// Behavior/CFG: one cdecl pointer argument (a NUL-terminated name) and a pointer result. It walks the package list: node = the dword at 0x00A4EC60, record = node->+0x00; if that is null it returns null; otherwise, for each record, current = the record's +0x28 embedded list's first item, next = current->+0x00; while next is non-null it compares the argument against the item's name at +0x08 two bytes at a time and returns the item on equality; otherwise it advances (current = next, next = next->+0x00) and continues; when the item list is exhausted it advances to the next record through +0x00 and repeats; if no record yields a match it returns null. TWO RETAIL DETAILS ARE PRESERVED: (1) the loop is entered only when `next` is non-null and compares `current` before advancing, so the LAST node of each +0x28 list is never compared - correct for a sentinel-terminated list, and a reader who converts this into a canonical for-loop changes which nodes are tested; (2) the comparison computes only the SIGN of a strcmp-style result through `sbb %ecx,%ecx; sbb %ecx,-1` (0x004CEE74), and the only consumer is `test %ecx,%ecx; je` at 0x004CEE79, so equality is the whole observable result and the magnitude is never read.
// Status/limits: TRANSCRIBED; NOT LINKABLE - undefined in tree, measured: g_a4ec60 (0x00A4EC60, no external definition - the same global the .mtp opener needs) and nothing else; the routine calls nothing. NAME CONTRADICTED: the manifest names this VA DynObj_ParseConfigFile_13 with a path outside the tree, but the body performs no parsing and no file access - it is a name lookup over the package list returning a matching item pointer, so it is filed under an address-suffixed placeholder and the manifest record needs the same correction. STRUCTURALLY_VERIFIED against the 54-instruction listing (14 blocks, 23 edges); BUILD_VERIFIED (i686-w64-mingw32-g++ -std=c++20 -O2 -Wall -Wextra single-file compile, rc=0, zero warnings); runtime UNOBSERVED. Domain: dynobj/.

#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// NATIVE GAP: 0x00A4EC60 has no external definition under engine/ - dynobj/mtp_package_loader.cpp defines an
// identically named object inside an unnamed namespace, which gives it internal linkage, so it cannot satisfy this
// reference until it is exposed. 0x004CEE30 reads the dword AT that address and treats it as the package list's
// head node, whose +0x00 is the first record and whose +0x28 is an embedded item list.
extern "C" void* g_a4ec60;

void* NATIVE_DECL DynObj_LookupPackageItem_4CEE30(char const* name)
{
    // 0x004CEE30/0x004CEE38: mov 0x00A4EC60,%eax; mov (%eax),%ebx - the head node and the first record.
    std::uint8_t* node = static_cast<std::uint8_t*>(g_a4ec60);       // %eax
    std::uint8_t* record = *reinterpret_cast<std::uint8_t**>(node);  // %ebx

    // 0x004CEE3B/0x004CEE3D: test %ebx,%ebx; je 0x004CEE8D - an empty list returns null.
    if (record == nullptr)
    {
        return nullptr;
    }

    for (;;)
    {
        // 0x004CEE3F: mov 0x28(%eax),%ebp - the record's embedded +0x28 list, whose first field is the first item.
        std::uint8_t* current = *reinterpret_cast<std::uint8_t**>(node + 0x28); // %ebp
        // 0x004CEE42/0x004CEE45/0x004CEE47: mov (%ebp),%edi; test %edi,%edi; je 0x004CEE85.
        std::uint8_t* next = *reinterpret_cast<std::uint8_t**>(current);        // %edi

        if (next != nullptr)
        {
            for (;;)
            {
                // 0x004CEE49: mov 0x8(%ebp),%esi - the item's name string.
                std::uint8_t const* item_name = *reinterpret_cast<std::uint8_t const**>(current + 8);
                // 0x004CEE4C: mov 0x14(%esp),%ecx - the argument, re-read on every comparison.
                std::uint8_t const* left = reinterpret_cast<std::uint8_t const*>(name);
                std::uint8_t const* right = item_name;
                bool equal = false;

                for (;;)                                  // 0x004CEE50
                {
                    std::uint8_t ch = left[0];            // mov (%ecx),%al
                    if (ch != right[0])                   // cmp (%esi),%al
                    {
                        break;                            // jne 0x004CEE74 - not equal
                    }
                    if (ch == 0)                          // test %dl,%dl
                    {
                        equal = true;                     // je 0x004CEE70 - both strings ended together
                        break;
                    }
                    ch = left[1];                         // 0x004CEE5C: mov 0x1(%ecx),%al
                    if (ch != right[1])                   // cmp 0x1(%esi),%al
                    {
                        break;                            // jne 0x004CEE74 - not equal
                    }
                    left += 2;                            // 0x004CEE66: add $0x2,%ecx
                    right += 2;                           // 0x004CEE69: add $0x2,%esi
                    if (ch == 0)                          // test %dl,%dl
                    {
                        equal = true;                     // fallthrough to 0x004CEE70
                        break;
                    }
                }

                // 0x004CEE79/0x004CEE7B: test %ecx,%ecx; je 0x004CEE94 - only the equality of the comparison is
                // consumed, so the sign computed at 0x004CEE74 is not reproduced here.
                if (equal)
                {
                    return current;                       // 0x004CEE94: mov %ebp,%eax
                }

                // 0x004CEE7D/0x004CEE7F/0x004CEE81: mov %edi,%ebp; mov (%edi),%edi; test %edi,%edi.
                current = next;
                next = *reinterpret_cast<std::uint8_t**>(next);
                if (next == nullptr)
                {
                    break;                                // je 0x004CEE85 - the item list is exhausted
                }
            }
        }

        // 0x004CEE85/0x004CEE87/0x004CEE89: mov %ebx,%eax; mov (%ebx),%ebx; test %ebx,%ebx; jne 0x004CEE3F.
        node = record;
        record = *reinterpret_cast<std::uint8_t**>(record);
        if (record == nullptr)
        {
            return nullptr;                               // 0x004CEE8D: xor %eax,%eax
        }
    }
}

} // extern "C"

#undef NATIVE_DECL
