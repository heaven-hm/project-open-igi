// Native: QFile_ClearEntryByName_4B13A0; retail VA 0x004B13A0 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004B13A0-0x004B1415 (118 bytes, 13 blocks, 18 edges, 49 instructions).
// Tools/evidence: radare2 6.2.0 af/pd and the raw bytes (8b542404 53 55 56 57 33ed bf30239400 / 8b8790000000 / ... / c7048dc023... / c3); section 22.3 of the evidence sheet.
// Behavior/CFG: walks 32 records of 0x94 bytes from 0x00942330 up to 0x009435B0, skipping any record whose +0x90 word is zero, and compares the record's name at +0x00 against the argument with the same TWO-byte unrolled inlined strcmp that 0x004B1620 uses. On a match it clears the word at 0x009423C0 + index*0x94 and returns; otherwise it advances and ends with no value. The skip-then-compare order, the stride, the end address and the two-byte lattice are retail's.
// Status/limits: STRUCTURALLY_VERIFIED; runtime UNOBSERVED. 0x009423C0 is owned by native_4b12d0.cpp (g_9423c0); the record table 0x00942330 has no external owner, so this file defines it and it starts zeroed. The record's fields beyond the name at +0x00 and the +0x90 word are not established.
#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// 0x00942330: the record table this routine walks; 32 records of 0x94 bytes end at 0x009435B0.
std::uint8_t g_942330[0x1280];

// 0x009423C0: the parallel table the match clears; owned by native_4b12d0.cpp.
extern std::uint8_t g_9423c0[0x1280];

void NATIVE_DECL QFile_ClearEntryByName_4B13A0(const char* name)
{
    std::uint32_t index = 0;                              // 0x004B13A8: ebp = 0
    std::uint8_t* record = &::g_942330[0];                // 0x004B13AA: edi = 0x00942330

    for (;;)
    {
        if (*reinterpret_cast<const std::uint32_t*>(record + 0x90U) != 0U)   // 0x004B13AF-0x004B13B7: je 0x004B13EA
        {
            const char* text = reinterpret_cast<const char*>(record);        // 0x004B13BB: eax = edi
            const char* other = name;                                        // 0x004B13B9: esi = edx

            // Inlined strcmp, two bytes per iteration, exactly as retail unrolled it.
            for (;;)
            {
                const char first = *text;                    // 0x004B13BD: bl = [eax]
                if (first != *other)                         // 0x004B13C1: cmp bl,[esi]; jne 0x004B13E1
                {
                    goto mismatch;
                }
                if (first == '\0')                           // 0x004B13C5: test cl,cl; je 0x004B13DD
                {
                    break;
                }
                const char second = text[1];                 // 0x004B13C9: bl = [eax+1]
                if (second != other[1])                      // 0x004B13CE: cmp bl,[esi+1]; jne 0x004B13E1
                {
                    goto mismatch;
                }
                text += 2;                                   // 0x004B13D3: add eax,2
                other += 2;                                  // 0x004B13D6: add esi,2
                if (second == '\0')                          // 0x004B13D9: test cl,cl; jne 0x004B13BD
                {
                    break;
                }
            }

            // Match: clear the parallel word at 0x009423C0 + index*0x94 and return (0x004B13FE-0x004B1415).
            *reinterpret_cast<std::uint32_t*>(reinterpret_cast<std::uint8_t*>(&::g_9423c0[0]) +
                                              static_cast<std::uintptr_t>(index) * 0x94U) = 0U;
            return;
        }

    mismatch:
        record += 0x94;                                      // 0x004B13EA: add edi,0x94
        ++index;                                             // 0x004B13F0: inc ebp
        if (record >= &::g_942330[0x1280])                   // 0x004B13F1-0x004B13F7: cmp edi,0x009435B0; jl 0x004B13AF
        {
            return;                                          // 0x004B13F9-0x004B13FD: pop edi/esi/ebp/ebx; ret
        }
    }
}

} // extern "C"

#undef NATIVE_DECL
