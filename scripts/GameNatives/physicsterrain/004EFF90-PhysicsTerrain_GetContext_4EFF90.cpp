// IGI native provenance: PhysicsTerrain_GetContext_4EFF90 - 0x004EFF90 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004EFF90-PhysicsTerrain_GetContext_4EFF90.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "PhysicsTerrain_GetContext_4EFF90.hpp"

int PhysicsTerrain_GetContext_4EFF90(int* arg1, int* arg2)

{
    int* context_val = *arg2;
    *context_val = *arg1;
    int result = arg1[1];
    context_val[1] = result;
    return result;
}
