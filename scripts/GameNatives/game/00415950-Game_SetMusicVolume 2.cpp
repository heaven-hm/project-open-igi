// IGI native provenance: Game_SetMusicVolume - 0x00415950 - CFG score 88.9% (PASS; structural only); runtime behavior not verified.
// Evidence: 00415950-Game_SetMusicVolume.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Game_SetMusicVolume.h"
int Game_SetMusicVolume(long double context , int arg2, float arg3 )

// Sig: void Game_SetMusicVolume(float volume)
// Note: Live-sets music volume through the profile real-value setter
// Param: volume = float 0.0-1.0. Called by ingamemenu.qvm options sliders.
{
    Profile_ValueSet(arg3, 0.0f);
    arg3 = context;
    float item_30 = arg3;
    Music_SetVolume(item_30, arg3);
    int item_18 = 1;
    int item_10 = 0;
    int item_c = 0x3ff00000;
    char* item_8 = &g_567c74;
    memcpy(arg2, &item_18, 0x18);
    return arg2;
}
