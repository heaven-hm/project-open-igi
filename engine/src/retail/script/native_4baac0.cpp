// Native: Script_QHashNodeCreate (placeholder until VERIFIED); retail VA 0x004BAAC0-0x004BAB27 (104 bytes, 1 block, 0 edges).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 complete listing (56 57 6a01 e8.. 6a04 6a1c e8.. 8bf0 b907000000 33c0 8bfe f3ab 8b442418 8b0d883ea400 56 50 51 e8.. 8b542428 8b4c2430 894614 8b44242c 895610 8b542434 894618 8b442438 894e0c 8b4c243c 83c418 8916 894604 894e08 e86b62ffff 5f 5e c3); seven arguments confirmed from the 0x4B7E9C call site (seven pushes, add esp,0x1c); callees 0x4B0D60/0x4B0C60/0x4C0560/0x4B0D90 all landed.
// Behavior/CFG: pops the flag stack with 1, allocates 28 bytes via Mem_Alloc(0x1C,4), zeroes them with seven dword stores, registers the name through 0x4C0560(g_a43e88, ARGUMENT 1, record) and stores the returned key at +0x14, then fills +0x10=arg2, +0x18=arg3, +0xC=arg4, +0=arg5, +4=arg6, +8=arg7 and pushes the flag stack back; a single block with no branches.
// Status/limits: BUILD_VERIFIED (static only); the retail returns whatever eax the final flag-stack push leaves (callers discard it - confirmed at 0x4B7E9C `add esp,0x1c; ret`), so this is transcribed as void and the implicit value is documented rather than invented; the zeroing is seven dword stores instead of rep stosd; runtime UNOBSERVED. THE REGISTRY ARGUMENT IS ARGUMENT 1, NOT ARGUMENT 3, and the proof is the deferred stack cleanup rather than a decompiler reading. The single `add esp,0x18` at 0x004BAB15 is 0x4 + 0x8 + 0xC - the one argument of 0x004B0D60, the two of 0x004B0C60 and the three of 0x004C0560 - so none of those pushes had been popped when the code reached 0x004BAADF. That makes esp = esp0-0x14 there (push esi, push edi, push 1, push 4, push 0x1C), so `mov eax,[esp+0x18]` at 0x004BAADF reads esp0+4, which is ARGUMENT 1. The cross-check that makes it certain rather than probable: after the three pushes for 0x004C0560 esp = esp0-0x20, and the six later loads at [esp+0x28], [esp+0x2c], [esp+0x30], [esp+0x34], [esp+0x38] and [esp+0x3c] map exactly onto arguments 2, 3, 4, 5, 6 and 7 - the frame closes with seven arguments and zero slack, so the offsets are pinned. The behaviour confirms it independently: with argument 1 as the registry value, Script_QHashNodeRegister(name, id) registers the NAME in the hash and stores the raw id at +0x18, which is what a named-value registry does; with the previous argument 3 the hash was keyed by the id instead of the name. The parameter types follow the same evidence and are the reverse of the previous declaration: argument 1 is the registry value and is typed char const* to match Script_QHashSymbolRegister's own `char const* value` parameter, while argument 3 is an opaque value stored raw at +0x18 and is typed void const*, which is why the store is a const_cast of a pointer that is never dereferenced.
#include "igi/retail/mem/mem_alloc.hpp"

#include <cstdint>

extern "C" {

extern "C" void* g_a43e88;                                                                     // 0x00A43E88, owned by 0x4BACD0
void Mem_FlagStackPop(std::uint8_t flag);                                    // 0x004B0D60
void Mem_FlagStackPush();                                                    // 0x004B0D90
void* Script_QHashSymbolRegister(void* list, char const* value, void* owner); // 0x004C0560

void Script_QHashNodeCreate(char const* const arg1, void* const arg2, void const* const arg3,
                                   void* const arg4, void* const arg5, void* const arg6,
                                   void* const arg7)
{
    Mem_FlagStackPop(1);                                                     // 0x004BAAC2/0x004BAAC4
    std::uint8_t* const record = static_cast<std::uint8_t*>(igi::retail::mem::Mem_Alloc(0x1C, 4)); // 0x004BAACD
    for (std::int32_t i = 0; i < 7; i += 1)                                         // 0x004BAAD4..0x004BAADD: rep stosd
        *reinterpret_cast<std::int32_t*>(record + static_cast<std::ptrdiff_t>(i) * 4) = 0;
    void* const key = Script_QHashSymbolRegister(g_a43e88, arg1, record);     // 0x004BAAEC: argument 1, not 3 - see the provenance
    *reinterpret_cast<void**>(record + 0x14) = key;                                  // 0x004BAAF9
    *reinterpret_cast<void**>(record + 0x10) = arg2;                                 // 0x004BAB00
    *reinterpret_cast<void**>(record + 0x18) = const_cast<void*>(arg3);              // 0x004BAB07: arg3 stored raw
    *reinterpret_cast<void**>(record + 0x0C) = arg4;                                 // 0x004BAB0E
    *reinterpret_cast<void**>(record) = arg5;                                        // 0x004BAB18
    *reinterpret_cast<void**>(record + 4) = arg6;                                    // 0x004BAB1A
    *reinterpret_cast<void**>(record + 8) = arg7;                                    // 0x004BAB1D
    Mem_FlagStackPush();                                                     // 0x004BAB20
}

} // extern "C"
