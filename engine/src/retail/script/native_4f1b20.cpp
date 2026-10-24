// Native: Script_ParameterEventSlotAlloc (placeholder until VERIFIED); retail VA 0x004F1B20-0x004F1B48 (41 bytes, 3 blocks).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 complete listing (33c0 8a88c858a700 84c9 7415 40 83f810 7cf0 682ce15400 e874dcfbff 83c404 ebfe / c680c858a7.. 01 c3); string 0x54E12C "Unable to allocate parameter event"; callee 0x4AF7B0 Debug_Error landed.
// Behavior/CFG: scans the 16-byte slot array at 0x00A758C8 for the first zero byte, marks it 1 and returns the index; when all 16 are taken it calls Debug_Error("Unable to allocate parameter event") and spins (0x004F1B3F).
// Status/limits: TRANSCRIBED (static only); declares the 16-slot array at the size proven by the loop bound (cmp eax,0x10) and preserves the fail-closed spin; runtime UNOBSERVED.
#include "igi/retail/core/retail_error.hpp"

#include <cstdint>

extern "C" {

std::uint8_t g_a758c8[16] = {}; // 0x00A758C8, 16 slots (loop bound cmp eax,0x10)

std::int32_t Script_ParameterEventSlotAlloc()
{
    std::int32_t index = 0;                                   // 0x004F1B20: xor eax,eax
    for (;;) {
        if (g_a758c8[index] == 0) {                           // 0x004F1B22: mov cl,[eax+0xa758c8]
            g_a758c8[index] = 1;                              // 0x004F1B41
            return index;                                     // 0x004F1B48: ret
        }
        index += 1;                                           // 0x004F1B2C: inc eax
        if (index >= 16) {                                    // 0x004F1B2D: cmp eax,0x10
            Debug_Error("Unable to allocate parameter event"); // 0x004F1B32
            for (;;) { }                                      // 0x004F1B3F: jmp $
        }
    }
}

} // extern "C"
