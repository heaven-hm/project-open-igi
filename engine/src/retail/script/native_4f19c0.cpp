// Native: Script_ParameterTypeRegister (placeholder until VERIFIED); retail VA 0x004F19C0-0x004F1A38 (121 bytes, 8 blocks).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 complete listing (docs/fidelity/evidence/0x004F19C0-listing.txt): 33c0 b92859a700 / loop 833900 741b 83c14c 40 81f9286ca700 7cef / 68ece05400 e8ceddfbff 83c404 ebfe / 8b542408 56 83faff 7423 / 8d0cd2 57 8d0c4a 8d348de058a700 8d0cc0 8d0c48 8d3c8de058a700 b913000000 f3a5 5f / 8b742408 8d0cc0 8d0c48 c1e102 c7812859a7.. 01 89b1e058a7.. 89b1e458a7.. 5e c3.
// Behavior/CFG: scans the 64 descriptor slots for the first with a clear used dword (slot index*19+18), errors with Debug_Error("Unable to allocate parameter type") and spins when all are taken, otherwise optionally copies the 19-dword template slot named by arg1 (when arg1 != -1) into the new slot with rep movsd, then writes used=1, the name at +0 and the template index at +4, returning the slot index.
// Status/limits: STRUCTURALLY_VERIFIED (static only); the 19-dword slot layout and the used flag at dword 18 come from this listing and the 0x4F1BC0 writer; storage 0xA758E0 is owned by 0x4F1840; the slot count is 64 because the retail loop compares the cursor against 0x00A76C28, i.e. (0x00A76C28 - 0x00A75928) / 0x4C; runtime UNOBSERVED.
#include "igi/retail/core/retail_error.hpp"

#include <cstdint>

extern "C" {

extern std::uint32_t g_a758e0[0x4C0]; // 0x00A758E0, owned by 0x4F1840

std::int32_t Script_ParameterTypeRegister(std::uint32_t const name, std::int32_t const template_index)
{
    std::int32_t index = 0;                                              // 0x004F19C0: xor eax,eax
    for (;;) {
        if (g_a758e0[index * 19 + 18] == 0) {                            // 0x004F19C7: cmp dword [ecx],0
            break;                                                       // 0x004F19E7
        }
        index += 1;                                                      // 0x004F19CF: inc eax
        if (index >= 64) {                                               // 0x004F19D0: cmp ecx,0xa76c28; cursor = 0xA75928 + index*0x4C, limit-bound (0xA76C28-0xA75928)/0x4C = 64
            Debug_Error("Unable to allocate parameter type");            // 0x004F19D8
            for (;;) { }                                                 // 0x004F19E5: jmp $
        }
    }
    if (template_index != -1) {                                          // 0x004F19EC: cmp edx,0xffffffff
        for (std::int32_t word = 0; word < 19; word += 1) {              // 0x004F1A0C: mov ecx,0x13; rep movsd
            g_a758e0[index * 19 + word] = g_a758e0[template_index * 19 + word];
        }
    }
    g_a758e0[index * 19 + 18] = 1;                                       // 0x004F1A21: used flag
    g_a758e0[index * 19 + 0] = name;                                     // 0x004F1A2B
    g_a758e0[index * 19 + 1] = static_cast<std::uint32_t>(template_index); // 0x004F1A31
    return index;                                                        // 0x004F1A38
}

} // extern "C"
