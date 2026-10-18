// IGI native provenance: PhysicsTerrain_GetContext_4EF9B0 - 0x004EF9B0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004EF9B0-PhysicsTerrain_GetContext_4EF9B0.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "PhysicsTerrain_GetContext_4EF9B0.hpp"

int* PhysicsTerrain_GetContext_4EF9B0(int arg1, int* arg2)

{
    __builtin_memcpy(arg1, *arg2, 0x2c);
    return arg2;
}
