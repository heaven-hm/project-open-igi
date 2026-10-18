// IGI native provenance: Game_UpdateVolume - 0x004158E0 - CFG score 94.4% (PASS; structural only); runtime behavior not verified.
// Evidence: 004158E0-Game_UpdateVolume.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Game_UpdateVolume.h"
int Game_UpdateVolume(int context)

// Sig: void Game_UpdateVolume(void)
// Note: Applies current profile music/sfx volumes to the live audio engines.
// Takes no args.
{
    int entity_val;
    long double fp0;
    fp0 = Config_SaveGameOptions_2();
    int item_24 = entity_val;
    float item_24_1 = fp0;
    int entity_1;
    long double fp0_1;
    fp0_1 = Config_SaveGameOptions_2();
    int item_28 = entity_1;
    Music_SetVolume(fp0_1, item_24_1);
    Music_SetSfxVolume(Config_RegisterOptions_2());
    int item_18 = 1;
    int item_10 = 0;
    int item_c = 0x3ff00000;
    char* item_8 = &g_567c74;
    memcpy(context, &item_18, 0x18);
    return context;
}
