// Native: Script_QHashNodeCreate_4BAAC0 (placeholder until VERIFIED); retail VA 0x004BAAC0-0x004BAB27 (104 bytes, 1 block, 0 edges).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 complete listing (56 57 6a01 e8.. 6a04 6a1c e8.. 8bf0 b907000000 33c0 8bfe f3ab 8b442418 8b0d883ea400 56 50 51 e8.. 8b542428 8b4c2430 894614 8b44242c 895610 8b542434 894618 8b442438 894e0c 8b4c243c 83c418 8916 894604 894e08 e86b62ffff 5f 5e c3); seven arguments confirmed from the 0x4B7E9C call site (seven pushes, add esp,0x1c); callees 0x4B0D60/0x4B0C60/0x4C0560/0x4B0D90 all landed.
// Behavior/CFG: pops the flag stack with 1, allocates 28 bytes via Mem_Alloc(0x1C,4), zeroes them with seven dword stores, registers the name through 0x4C0560(g_a43e88, arg3, record) and stores the returned key at +0x14, then fills +0x10=arg2, +0x18=arg3, +0xC=arg4, +0=arg5, +4=arg6, +8=arg7 and pushes the flag stack back; a single block with no branches.
// Status/limits: TRANSCRIBED (static only); the retail returns whatever eax the final flag-stack push leaves (callers discard it - confirmed at 0x4B7E9C `add esp,0x1c; ret`), so this is transcribed as void and the implicit value is documented rather than invented; the zeroing is seven dword stores instead of rep stosd; runtime UNOBSERVED.
#include "igi/retail/mem/mem_alloc.hpp"

#include <cstdint>

extern "C" {

extern "C" void* g_a43e88;                                                                     // 0x00A43E88, owned by 0x4BACD0
void Mem_FlagStackPop_4B0D60(std::uint8_t flag);                                    // 0x004B0D60
void Mem_FlagStackPush_4B0D90();                                                    // 0x004B0D90
void* Script_QHashSymbolRegister_4C0560(void* list, char const* value, void* owner); // 0x004C0560

void Script_QHashNodeCreate_4BAAC0(void* const arg1, void* const arg2, char const* const arg3,
                                   void* const arg4, void* const arg5, void* const arg6,
                                   void* const arg7)
{
    Mem_FlagStackPop_4B0D60(1);                                                     // 0x004BAAC2/0x004BAAC4
    std::uint8_t* const record = static_cast<std::uint8_t*>(igi::retail::mem::Mem_Alloc(0x1C, 4)); // 0x004BAACD
    for (std::int32_t i = 0; i < 7; i += 1)                                         // 0x004BAAD4..0x004BAADD: rep stosd
        *reinterpret_cast<std::int32_t*>(record + static_cast<std::ptrdiff_t>(i) * 4) = 0;
    void* const key = Script_QHashSymbolRegister_4C0560(g_a43e88, arg3, record);     // 0x004BAAEC
    *reinterpret_cast<void**>(record + 0x14) = key;                                  // 0x004BAAF9
    *reinterpret_cast<void**>(record + 0x10) = arg2;                                 // 0x004BAB00
    *reinterpret_cast<void**>(record + 0x18) = const_cast<void*>(static_cast<void const*>(arg3)); // 0x004BAB07: arg3 stored raw
    *reinterpret_cast<void**>(record + 0x0C) = arg4;                                 // 0x004BAB0E
    *reinterpret_cast<void**>(record) = arg5;                                        // 0x004BAB18
    *reinterpret_cast<void**>(record + 4) = arg6;                                    // 0x004BAB1A
    *reinterpret_cast<void**>(record + 8) = arg7;                                    // 0x004BAB1D
    Mem_FlagStackPush_4B0D90();                                                     // 0x004BAB20
}

} // extern "C"
