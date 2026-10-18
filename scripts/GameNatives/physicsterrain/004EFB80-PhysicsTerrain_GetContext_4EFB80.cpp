// IGI native provenance: PhysicsTerrain_GetContext_4EFB80 - 0x004EFB80 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004EFB80-PhysicsTerrain_GetContext_4EFB80.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "PhysicsTerrain_GetContext_4EFB80.hpp"

int PhysicsTerrain_GetContext_4EFB80(int* arg1, int* arg2)

{
    int* context_val = *arg2;
    *arg1 = *context_val;
    int result = context_val[1];
    arg1[1] = result;
    arg1[2] = context_val[2];
    return result;
}
