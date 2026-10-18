// IGI native provenance: GunX2_Open - 0x0046DC20 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0046DC20-GunX2_Open.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "0046DC20-GunX2_Open.hpp"
void* GunX2_Open(void* self)

{
    *(self + 0x20) = 0;
    *(self + 0x24) = 0;
    *(self + 0x44) = 0;
    return self;
}
