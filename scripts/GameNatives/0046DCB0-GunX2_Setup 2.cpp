// IGI native provenance: GunX2_Setup - 0x0046DCB0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0046DCB0-GunX2_Setup.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "GunX2_Setup.h"

void* GunX2_Setup(void* self)

{
    Qhash_Init(1);
    *(self + 0x20) = Sys_CheckHeap_22(self + 0x24);
    return Qhash_Reset();
}
