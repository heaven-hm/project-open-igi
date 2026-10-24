// IGI native provenance: Cabinet_Open - 0x00443DA0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00443DA0-Cabinet_Open.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "00443DA0-Cabinet_Open.hpp"
int Cabinet_Open(int* self )

{
    void* result_val = *self;
    return FX_FireBallUpdate_9(result_val, result_val + 0x404);
}
