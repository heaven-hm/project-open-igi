// IGI native provenance: PhysicsTerrain_GetContext_4EFEE0 - 0x004EFEE0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004EFEE0-PhysicsTerrain_GetContext_4EFEE0.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "PhysicsTerrain_GetContext_4EFEE0.h"

int* PhysicsTerrain_GetContext_4EFEE0(int arg1, int* arg2)

{
    __builtin_memcpy(*arg2, arg1, 0x14);
    return arg2;
}
