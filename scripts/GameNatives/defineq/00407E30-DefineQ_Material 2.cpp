// IGI native provenance: DefineQMaterial - 0x00407E30 - CFG score 95.2% (PASS; structural only); runtime behavior not verified.
// Evidence: 00407E30-DefineQ_Material.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "DefineQ_Material.h"
int DefineQMaterial(long double context , long double arg2 , long double arg3 , int arg4, int arg5 )

// Sig: void DefineQMaterial(...)
// Note: Defines a Q-material (surface material) entry while a material
// definition block is parsed. Called BY the engine's own define-block parser -
// call only from material .qsc definition contexts; arg layout follows the
// Define block grammar (registration argc unknown).
{
    float source_val = arg5;
    Profile_ValueSet(source_val, 0.0f);
    int data_val = g_53851c;
    float item_2c = 1.40129846e-45.0f;
    float item_30 = source_val;
    *(data_val * 0x268 + &g_569210) = context;
    Profile_ValueSet(item_30, item_2c);
    *(g_53851c * 0x268 + &g_56921c) = arg2;
    Profile_ValueSet(source_val, 2.80259693e-45.0f);
    *(g_53851c * 0x268 + &g_569218) = arg3;
    *(g_53851c * 0x268 + &g_569224) = Script_GetArg(source_val, 3) * 9.53674316e-07.0f;
    *(g_53851c * 0x268 + &g_569228) = Script_GetArg(source_val, 4) * 9.53674316e-07.0f;
    int data_12 = Script_GetArg(source_val, 5);
    int data_13 = g_53851c;
    int item_18 = 1;
    int item_10 = 0;
    int item_c = 0x3ff00000;
    char* item_8 = &g_567c74;
    *(data_13 * 0x268 + &g_56922c) = data_12 * 9.53674316e-07.0f;
    memcpy(arg4, &item_18, 0x18);
    return arg4;
}
