// Native: Script_TaskContainer_Alloc (placeholder until VERIFIED); retail VA 0x004F1950-0x004F1978 (41 bytes, 1 block, 0 edges).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 disassembly (56 8b742408 8bc6 6a04 c1e004 83c00c 50 e8fbf2fbff 83c408 8d480c 897004 c70000000000 894808 5e c3); callee 0x004B0C60 = Mem_Alloc (VERIFIED, mem/mem_alloc.cpp).
// Behavior/CFG: size = count*16 + 12; allocates via igi::retail::mem::Mem_Alloc(size, 4); writes header { [0]=0, [4]=count, [8]=base+12 } and returns the block pointer. Single block.
// Status/limits: BUILD_VERIFIED (static only); flags argument 4 and the header layout are retail-derived, the allocator itself is already VERIFIED; runtime UNOBSERVED.
#include "igi/retail/mem/mem_alloc.hpp"

#include <cstdint>

extern "C" {

void* Script_TaskContainer_Alloc(std::int32_t const count)
{
    std::int32_t const size = (count << 4) + 0x0C;                 // 0x004F1959: shl eax,4; add eax,0xc
    std::uint8_t* const base = static_cast<std::uint8_t*>(igi::retail::mem::Mem_Alloc(size, 4)); // 0x004F1957 push 4; 0x004F1960 call 0x4B0C60
    *reinterpret_cast<std::int32_t*>(base + 4) = count;            // 0x004F196B: mov [eax+4],esi
    *reinterpret_cast<std::int32_t*>(base) = 0;                    // 0x004F196E: mov dword [eax],0
    *reinterpret_cast<std::uint8_t**>(base + 8) = base + 0x0C;     // 0x004F1968/0x004F1974: lea ecx,[eax+0xc]; mov [eax+8],ecx
    return base;                                                   // 0x004F1977 pop esi; ret
}

} // extern "C"
