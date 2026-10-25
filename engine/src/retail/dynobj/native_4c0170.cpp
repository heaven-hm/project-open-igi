// Native: DynObj_IsNumericToken_4C0170; retail VA 0x004C0170 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004C0170-0x004C01C9 (89 bytes, 13 blocks, 17 edges, 36 instructions).
// Tools/evidence: radare2 6.2.0 af/afij/pdf and NASM ndisasm over the exact bytes; six exits, no call, one field load (+0x14) and two character-array reads (offset 0 and 1).
// Behavior/CFG: takes one cdecl argument, loads the character pointer from object+0x14, and classifies the text by its first one or two characters: a leading digit returns 1; a leading '$' (0x24) returns 1 when the next byte is a digit, a-f or A-F; a leading '0' returns 1 when the next byte is 'x' (0x78); and independently of the first byte, a second byte of 'X' (0x58) returns 1. Everything else returns 0. The order of those tests and the fall-through between them follow the bytes exactly.
// Status/limits: BUILD_VERIFIED; runtime UNOBSERVED. The argument's object type is not established beyond the +0x14 pointer this body reads; the routine only inspects two characters and returns 0 or 1.
#include "dev_log.hpp"
#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

namespace igi::retail::dynobj {

std::int32_t NATIVE_DECL DynObj_IsNumericToken_4C0170(void* object)
{
    NATIVE_LOG_ENTER("DynObj_IsNumericToken_4C0170", "0x004C0170");
    const char* const text = *reinterpret_cast<const char* const*>(        // 0x4C0174: mov 0x14(%eax),%edx
        static_cast<std::uint8_t*>(object) + 0x14U);
    const char first = text[0];                                            // 0x4C0177: mov (%edx),%cl
    if (first >= 0x30 && first <= 0x39)                                    // 0x4C0179/0x4C017C/0x4C017E/0x4C0181: cmp 0x30; jl; cmp 0x39; jg
    {
        return 1;                                                          // 0x4C0183: mov $0x1,%eax; 0x4C0188: ret
    }
    if (first == 0x24)                                                     // 0x4C0189/0x4C018C: cmp 0x24; jne
    {
        const char second = text[1];                                       // 0x4C018E: mov 0x1(%edx),%al
        if (second >= 0x30 && second <= 0x39)                              // 0x4C0191-0x4C0197: cmp 0x30; jl; cmp 0x39; jle
        {
            return 1;                                                      // 0x4C01A9: mov $0x1,%eax; 0x4C01AE: ret
        }
        if (second >= 0x61 && second <= 0x66)                              // 0x4C0199-0x4C019F: cmp 0x61; jl; cmp 0x66; jle
        {
            return 1;                                                      // 0x4C01A9: mov $0x1,%eax; 0x4C01AE: ret
        }
        if (second >= 0x41 && second <= 0x46)                              // 0x4C01A1-0x4C01A7: cmp 0x41; jl; cmp 0x46; jg
        {
            return 1;                                                      // 0x4C01A9: mov $0x1,%eax; 0x4C01AE: ret
        }
    }
    if (first == 0x30)                                                     // 0x4C01AF/0x4C01B2: cmp 0x30; jne
    {
        if (text[1] == 0x78)                                               // 0x4C01B4/0x4C01B8: cmpb $0x78,0x1(%edx); je
        {
            return 1;                                                      // 0x4C01C0: mov $0x1,%eax; 0x4C01C5: ret
        }
    }
    if (text[1] == 0x58)                                                   // 0x4C01BA/0x4C01BE: cmpb $0x58,0x1(%edx); jne
    {
        return 1;                                                          // 0x4C01C0: mov $0x1,%eax; 0x4C01C5: ret
    }
    return 0;                                                              // 0x4C01C6/0x4C01C8: xor eax,eax; ret
}

} // namespace igi::retail::dynobj

#undef NATIVE_DECL
