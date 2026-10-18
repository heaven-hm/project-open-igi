// IGI native provenance: PhysicsTerrain_GetContext_4EFB60 - 0x004EFB60 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004EFB60-PhysicsTerrain_GetContext_4EFB60.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "PhysicsTerrain_GetContext_4EFB60.h"

int PhysicsTerrain_GetContext_4EFB60(int* arg1, int* arg2)

{
    int* data_val = *arg2;
    *data_val = *arg1;
    data_val[1] = arg1[1];
    int result = arg1[2];
    data_val[2] = result;
    return result;
}
