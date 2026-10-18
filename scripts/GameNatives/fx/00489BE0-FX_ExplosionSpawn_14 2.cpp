// IGI native provenance: FX_ExplosionSpawn_14 - 0x00489BE0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00489BE0-FX_ExplosionSpawn_14.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "FX_ExplosionSpawn_14.h"
int* FX_ExplosionSpawn_14(void* context, int arg2)

{
    int context_val;
    int item_4 = context_val;
    int* result = *(context + 0x18);

    for (int* i = *result; i; i = *i)
    {
        if (*(result + 0x2a) && *result[2] == arg2)
            return result;

        result = i;
    }

    result = *(context + 0x24);
    int* result_1 = *result;

    if (!result_1)
        return nullptr;

    while (true)
    {
        if (*(result + 0x2a) && *result[2] == arg2)
            return result;

        result = result_1;
        result_1 = *result_1;

        if (!result_1)
            return nullptr;
    }
}
