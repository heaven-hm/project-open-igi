// Native: QFile_LookupDeviceByName_4B1620; retail VA 0x004B1620 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004B1620-0x004B1685 (102 bytes, 13 blocks, 18 edges, 46 instructions).
// Tools/evidence: radare2 6.2.0 af/pd and the raw bytes (5355 56 8b35046f5400 33ed 57 85f6 / ... / 8b04ad704a..); section 22.3 of the evidence sheet (landability sweep row 0x004B1620).
// Behavior/CFG: walks the device-name pointer table at 0x00546F04 (32-bit string pointers, 4-byte stride, null-terminated) and compares each name against the argument with an inlined strcmp that consumes TWO bytes per iteration: first byte compare, end-of-string test, second byte compare, advance by two, end-of-string test. On a match it returns the paired word from 0x00534A70 indexed by the match position; if the table is empty or exhausted it returns 0. The two-byte unrolling, the end-of-string tests after each of the two byte comparisons, and the per-iteration reload of the next table slot are retail's shape and are reproduced as written rather than collapsed into strcmp.
// Status/limits: BUILD_VERIFIED; runtime UNOBSERVED. Neither 0x00546F04 nor 0x00534A70 has an external owner in the tree, so this file defines both and they start zeroed. What the paired word in 0x00534A70 means is unproven; only its address and 4-byte stride are established.
#include "dev_log.hpp"
#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// 0x00546F04: null-terminated table of device-name string pointers, 4-byte stride.
const char* g_546f04[16];

// 0x00534A70: the table this routine returns from, indexed by the matched name's position.
std::uint32_t g_534a70[16];

std::uint32_t NATIVE_DECL QFile_LookupDeviceByName_4B1620(const char* name)
{
    NATIVE_LOG_ENTER("QFile_LookupDeviceByName_4B1620", "0x004B1620");
    const char* const* slot = &::g_546f04[0];            // 0x004B1634: edi = 0x00546F04
    const char* candidate = *slot;                       // 0x004B1623: esi = [0x00546F04]

    if (candidate == nullptr)                             // 0x004B162C-0x004B162E: test esi,esi; je 0x004B1673
    {
        return 0U;                                       // 0x004B1673-0x004B1679: xor eax,eax; ret
    }

    for (std::uint32_t position = 0;; ++position)        // 0x004B1629: ebp = 0, incremented at 0x004B166E
    {
        const char* text = name;                         // 0x004B1639: eax = edx
        const char* other = candidate;

        // Inlined strcmp, two bytes per iteration, exactly as retail unrolled it.
        for (;;)
        {
            const char first = *text;                    // 0x004B163B: bl = [eax]
            if (first != *other)                         // 0x004B163F: cmp bl,[esi]; jne 0x004B165F
            {
                goto mismatch;
            }
            if (first == '\0')                           // 0x004B1643: test cl,cl; je 0x004B165B
            {
                break;                                   // equal: 0x004B165B: xor eax,eax
            }
            const char second = text[1];                 // 0x004B1647: bl = [eax+1]
            if (second != other[1])                      // 0x004B164C: cmp bl,[esi+1]; jne 0x004B165F
            {
                goto mismatch;
            }
            text += 2;                                   // 0x004B1651: add eax,2
            other += 2;                                  // 0x004B1654: add esi,2
            if (second == '\0')                          // 0x004B1657: test cl,cl; jne 0x004B163B
            {
                break;
            }
        }

        // Match: return the paired word for this position (0x004B167A: mov eax,[ebp*4 + 0x00534A70]).
        return ::g_534a70[position];

    mismatch:
        ++slot;                                          // 0x004B166B: add edi,4
        candidate = *slot;                               // 0x004B1668: esi = [edi+4]
        if (candidate == nullptr)                        // 0x004B166F-0x004B1671: test esi,esi; jne 0x004B1639
        {
            return 0U;                                   // 0x004B1673-0x004B1679
        }
    }
}

} // extern "C"

#undef NATIVE_DECL
