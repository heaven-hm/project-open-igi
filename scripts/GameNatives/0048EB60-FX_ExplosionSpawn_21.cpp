// IGI native provenance: FX_ExplosionSpawn_21 - 0x0048EB60 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0048EB60-FX_ExplosionSpawn_21.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "0048EB60-FX_ExplosionSpawn_21.hpp"
int* FX_ExplosionSpawn_21(void* context, int* arg2)

{
    float* source_1 = arg2[0xa] + 8;
    unsigned int data_val = Frames_GetUpdateCounter();
    int item_18 = source_1[1] - data_val;
    char* item_1c = "Fall flat - %d";
    int item_20 = 0xffffffff;
    void* item_24 = context;
    AI_WriteChannel(context, 2, 1.0f);

    if (source_1[1] < 0)
        source_1[1] = Frames_GetUpdateCounter() - *source_1 * -30.0f;

    int* result = Frames_GetUpdateCounter();

    if (result <= source_1[1])
        return result;

    return AI_CombatTarget_2(context, arg2);
}
