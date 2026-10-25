// Native: DynObj_LoadRecordVariants_4CED50 (address-suffixed placeholder: retail's symbol name is unknown and no manifest record exists; the name records only measured behavior - the routine fills one record from successive name variants); retail VA 0x004CED50-0x004CEE26 (215 bytes, 5 blocks, 6 edges, 84 instructions).
// Artifact: retail igi.exe SHA-256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; image base 0x00400000; .text VA 0x00401000 raw 0x1000.
// Tools/evidence: capstone 5.0.7 reachability decode from the verified entry (fallthrough plus every branch target, no fallthrough past a ret). The extent is bounded by the `ret` at 0x004CEE26 and NINE `90` nops follow it (read byte-by-byte from the image; the next entry 0x004CEE30 starts `mov 0x00A4EC60,%eax`). Exact bytes 83 EC 24 53 8B 5C 24 2C 55 56 57 33 ED 8D 7B 14 83 C9 FF 33 C0 89 6B 24 F2 AE F7 D1 2B F9 8D 54 24 14 8B C1 8B F7 8B FA 68 18 C9 53 00 C1 E9 02 F3 A5 8B C8 83 E1 03 F3 A4 8D 4C 24 18 51 E8 1D 6E FD FF 89 6C 24 18 83 C4 08 BE 01 00 00 00 8D 6B 3E 8B CE B8 64 00 00 00 0F AF CE 99 F7 F9 8D 54 24 14 52 66 89 45 00 E8 73 00 00 00 83 C4 04 89 03 85 C0 74 4C 8D 7C 24 14 83 C9 FF 33 C0 83 C5 02 F2 AE F7 D1 49 8D 7C 24 14 83 C3 04 8A 54 0C 13 83 C9 FF FE C2 F2 AE 8B 44 24 10 F7 D1 49 40 46 89 44 24 10 83 FE 06 88 54 0C 13 7C A3 66 8B D0 8B 44 24 38 5F 5E 5D 66 89 50 3C 5B 83 C4 24 C3 8B 44 24 38 66 8B 4C 24 10 5F 5E 5D 66 89 48 3C 5B 83 C4 24 C3. Call displacements resolve as v+5+rel32: 0x004CED8E + 0xFFFD6E1D (-168419) = 0x004A5BB0; 0x004CEDB8 + 0x73 (115) = 0x004CEE30. The literal pushed at 0x004CED78 is 0x0053C918 and its bytes are "johan" followed by a terminator. Call census from the image's 0xE8 enumeration: TWO call sites image-wide, 0x004CED18 and 0x004CF05E (the package sweep in dynobj/native_4cf050.cpp).
// Behavior/CFG: one cdecl pointer argument (the record), no return value. The frame is `sub $0x24,%esp`. It zeroes the record's dword at +0x24, copies the record's NUL-terminated name from +0x14 into the frame (rep movsd then rep movsb, terminator included), calls 0x004A5BB0 with a pointer four bytes into that buffer and the literal "johan" - discarding the result, which retail never reads - then loops five times with the index 1..5: it computes the SIGNED quotient 100/(index*index), stores it as a word at +0x3E+2*(index-1) BEFORE the call, calls 0x004CEE30 with the name buffer, stores the returned pointer at +0x00+4*(index-1), and on a null result leaves through the tail epilogue. On a non-null result it re-scans the name, increments its LAST character in place, and counts the load. Both epilogues write the low word of the count to +0x3C of the ORIGINAL argument - not of the advanced slot pointer, which is why the argument is re-read from the stack at 0x004CEE02/0x004CEE12. THE FRAME ALIASES AND IS MODELLED AS ONE ARRAY: the pointer pushed at 0x004CED89 is [esp+0x18], which is four bytes into the buffer written through [esp+0x14] at 0x004CED6E, and the last-character read at [esp+0x13+ecx] falls one byte BELOW that buffer when the name is empty - two separate C++ objects would not reproduce either overlap. Note also that the counter is initialized at 0x004CED93 as `mov %ebp,0x18(%esp)` while the loop reads it as `0x10(%esp)`: the same slot, addressed eight bytes higher because the two call arguments are still pushed at that point.
// Status/limits: TRANSCRIBED; NOT LINKABLE - undefined in tree, measured: Str_FindSubstring_4A5BB0 (0x004A5BB0) and DynObj_ParseConfigFile_13 (0x004CEE30), both assigned to another writer and neither stubbed here. NAME COORDINATION: 0x004CEE30 is declared with its manifest name, so the implementation of that VA must define exactly this symbol; 0x004A5BB0 has no manifest record and is declared with an address-suffixed placeholder derived from its measured behavior, so that name must be reconciled with the writer's file or the link error will look like a missing implementation. STRUCTURALLY_VERIFIED against the 84-instruction listing (5 blocks, 6 edges); BUILD_VERIFIED (i686-w64-mingw32-g++ -std=c++20 -O2 -Wall -Wextra single-file compile, rc=0, zero warnings); runtime UNOBSERVED. Domain: dynobj/.

#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// NATIVE GAP: 0x004CEE30 has no implementation under engine/ (its manifest record names
// scripts/GameNatives/dynobj/004CEE30-DynObj_ParseConfigFile_13.cpp, a path outside the tree). Declared with the
// manifest's name so this file and that implementation agree. One cdecl pointer argument (the name buffer); the
// returned pointer is stored in the record, and a null result ends the loop.
void* NATIVE_DECL DynObj_ParseConfigFile_13(void* name_buffer);

// NATIVE GAP: 0x004A5BB0 has no implementation and no manifest record. Measured behavior: a read-only substring
// search - it takes its second argument's first byte and scans its first argument for it, returning a pointer into
// that haystack or null, and it never writes through either pointer. The placeholder name records only that.
char* NATIVE_DECL Str_FindSubstring_4A5BB0(char const* haystack, char const* needle);

void NATIVE_DECL DynObj_LoadRecordVariants_4CED50(void* record_argument)
{
    // 0x004CED50: sub $0x24,%esp - retail's frame, modelled as ONE array because retail's slots alias: the pointer
    // pushed at 0x004CED89 is [esp+0x18], four bytes into the name buffer that 0x004CED6E writes through
    // [esp+0x14], and the last-character access at [esp+0x13+ecx] reaches one byte below that buffer when the name
    // is empty. Separate C++ objects could not reproduce either overlap.
    std::uint8_t frame[0x24];
    std::uint32_t* const loaded_count = reinterpret_cast<std::uint32_t*>(frame); // [esp+0x10]
    std::uint8_t* const name_buffer = frame + 0x04;                              // [esp+0x14]

    // 0x004CED53/0x004CED54: push %ebx; mov 0x2c(%esp),%ebx - the argument. Both epilogues re-read it from the stack
    // as [esp+0x38], so it is kept unmodified here while the loop advances its own copy.
    std::uint8_t* const record = static_cast<std::uint8_t*>(record_argument);

    // 0x004CED5B/0x004CED65: xor %ebp,%ebp; mov %ebp,0x24(%ebx).
    *reinterpret_cast<std::uint32_t*>(record + 0x24) = 0;

    // 0x004CED5D-0x004CED6C: lea 0x14(%ebx),%edi; or $0xffffffff,%ecx; xor %eax,%eax; repne scasb; not %ecx;
    // sub %ecx,%edi - a NUL scan of the record's name field; the byte count copied below includes the terminator.
    std::uint8_t const* const name_source = record + 0x14;
    std::uint32_t copy_length = 0;
    while (name_source[copy_length] != 0)
    {
        ++copy_length;
    }
    ++copy_length;

    // 0x004CED6E-0x004CED87: lea 0x14(%esp),%edx; mov %ecx,%eax; mov %edi,%esi; mov %edx,%edi; shr $0x2,%ecx;
    // rep movsd; mov %eax,%ecx; and $0x3,%ecx; rep movsb - whole dwords first, then the trailing 0..3 bytes.
    std::uint32_t const whole_words = copy_length >> 2;
    for (std::uint32_t word = 0; word < whole_words; ++word)
    {
        reinterpret_cast<std::uint32_t*>(name_buffer)[word] =
            reinterpret_cast<std::uint32_t const*>(name_source)[word];
    }
    for (std::uint32_t byte = 0; byte < (copy_length & 3); ++byte)
    {
        name_buffer[whole_words * 4 + byte] = name_source[whole_words * 4 + byte];
    }

    // 0x004CED78/0x004CED89/0x004CED8D/0x004CED8E: push $0x0053C918; lea 0x18(%esp),%ecx; push %ecx; call
    // 0x004A5BB0. The haystack is [esp+0x18], four bytes past the start of the name buffer, and the result is
    // discarded - retail never reads that slot again. The call is retail's, so it is made.
    Str_FindSubstring_4A5BB0(reinterpret_cast<char const*>(frame + 0x08), "johan");

    // 0x004CED93/0x004CED97: mov %ebp,0x18(%esp); add $0x8,%esp - with the two arguments still pushed, [esp+0x18]
    // resolves to the frame base, the same slot the loop later reads as [esp+0x10]. This is the counter's only
    // initialization, and it happens after the call above.
    *loaded_count = 0;

    // 0x004CED9A/0x004CED9F: mov $0x1,%esi; lea 0x3e(%ebx),%ebp - the variant index and the record's scale slot.
    std::uint32_t index = 1;
    std::uint8_t* slot = record;               // %ebx, advanced by 4 for each stored pointer
    std::uint8_t* scale_slot = record + 0x3e;  // %ebp, advanced by 2 for each scale word

    do
    {
        // 0x004CEDA2-0x004CEDAD: mov %esi,%ecx; mov $0x64,%eax; imul %esi,%ecx; cdq; idiv %ecx - the signed
        // quotient 100/(index*index).
        std::int32_t const scale = 100 / static_cast<std::int32_t>(index * index);

        // 0x004CEDAF/0x004CEDB3/0x004CEDB4: lea 0x14(%esp),%edx; push %edx; mov %ax,(%ebp) - the quotient is stored
        // as a WORD before the call, not after it.
        *reinterpret_cast<std::uint16_t*>(scale_slot) = static_cast<std::uint16_t>(scale);
        void* const loaded = DynObj_ParseConfigFile_13(name_buffer);

        // 0x004CEDC0/0x004CEDC2/0x004CEDC4: mov %eax,(%ebx); test %eax,%eax; je 0x004CEE12 - the pointer is stored
        // at +0x00+4*(index-1) and a null result leaves through the tail epilogue.
        *reinterpret_cast<void**>(slot) = loaded;
        if (loaded == nullptr)
        {
            // 0x004CEE12/0x004CEE16/0x004CEE1E: mov 0x38(%esp),%eax; mov 0x10(%esp),%cx; mov %cx,0x3c(%eax) - the
            // tail writes the same count word, to the original argument.
            *reinterpret_cast<std::uint16_t*>(record + 0x3c) = static_cast<std::uint16_t>(*loaded_count);
            return;
        }

        // 0x004CEDC6-0x004CEDDE: a second NUL scan of the name buffer; ecx ends as the length and dl takes the byte
        // at [esp+0x13+ecx], the LAST character of the name.
        std::uint32_t name_length = 0;
        while (name_buffer[name_length] != 0)
        {
            ++name_length;
        }
        std::uint8_t last_character = frame[0x04 + name_length - 1];
        ++last_character; // 0x004CEDE5: inc %dl

        // 0x004CEDE9-0x004CEDF2: mov 0x10(%esp),%eax; inc %eax; mov %eax,0x10(%esp) - count the successful load.
        ++*loaded_count;

        // 0x004CEDCF/0x004CEDDB/0x004CEDF1: add $0x2,%ebp; add $0x4,%ebx; inc %esi.
        scale_slot += 2;
        slot += 4;
        ++index;

        // 0x004CEDF9: mov %dl,0x13(%esp,%ecx,1) - the incremented last character goes back to the byte it came
        // from, which is how the next iteration asks for the next name variant.
        frame[0x04 + name_length - 1] = last_character;
        // 0x004CEDF6/0x004CEDFD: cmp $0x6,%esi; jl 0x004CEDA2 - five iterations, index 1 through 5.
    } while (index < 6);

    // 0x004CEDFF/0x004CEE02/0x004CEE09: mov %ax,%dx; mov 0x38(%esp),%eax; mov %dx,0x3c(%eax) - the low word of the
    // count goes to the ORIGINAL record argument, not to the advanced slot pointer.
    *reinterpret_cast<std::uint16_t*>(record + 0x3c) = static_cast<std::uint16_t>(*loaded_count);
}

} // extern "C"

#undef NATIVE_DECL
