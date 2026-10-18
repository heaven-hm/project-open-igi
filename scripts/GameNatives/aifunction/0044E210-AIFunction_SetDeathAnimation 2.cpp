// IGI native provenance: AIFunction_SetDeathAnimation - 0x0044E210 - CFG score 83.3% (FAIL; structural only); runtime behavior not verified.
// Evidence: 0044E210-AIFunction_SetDeathAnimation.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AIFunction_SetDeathAnimation.h"
int AIFunction_SetDeathAnimation(int context, float arg2 )

// Sig: void AIFunction_SetDeathAnimation(int animId)
// Note: Overrides the death animation played when this AI dies. Param: animId =
// animation entry id. Use in AI init for custom deaths.
{
    int data_1 = Script_GetArg(arg2, 0);
    void* entity_val = g_5bda58;
    int item_18 = 1;
    *(entity_val + 0x3ad0) = data_1;
    int item_10 = 0;
    int item_c = 0x3ff00000;
    char* item_8 = &g_567c74;
    memcpy(context, &item_18, 0x18);
    return context;
}
