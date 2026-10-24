// Native: Script_NodeSizeTable_Set (placeholder until VERIFIED); retail VA 0x004F9460-0x004F94C3 (100 bytes, 11 blocks).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 complete listing captured in 0x00508FA0-disassembly.txt; table 0x00A77228..0x00A77248 (4 slots of 8 bytes: {int16 size, int32 value}); failure string "NO FREE NODESIZE ELEMENTS" at 0x0054E374; Debug_Error = verified 0x004AF7B0.
// Behavior/CFG: scans 4 table slots from 0x00A77228; remembers the first slot whose size word is 0x181 while none has matched; on an exact size match it overwrites that slot's value and returns; if the scan ends with no remembered slot it calls Debug_Error("NO FREE NODESIZE ELEMENTS") then spins (0x004F94C2 jmp $); otherwise it claims the remembered slot, stores the size word and value, and returns.
// Status/limits: TRANSCRIBED (static only); loop order, the 0x181 "free slot" sentinel, the exclusive bound 0x00A77248 and the fail-closed spin are retail; runtime UNOBSERVED.
#include "igi/retail/core/retail_error.hpp"

#include <cstdint>

extern "C" {

void Script_NodeSizeTable_Set(std::int16_t const size, std::int32_t const value)
{
    std::int32_t free_slot = -1;                                        // 0x004F9466: or edx,-1
    std::int32_t index = 0;                                             // 0x004F9469: xor ecx,ecx
    std::uintptr_t const base = 0x00A77228U;                            // 0x004F946B: mov eax,0xa77228
    for (;;) {
        std::uintptr_t const slot = base + static_cast<std::uintptr_t>(index) * 8U;
        if (free_slot == -1 && *reinterpret_cast<std::int16_t*>(slot) == 0x0181)  // 0x004F9470..0x004F947C
            free_slot = index;
        if (*reinterpret_cast<std::int16_t*>(slot) == size) {           // 0x004F947E: cmp word [eax],si
            *reinterpret_cast<std::int32_t*>(slot + 4) = value;         // 0x004F94AD: mov [ecx*8+0xA7722C],eax
            return;
        }
        index += 1;                                                     // 0x004F9486: inc ecx
        if (!(base + static_cast<std::uintptr_t>(index) * 8U < 0x00A77248U)) // 0x004F9487: cmp eax,0xA77248; jl
            break;
    }
    if (free_slot == -1) {                                              // 0x004F948E: cmp edx,-1; je
        Debug_Error("NO FREE NODESIZE ELEMENTS");                       // 0x004F94BA: call 0x4AF7B0
        for (;;) { }                                                    // 0x004F94C2: jmp $
    }
    *reinterpret_cast<std::int16_t*>(base + static_cast<std::uintptr_t>(free_slot) * 8U) = size;      // 0x004F9497
    *reinterpret_cast<std::int32_t*>(base + static_cast<std::uintptr_t>(free_slot) * 8U + 4U) = value; // 0x004F949F
}

} // extern "C"
