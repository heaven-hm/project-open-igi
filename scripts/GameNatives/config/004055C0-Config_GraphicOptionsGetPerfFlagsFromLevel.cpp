// IGI native provenance: Config_GraphicOptionsGetPerfFlagsFromLevel - 0x004055C0 - CFG score 88.9% (PASS; structural only); runtime behavior not verified.
// Evidence: 004055C0-Config_GraphicOptionsGetPerfFlagsFromLevel.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Config_GraphicOptionsGetPerfFlagsFromLevel.h"
int Config_GraphicOptionsGetPerfFlagsFromLevel(int context, float arg2 )

// Sig: int Config_GraphicOptionsGetPerfFlagsFromLevel(int level)
// Note: Converts a detail LEVEL number into its performance-flag bitmask.
// Param: level = 0..Config_GraphicOptionsNumPerfLevels()-1. Use when the user
// picks a named detail preset.
{
    int data_1 = *((Script_GetArg(arg2, 0) << 2) + &g_537b74);
    int item_2c = 0;
    float item_30 = arg2;
    int item_18 = data_1;
    double item_10 = *((Script_GetArg(item_30, item_2c) << 2) + &g_537b74);
    char* item_8 = &g_567c74;
    memcpy(context, &item_18, 0x18);
    return context;
}
