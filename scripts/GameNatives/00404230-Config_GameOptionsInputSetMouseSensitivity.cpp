// IGI native provenance: Config_GameOptionsInputSetMouseSensitivity - 0x00404230 - CFG score 88.9% (PASS; structural only); runtime behavior not verified.
// Evidence: 00404230-Config_GameOptionsInputSetMouseSensitivity.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Config_GameOptionsInputSetMouseSensitivity.h"
int Config_GameOptionsInputSetMouseSensitivity(long double context , int arg2, float arg3 )

// Sig: void Config_GameOptionsInputSetMouseSensitivity(int value)
// Note: Sets mouse-sensitivity in the config game-options write-out context.
// Param: value = sensitivity as stored by the options screen (integer scale).
{
    Profile_ValueSet(arg3, 0.0f);
    *(Config_GetActivePlayerProfileRecord() + 0x53c) = context;
    int item_18 = 0;
    int item_10 = 0;
    int item_c = 0;
    char* item_8 = &g_567c74;
    memcpy(arg2, &item_18, 0x18);
    return arg2;
}
