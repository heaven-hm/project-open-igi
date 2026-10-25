// Native: Sys_MapCode_4ABC46; retail VA 0x004ABC46 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004ABC46-0x004ABCAB (103 bytes, 7 blocks, 10 edges, 26 instructions).
// Tools/evidence: radare2 6.2.0 af/afij/pdf and NASM ndisasm; the 45-entry table at 0x005466D8-0x00546840 is read from the retail image, and its bounds are both stated by the body (base mov $0x5466D8 and cmp $0x546840).
// Behavior/CFG: stores the argument at 0x00936068, scans the 45 eight-byte entries of 0x005466D8 comparing the entry code with the argument; on a hit it stores the entry's second dword at 0x00936064 and returns. With no hit, codes 0x13..0x24 map to 0x0D, codes 0xBC..0xCA store 0x08 and then 0x16 when above 0xCA, and anything else gets 0x16. The 0x08 store happens before the range test exactly as the bytes order it, so out-of-range values pass through 0x08 first.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. The table, 0x00936068 and 0x00936064 have no other owner in the tree, so this file defines them; the meaning of the codes and of the mapped values is not established by this body.
#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

struct CodeMapEntry
{
    std::uint32_t code;      // +0x00, compared with the argument
    std::uint32_t mapped;    // +0x04, stored at 0x00936064 on a hit
};

// 0x005466D8: 45 entries x 8 bytes; the body states both ends (0x005466D8 base, 0x00546840 limit).
CodeMapEntry g_5466d8[45] = {
    { 1U, 22U },
    { 2U, 2U },
    { 3U, 2U },
    { 4U, 24U },
    { 5U, 13U },
    { 6U, 9U },
    { 7U, 12U },
    { 8U, 12U },
    { 9U, 12U },
    { 10U, 7U },
    { 11U, 8U },
    { 12U, 22U },
    { 13U, 22U },
    { 15U, 2U },
    { 16U, 13U },
    { 17U, 18U },
    { 18U, 2U },
    { 33U, 13U },
    { 53U, 2U },
    { 65U, 13U },
    { 67U, 2U },
    { 80U, 17U },
    { 82U, 13U },
    { 83U, 13U },
    { 87U, 22U },
    { 89U, 11U },
    { 108U, 13U },
    { 109U, 32U },
    { 112U, 28U },
    { 114U, 9U },
    { 6U, 22U },
    { 128U, 10U },
    { 129U, 10U },
    { 130U, 9U },
    { 131U, 22U },
    { 132U, 13U },
    { 145U, 41U },
    { 158U, 13U },
    { 161U, 2U },
    { 164U, 11U },
    { 167U, 13U },
    { 183U, 17U },
    { 206U, 2U },
    { 215U, 11U },
    { 1816U, 12U },
};

// 0x00936068: the last code passed in. 0x00936064: the mapped result. Both four bytes, written only here.
std::uint32_t g_936068;
std::uint32_t g_936064;

void NATIVE_DECL Sys_MapCode_4ABC46(std::int32_t code)
{
    ::g_936068 = static_cast<std::uint32_t>(code);                      // 0x4ABC4C: mov ecx,0x00936068
    const CodeMapEntry* entry = &::g_5466d8[0];                        // 0x4ABC52: mov $0x005466D8,%eax
    for (;;)
    {
        if (entry->code == static_cast<std::uint32_t>(code))            // 0x4ABC57/0x4ABC59: cmp (%eax),%ecx; je
        {
            ::g_936064 = entry->mapped;                                // 0x4ABC7B/0x4ABC82: mov 0x005466DC(,%edx,8),%eax; mov eax,0x00936064
            return;                                                    // 0x4ABC87: ret
        }
        ++entry;                                                       // 0x4ABC5B: add $0x8,%eax
        if (entry >= &::g_5466d8[45])                                  // 0x4ABC5F/0x4ABC64: cmp $0x00546840,%eax; jb
        {
            break;
        }
    }
    if (code >= 0x13 && code <= 0x24)                                  // 0x4ABC66/0x4ABC6B/0x4ABC69/0x4ABC6E: cmp $0x13; jb; cmp $0x24; ja
    {
        ::g_936064 = 0x0DU;                                            // 0x4ABC70: movl $0xD,0x00936064
        return;                                                        // 0x4ABC7A: ret
    }
    if (code < 0xBC)                                                   // 0x4ABC88/0x4ABC8E: cmp $0xBC; jb
    {
        ::g_936064 = 0x16U;                                            // 0x4ABCA2: movl $0x16,0x00936064
        return;                                                        // 0x4ABCAC: ret
    }
    ::g_936064 = 0x08U;                                                // 0x4ABC96: movl $0x8,0x00936064 (before the range test, as retail orders it)
    if (code > 0xCA)                                                   // 0x4ABC90/0x4ABC96: cmp $0xCA; jbe
    {
        ::g_936064 = 0x16U;                                            // 0x4ABCA2: movl $0x16,0x00936064
    }                                                                 // 0x4ABCAC: ret
}

} // extern "C"

#undef NATIVE_DECL
