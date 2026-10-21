// IGI native provenance: AIFunction_SetAnimationInterval - 0x0044EAF0 - CFG score 93.3% (PASS; structural only); runtime behavior not verified.
// Evidence: 0044EAF0-AIFunction_SetAnimationInterval.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AIFunction_SetAnimationInterval.h"
int AIFunction_SetAnimationInterval(long double context , long double arg2 , int arg3, float arg4 )

// Sig: void AIFunction_SetAnimationInterval(int entryIndex, int interval)
// Note: Sets playback INTERVAL for an animation entry. Params: entryIndex =
// animation entry added via AddAnimationEntry, interval = frame/tick interval.
// Use when tuning scripted animations.
{
    float item_2c = 0.0f;
    float item_30 = arg4;
    Profile_ValueSet(item_30, item_2c);
    *(g_5bda58 + 0x3b58) = context * 30.0;
    Profile_ValueSet(arg4, 1.40129846e-45.0f);
    *(g_5bda58 + 0x3b5c) = arg2 * 30.0;
    int temp_1 = *(g_5bda58 + 0x3b5c);
    item_30 = Level_SetMission_5();
    unsigned int data_3 = Sys_FreeMemory_14(item_30, temp_1);
    void* data_4 = g_5bda58;
    int temp_2 = *(data_4 + 0x3b58);
    int item_18 = 2;
    int item_10 = 0;
    *(data_4 + 0x3b60) = data_3 + temp_2;
    int item_c = 0x40000000;
    char* item_8 = &g_567c74;
    memcpy(arg3, &item_18, 0x18);
    return arg3;
}
