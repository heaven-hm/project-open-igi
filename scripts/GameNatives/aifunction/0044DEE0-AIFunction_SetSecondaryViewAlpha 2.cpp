// IGI native provenance: AIFunction_SetSecondaryViewAlpha - 0x0044DEE0 - CFG score 83.3% (FAIL; structural only); runtime behavior not verified.
// Evidence: 0044DEE0-AIFunction_SetSecondaryViewAlpha.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AIFunction_SetSecondaryViewAlpha.h"
int AIFunction_SetSecondaryViewAlpha(long double context , int arg2, float arg3 )

// Sig: void AIFunction_SetSecondaryViewAlpha(int alpha)
// Note: Sets secondary-view detection gain. Param: alpha = engine factor.
{
    Profile_ValueSet(arg3, 0.0f);
    *(g_5bda58 + 0xc0) = context * 0.0174532924f;
    int item_18 = 1;
    int item_10 = 0;
    int item_c = 0x3ff00000;
    char* item_8 = &g_567c74;
    memcpy(arg2, &item_18, 0x18);
    return arg2;
}
