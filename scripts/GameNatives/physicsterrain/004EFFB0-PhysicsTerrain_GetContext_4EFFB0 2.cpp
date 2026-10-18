// IGI native provenance: PhysicsTerrain_GetContext_4EFFB0 - 0x004EFFB0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004EFFB0-PhysicsTerrain_GetContext_4EFFB0.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "PhysicsTerrain_GetContext_4EFFB0.h"

int* PhysicsTerrain_GetContext_4EFFB0(int* arg1, int* arg2)

{
    int* context_val = *arg2;
    *arg1 = *context_val;
    arg1[1] = context_val[1];
    return arg1;
}
