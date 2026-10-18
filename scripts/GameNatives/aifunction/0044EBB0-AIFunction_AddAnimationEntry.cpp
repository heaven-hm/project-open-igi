// IGI native provenance: AIFunction_AddAnimationEntry - 0x0044EBB0 - CFG score 90.5% (PASS; structural only); runtime behavior not verified.
// Evidence: 0044EBB0-AIFunction_AddAnimationEntry.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "0044EBB0-AIFunction_AddAnimationEntry.hpp"
int AIFunction_AddAnimationEntry(long double context , int arg2, float arg3 )

// Sig: void AIFunction_AddAnimationEntry(int animationId, int data)
// Note: Adds an animation ENTRY to this AI's available set (builds the
// pick-list GetAnimationToPlay draws from). Params: animationId = animation
// resource id, data = entry config value. Use in AIEVENT_CREATE before playing
// custom animations.
{
    float item_30;
    int item_18;

    if (*(g_5bda58 + 0x3b64) >= 5)
    {
        int item_24 = 5;
        Warning_Show("Maximum number of animation entries (#%d) reached");
        int item_2c = 0;
        item_30 = arg3;
        Script_GetArg(item_30, item_2c);
        Profile_ValueSet(arg3, 1.40129846e-45.0f);
        item_18 = 2;
        int item_10 = 0;
        int item_c = 0x40000000;
        char* item_8 = &g_567c74;
        memcpy(arg2, &item_18, 0x18);
        return arg2;
    }

    int data_1 = Script_GetArg(arg3, 0);
    void* entity_1 = g_5bda58;
    float item_2c_1 = 1.40129846e-45.0f;
    item_30 = arg3;
    *((*(entity_1 + 0x3b64) << 4) + entity_1 + 0x3b08) = data_1;
    Profile_ValueSet(item_30, item_2c_1);
    void* data_2 = g_5bda58;
    item_18 = 2;
    int item_10_1 = 0;
    int item_c_1 = 0x40000000;
    char* item_8_1 = &g_567c74;
    *(((*(data_2 + 0x3b64) + 0x3b1) << 4) + data_2) = context;
    void* data_3 = g_5bda58;
    *(data_3 + 0x3b64) += 1;
    memcpy(arg2, &item_18, 0x18);
    return arg2;
}
