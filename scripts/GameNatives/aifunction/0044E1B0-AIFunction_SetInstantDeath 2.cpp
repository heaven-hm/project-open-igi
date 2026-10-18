// IGI native provenance: AIFunction_SetInstantDeath - 0x0044E1B0 - CFG score 83.3% (FAIL; structural only); runtime behavior not verified.
// Evidence: 0044E1B0-AIFunction_SetInstantDeath.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AIFunction_SetInstantDeath.h"
int AIFunction_SetInstantDeath(int context, float arg2 )

// Sig: void AIFunction_SetInstantDeath(int onOff)
// Note: When on, any hit instantly kills this AI (no damage pooling). Param:
// onOff = 1 one-shot kills, 0 normal health. Use for scripted one-shot enemies.
{
    *(g_5bda58 + 0x3abd) = !Script_GetArg(arg2, 0);
    int item_18 = 1;
    int item_10 = 0;
    int item_c = 0x3ff00000;
    char* item_8 = &g_567c74;
    memcpy(context, &item_18, 0x18);
    return context;
}
