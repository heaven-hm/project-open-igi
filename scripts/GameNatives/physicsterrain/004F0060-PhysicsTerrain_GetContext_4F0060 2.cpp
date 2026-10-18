// IGI native provenance: PhysicsTerrain_GetContext_4F0060 - 0x004F0060 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004F0060-PhysicsTerrain_GetContext_4F0060.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "PhysicsTerrain_GetContext_4F0060.h"

int* PhysicsTerrain_GetContext_4F0060(int arg1, int* arg2)

{
    __builtin_memcpy(*arg2, arg1, 0x18);
    return arg2;
}
