// IGI native provenance: SoundSys_SetMasterVolume_17 - 0x004ECF80 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004ECF80-SoundSys_SetMasterVolume_17.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "SoundSys_SetMasterVolume_17.h"
void* SoundSys_SetMasterVolume_17(float* context, void* arg2, float* arg3)

{
    float item_8 = -((arg3[2] * *(arg2 + 0x48) - *(arg2 + 0x50) * *arg3));
    long double fpu_val_7_9 = *(arg2 + 0x48) * arg3[1] - *arg3 * *(arg2 + 0x4c);
    *context = arg3[2] * *(arg2 + 0x4c) - *(arg2 + 0x50) * arg3[1];
    context[1] = item_8;
    context[2] = fpu_val_7_9;
    *context = *(arg2 + 0x18) + *context;
    context[1] = *(arg2 + 0x1c) + context[1];
    context[2] = *(arg2 + 0x20) + context[2];
    return arg2;
}
