// IGI native provenance: Script_EvaluateCondition_28 - 0x004FBF90 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004FBF90-Script_EvaluateCondition_28.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Script_EvaluateCondition_28.h"
int Script_EvaluateCondition_28(void* context, float arg2, float arg3, float arg4)

{
    int item_c = 0;
    void* entity_val = *(context + 0x1b4);
    int item_8 = 0;
    int item_4 = 0;
    float item_18 = 0.0f;
    int item_14 = 0;
    long double fpu_val_7_2 = -((arg2 * *(entity_val + 8)));
    long double fpu_val_6_3 = fpu_val_7_2 * *(context + 0x8c);
    item_18 = fpu_val_7_2 * *(context + 0x88);
    float item_14_1 = fpu_val_6_3;
    float item_10 = fpu_val_7_2 * *(context + 0x90);
    ForceAccumulator_Add(context + 0xf0, &item_18);
    int result = ForceAccumulator_Integrate(context + 0xf0, context + 0x70);
    float entity_3 = *(context + 0x128);
    long double fpu_val_7_7 = *(context + 0x124) * arg4;
    *(context + 0x120) = *(context + 0x120) * arg4;
    *(context + 0x124) = fpu_val_7_7;
    *(context + 0x128) = entity_3 * arg4;
    float entity_6 = *(context + 0x110);
    long double fpu_val_7_13 = *(context + 0x10c) * arg3;
    *(context + 0x108) = *(context + 0x108) * arg3;
    *(context + 0x10c) = fpu_val_7_13;
    *(context + 0x110) = entity_6 * arg3;
    SoundSys_InitSoundSys_17(context + 0xf0);
    return result;
}
