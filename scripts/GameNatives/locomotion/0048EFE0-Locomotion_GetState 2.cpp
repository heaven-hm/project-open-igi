// IGI native provenance: Locomotion_GetState - 0x0048EFE0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0048EFE0-Locomotion_GetState.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Locomotion_GetState.h"
int Locomotion_GetState(void* context, int arg2)

{
    int result = AI_CheckSector_18(*(context + 0x14));
    int entity_2 = *(*((*(context + 0x3d38) << 2) + &g_541658) + (arg2 << 2));

    if (entity_2 != 0xffffffff)
    {
        result = result == entity_2;
        return result;
    }

    result = 0;
    return result;
}
