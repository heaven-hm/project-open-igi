// IGI native provenance: Game_SetSFXVolume - 0x004159B0 - CFG score 88.9% (PASS; structural only); runtime behavior not verified.
// Evidence: 004159B0-Game_SetSFXVolume.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Game_SetSFXVolume.h"
int Game_SetSFXVolume(long double context , int arg2, float arg3 )

// Sig: void Game_SetSFXVolume(float volume)
// Note: Live-sets sound-effects volume through the profile real-value setter
// (fcn.004B8A50), same 1.0f construction pattern. Param: volume = float
// 0.0-1.0. Called by ingamemenu.qvm options sliders.
{
    Profile_ValueSet(arg3, 0.0f);
    Music_SetSfxVolume(context);
    int item_18 = 1;
    int item_10 = 0;
    int item_c = 0x3ff00000;
    char* item_8 = &g_567c74;
    memcpy(arg2, &item_18, 0x18);
    return arg2;
}
