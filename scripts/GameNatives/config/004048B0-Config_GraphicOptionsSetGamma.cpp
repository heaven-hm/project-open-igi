// IGI native provenance: Config_GraphicOptionsSetGamma - 0x004048B0 - CFG score 88.9% (PASS; structural only); runtime behavior not verified.
// Evidence: 004048B0-Config_GraphicOptionsSetGamma.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004048B0-Config_GraphicOptionsSetGamma.hpp"
int Config_GraphicOptionsSetGamma(long double context , int arg2, float arg3 )

// Sig: void Config_GraphicOptionsSetGamma(float gamma)
// Note: Sets and immediately applies gamma to the active profile (also feeds
// the live lighting math through the profile record +0x220). Param: gamma =
// float (retail default 1.0). Use in video-options screen script or your own
// gamma slider.
{
    Profile_ValueSet(arg3, 0.0f);
    *(Config_GetActiveGraphicOptions() + 0x214) = context;
    int item_18 = 0;
    int item_10 = 0;
    int item_c = 0;
    char* item_8 = &g_567c74;
    memcpy(arg2, &item_18, 0x18);
    return arg2;
}
