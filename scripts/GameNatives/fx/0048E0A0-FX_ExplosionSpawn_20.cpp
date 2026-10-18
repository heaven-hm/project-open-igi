// IGI native provenance: FX_ExplosionSpawn_20 - 0x0048E0A0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0048E0A0-FX_ExplosionSpawn_20.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "0048E0A0-FX_ExplosionSpawn_20.hpp"
int* FX_ExplosionSpawn_20(void* context, int* arg2)

{
    void* source_1 = arg2[0xa] + 8;
    unsigned int data_val = Frames_GetUpdateCounter();
    int item_18 = *(source_1 + 0x1c) - data_val;
    char* item_1c = "Look at event, %d";
    int item_20 = 0xffffffff;
    void* item_24 = context;

    if (*(source_1 + 0x1c) < 0)
        *(source_1 + 0x1c) = Frames_GetUpdateCounter() - *(source_1 + 0x18) * -30.0f;

    int* result = AI_UpdateOrders_3(context, source_1, 0.0349065848f, 0);

    if (result)
    {
        result = Frames_GetUpdateCounter();

        if (result > *(source_1 + 0x1c))
            return AI_CombatTarget_2(context, arg2);
    }

    return result;
}
