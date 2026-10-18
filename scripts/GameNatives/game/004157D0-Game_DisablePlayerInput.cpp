// IGI native provenance: Game_DisablePlayerInput - 0x004157D0 - CFG score 88.9% (PASS; structural only); runtime behavior not verified.
// Evidence: 004157D0-Game_DisablePlayerInput.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004157D0-Game_DisablePlayerInput.hpp"
int Game_DisablePlayerInput(int context)

// Sig: void Game_DisablePlayerInput(void)
// Note: Disables ALL player input (keyboard+mouse) - used by cutscenes/menus.
// Re-enable with Game_EnablePlayerInput.
{
    for (int i = 0; i < 4; i += 1)
    {
        void* data_1 = Level_InitTextures_5(i);

        if (data_1)
            AI_FireWeapon_20(data_1);
    }

    int item_18 = 1;
    int item_10 = 0;
    int item_c = 0x3ff00000;
    char* item_8 = &g_567c74;
    memcpy(context, &item_18, 0x18);
    return context;
}
