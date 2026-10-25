// Native: Sys_AddRenderMode_4B0E00; retail VA 0x004B0E00 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004B0E00-0x004B0E4A (75 bytes, 6 blocks, 7 edges, 22 instructions).
// Tools/evidence: radare2 6.2.0 af/pd and the raw bytes (33c0 b9a452a900 / 803900 / 81c108020000 40 81f9e462a900 / 50 68a06e5400 e886e9ffff 83c408 ebfe / 8991a050a900 c681a452a9..01 / c3); section 22.3 of the evidence sheet.
// Behavior/CFG: scans the 0x208-stride flag bytes at 0x00A952A4 for the first zero entry, counting in EAX, until 0x00A962E4. With no free entry it calls Debug_Error("RenderMode: Failed to create new rendermode '%s'", name) and spins at 0x004B0E2D. Otherwise it stores the name pointer at 0x00A950A0 + index*0x208 (retail computes index<<6 + index then <<3, i.e. *520) and sets the flag byte at 0x00A952A4 + index*0x208 to 1.
// Status/limits: BUILD_VERIFIED; runtime UNOBSERVED. The record table 0x00A950A0 has no external owner, so this file defines it; it starts zeroed, so every slot reads free exactly as an unwritten table would. The record's fields beyond the +0x00 pointer and the flag at +0x204 that this routine touches are not established, so the table is raw bytes rather than a described structure.
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

// 0x00A950A0: record table, 0x208 stride, name pointer at +0x00 and in-use flag byte at +0x204.
// 0x00A962E4 - 0x00A950A0 = 0x1244 bytes (eight records), which is the scanned extent.
std::uint8_t g_a950a0[0x1244];

void NATIVE_DECL Sys_AddRenderMode_4B0E00(const char* name)
{
    std::uint8_t* flag = &::g_a950a0[0] + 0x204U;              // 0x004B0E02: ecx = 0x00A952A4
    std::uint32_t index = 0;                                   // 0x004B0E00: xor eax,eax

    for (;;)
    {
        if (*flag == 0U)                                       // 0x004B0E07-0x004B0E0A: cmp byte [ecx],0; je 0x004B0E2F
        {
            break;
        }
        flag += 0x208U;                                        // 0x004B0E0C: add ecx,0x208
        ++index;                                               // 0x004B0E12: inc eax
        if (flag >= &::g_a950a0[0] + 0x1244U)                  // 0x004B0E13-0x004B0E19: cmp ecx,0x00A962E4; jl
        {
            // 0x004B0E1B-0x004B0E2D: push name; push "RenderMode: Failed to create new rendermode '%s'"; call Debug_Error; add esp,8; jmp 0x004B0E2D.
            Debug_Error("RenderMode: Failed to create new rendermode '%s'", name);
            for (;;)
            {
            }
        }
    }

    // 0x004B0E2F-0x004B0E4A: edx = name; ecx = (index<<6 + index)<<3 = index*0x208;
    // mov dword [ecx + 0x00A950A0], edx; mov byte [ecx + 0x00A952A4], 1; ret.
    *reinterpret_cast<const char**>(&::g_a950a0[0] + static_cast<std::uintptr_t>(index) * 0x208U) = name;
    ::g_a950a0[0x204U + static_cast<std::uintptr_t>(index) * 0x208U] = 1U;
}

} // extern "C"

#undef NATIVE_DECL
