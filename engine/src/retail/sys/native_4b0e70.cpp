// Native: Sys_AddSlot_4B0E70; retail VA 0x004B0E70 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004B0E70-0x004B0EAF (64 bytes, 6 blocks, 7 edges, 18 instructions).
// Tools/evidence: radare2 6.2.0 af/pd and the raw bytes (33c0 b9844aa900 / 803900 / 83c108 40 81f9844ea900 / 52 68d46e5400 e819e9ffff 83c408 ebfe / 890cc5804a.. c604c5844a..01 / c3); section 22.3 of the evidence sheet.
// Behavior/CFG: scans the 8-byte-stride flag bytes at 0x00A94A84 for the first zero entry, counting in EAX, until 0x00A94E84. With no free entry it calls Debug_Error("RenderMode: Failed to create new method '%s'", name) and spins at 0x004B0E9A. Otherwise it stores the name pointer at 0x00A94A80 + index*8 and sets the flag byte at 0x00A94A84 + index*8 to 1.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. The record table 0x00A94A80 has no external owner, so this file defines it and it starts zeroed. 0x004B0EB0 clears entries of the same table and now refers to this definition. The fields beyond the +0x00 pointer and the +0x04 flag are not established.
#include "igi/retail/core/retail_error.hpp"

#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// 0x00A94A80: record table, 8 stride, name pointer at +0x00 and in-use flag byte at +0x04.
// 0x00A94E84 - 0x00A94A80 = 0x404 bytes (128 records), which is the scanned extent.
std::uint8_t g_a94a80[0x404];

void NATIVE_DECL Sys_AddSlot_4B0E70(const char* name)
{
    std::uint8_t* flag = &::g_a94a80[0] + 4U;                  // 0x004B0E72: ecx = 0x00A94A84
    std::uint32_t index = 0;                                   // 0x004B0E70: xor eax,eax

    for (;;)
    {
        if (*flag == 0U)                                       // 0x004B0E77-0x004B0E7A: cmp byte [ecx],0; je 0x004B0E9C
        {
            break;
        }
        flag += 8U;                                            // 0x004B0E7C: add ecx,8
        ++index;                                               // 0x004B0E7F: inc eax
        if (flag >= &::g_a94a80[0] + 0x404U)                   // 0x004B0E80-0x004B0E86: cmp ecx,0x00A94E84; jl
        {
            // 0x004B0E88-0x004B0E9A: push name; push "RenderMode: Failed to create new method '%s'"; call Debug_Error; add esp,8; jmp 0x004B0E9A.
            Debug_Error("RenderMode: Failed to create new method '%s'", name);
            for (;;)
            {
            }
        }
    }

    // 0x004B0E9C-0x004B0EAF: mov dword [eax*8 + 0x00A94A80], ecx; mov byte [eax*8 + 0x00A94A84], 1; ret.
    *reinterpret_cast<const char**>(&::g_a94a80[0] + static_cast<std::uintptr_t>(index) * 8U) = name;
    ::g_a94a80[4U + static_cast<std::uintptr_t>(index) * 8U] = 1U;
}

} // extern "C"

#undef NATIVE_DECL
