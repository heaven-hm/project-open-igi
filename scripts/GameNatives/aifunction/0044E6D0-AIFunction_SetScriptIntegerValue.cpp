// IGI native provenance: AIFunction_SetScriptIntegerValue - 0x0044E6D0 - CFG score 88.9% (PASS; structural only); runtime behavior not verified.
// Evidence: 0044E6D0-AIFunction_SetScriptIntegerValue.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "0044E6D0-AIFunction_SetScriptIntegerValue.hpp"
int AIFunction_SetScriptIntegerValue(int context, float arg2 )

// Sig: void AIFunction_SetScriptIntegerValue(int varId, int value)
// Note: Writes an AI-script integer variable. Params: varId = script variable
// index, value = new integer. Use for custom AI state communication between
// scripts.
{
    int data_val = Script_GetArg(arg2, 0);
    int item_2c = 1;
    float item_30 = arg2;
    int data_1 = Script_GetArg(item_30, item_2c);
    int entity_val = g_5bda58;
    int item_18 = 1;
    *(entity_val + (data_val << 2) + 0x3ad4) = data_1;
    int item_10 = 0;
    int item_c = 0x3ff00000;
    char* item_8 = &g_567c74;
    memcpy(context, &item_18, 0x18);
    return context;
}
