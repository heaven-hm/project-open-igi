// IGI native provenance: Human_TransformBoneToWorld - 0x0048A340 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0048A340-FX_ExplosionSpawn_16.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "FX_ExplosionSpawn_16.h"
float* Human_TransformBoneToWorld(float* context, void* arg2, int arg3)

{
    int* entity_3 = *(arg2 + 0x70) + arg3 * 0x84 + 0x18;
    float temp_1 = *entity_3;
    float temp_2 = entity_3[1];
    float entity_4 = entity_3[2];
    float item_c = entity_4 * *(arg2 + 0x90) + temp_2 * *(arg2 + 0x84) + temp_1 * *(arg2 + 0x78);
    float item_18_1 = item_c;
    long double fpu_val_7_15 =
        entity_4 * *(arg2 + 0x94) + temp_2 * *(arg2 + 0x88) + temp_1 * *(arg2 + 0x7c) + *(arg2 + 0x518);
    long double fpu_val_7_17 =
        entity_4 * *(arg2 + 0x98) + temp_2 * *(arg2 + 0x8c) + temp_1 * *(arg2 + 0x80) + *(arg2 + 0x51c);
    *context = item_c + *(arg2 + 0x514);
    context[1] = fpu_val_7_15;
    context[2] = fpu_val_7_17;
    return context;
}
