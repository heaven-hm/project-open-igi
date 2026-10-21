// IGI native provenance: Config_GameOptionsInputSetBloodEnabled - 0x004042E0 - CFG score 88.9% (PASS; structural only); runtime behavior not verified.
// Evidence: 004042E0-Config_GameOptionsInputSetBloodEnabled.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Config_GameOptionsInputSetBloodEnabled.h"
int Config_GameOptionsInputSetBloodEnabled(long double context , int arg2, float arg3 )

// Sig: void Config_GameOptionsInputSetBloodEnabled(int blood)
// Note: Sets blood-enabled in the config game-options write-out context. Param:
// blood = 1 on, 0 off.
{
    float item_30 = 0.0f;
    Profile_ValueSet(arg3, item_30);
    *(Config_GetActivePlayerProfileRecord() + 8) = context;
    int item_18 = 0;
    int item_10 = 0;
    int item_c = 0;
    char* item_8 = &g_567c74;
    memcpy(arg2, &item_18, 0x18);
    return arg2;
}
