// IGI native provenance: PhysicsTerrain_GetContext_4F0080 - 0x004F0080 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004F0080-PhysicsTerrain_GetContext_4F0080.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "PhysicsTerrain_GetContext_4F0080.hpp"

int* PhysicsTerrain_GetContext_4F0080(int arg1, int* arg2)

{
    __builtin_memcpy(arg1, *arg2, 0x18);
    return arg2;
}
