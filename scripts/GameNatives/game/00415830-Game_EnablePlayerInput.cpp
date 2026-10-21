// IGI native provenance: Game_EnablePlayerInput - 0x00415830 - CFG score 88.9% (PASS; structural only); runtime behavior not verified.
// Evidence: 00415830-Game_EnablePlayerInput.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Game_EnablePlayerInput.h"
int Game_EnablePlayerInput(int context)

// Sig: void Game_EnablePlayerInput(void)
// Note: Re-enables player input after Game_DisablePlayerInput. Takes no args.
{
    for (int i = 0; i < 4; i += 1)
    {
        void* data_1 = Level_InitTextures_5(i);

        if (data_1)
            AI_MoveToNode_20(data_1);
    }

    int item_18 = 1;
    int item_10 = 0;
    int item_c = 0x3ff00000;
    char* item_8 = &g_567c74;
    memcpy(context, &item_18, 0x18);
    return context;
}
