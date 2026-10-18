// IGI native provenance: PhysicsTerrain_GetContext_4EFCB0 - 0x004EFCB0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004EFCB0-PhysicsTerrain_GetContext_4EFCB0.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "PhysicsTerrain_GetContext_4EFCB0.h"

int* PhysicsTerrain_GetContext_4EFCB0(int arg1, int* arg2)

{
    __builtin_memcpy(arg1, *arg2, 0x28);
    return arg2;
}
