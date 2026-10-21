// IGI native provenance: AIFunction_SetScriptRealValue - 0x0044E790 - CFG score 88.9% (PASS; structural only); runtime behavior not verified.
// Evidence: 0044E790-AIFunction_SetScriptRealValue.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AIFunction_SetScriptRealValue.h"
int AIFunction_SetScriptRealValue(long double context , int arg2, float arg3 )

// Sig: void AIFunction_SetScriptRealValue(int varId, float value)
// Note: Writes an AI-script real (float) variable. Params: varId = script
// variable index, value = float. Pairs with AIFunction_GetScriptRealValue.
{
    int data_val = Script_GetArg(arg3, 0);
    float item_2c = 1.40129846e-45.0f;
    float item_30 = arg3;
    Profile_ValueSet(item_30, item_2c);
    *(g_5bda58 + (data_val << 3) + 0x3ae8) = context;
    int item_18 = 1;
    int item_10 = 0;
    int item_c = 0x3ff00000;
    char* item_8 = &g_567c74;
    memcpy(arg2, &item_18, 0x18);
    return arg2;
}
