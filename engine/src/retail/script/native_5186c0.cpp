// Native: Script_NodeCriteriaRegister (placeholder until VERIFIED); retail VA 0x005186C0-0x00518713 (84 bytes, 6 blocks).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 complete listing (33c9 b8f800a800 803800 741f 83c008 41 3df801a800 7cf0 8b442404 50 68dcec5400 e8ca70f9ff 83c408 ebfe / 8b442404 56 be01000000 c604cdf800.. d3e6 8904cdfc00.. 56 50 e80301faff 83c408 8bc6 5e c3); failure string "Unable to register NodeCriteria %s" at 0x0054ECDC; callee 0x4B8810 landed.
// Behavior/CFG: scans the 32 eight-byte slots at 0x00A800F8 for a free slot; if none is free it calls Debug_Error("Unable to register NodeCriteria %s", arg) and spins (0x005186E9). On a free slot it marks the used byte, stores the argument in the slot's dword, builds the mask 1 << index, calls 0x4B8810(arg, mask) and returns the mask.
// Status/limits: BUILD_VERIFIED (static only); the 32-slot table is port-owned storage declared here because this native is both its writer and reader, the scan bound 0x00A801F8 is exclusive as retail, and the fail-closed spin is preserved; runtime UNOBSERVED.
#include "igi/retail/core/retail_error.hpp"

#include <cstdint>

extern "C" void Script_QHashNodeRegister(void* arg1, void* value);

extern "C" {

// Retail storage at 0x00A800F8: 32 slots of 8 bytes {used byte at +0, value dword at +4}.
std::uint8_t g_a800f8[32 * 8] = {};

std::int32_t Script_NodeCriteriaRegister(void* const value)
{
    std::int32_t index = 0;                                                     // 0x005186C0: xor ecx,ecx
    for (;;) {
        std::uint8_t* const slot = g_a800f8 + static_cast<std::ptrdiff_t>(index) * 8; // 0x005186C2: mov eax,0xa800f8 + index*8
        if (slot[0] == 0) {                                                     // 0x005186C7: cmp byte [eax],0; je
            slot[0] = 1;                                                        // 0x005186F5
            *reinterpret_cast<std::int32_t*>(slot + 4) = static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(value)); // 0x005186FF
            std::uint32_t const mask = 1U << index;                             // 0x005186FD: shl esi,cl
            Script_QHashNodeRegister(value, reinterpret_cast<void*>(static_cast<std::uintptr_t>(mask))); // 0x00518708
            return static_cast<std::int32_t>(mask);                             // 0x00518710: mov eax,esi
        }
        index += 1;                                                             // 0x005186CF: inc ecx
        if (index >= 32) {                                                      // 0x005186D0: cmp eax,0xa801f8; jl
            Debug_Error("Unable to register NodeCriteria %s", value);            // 0x005186E1
            for (;;) { }                                                        // 0x005186E9: jmp $
        }
    }
}

} // extern "C"
