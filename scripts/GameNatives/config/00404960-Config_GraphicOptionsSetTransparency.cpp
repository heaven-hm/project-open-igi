// IGI native provenance: Config_GraphicOptionsSetTransparency - 0x00404960 - CFG score 88.9% (PASS; structural only); runtime behavior not verified.
// Evidence: 00404960-Config_GraphicOptionsSetTransparency.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "00404960-Config_GraphicOptionsSetTransparency.hpp"
int Config_GraphicOptionsSetTransparency(int context, float arg2 )

// Sig: void Config_GraphicOptionsSetTransparency(int onOff)
// Note: Toggles transparency for the active profile. Param: onOff = 1 enable, 0
// disable. Video-options screen.
{
    *(Config_GetActiveGraphicOptions() + 0x10) = Script_GetArg(arg2, 0);
    int item_18 = 0;
    int item_10 = 0;
    int item_c = 0;
    char* item_8 = &g_567c74;
    memcpy(context, &item_18, 0x18);
    return context;
}
