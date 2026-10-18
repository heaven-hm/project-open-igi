// IGI native provenance: Game_DisableMusic - 0x00415A60 - CFG score 66.7% (FAIL; structural only); runtime behavior not verified.
// Evidence: 00415A60-Game_DisableMusic.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "00415A60-Game_DisableMusic.hpp"
int Game_DisableMusic(int context)

// Sig: void Game_DisableMusic(void)
// Note: Stops the music engine. Takes no args.
{
    g_57bac0 = 0;
    int item_18 = 1;
    int item_10 = 0;
    int item_c = 0x3ff00000;
    char* item_8 = &g_567c74;
    memcpy(context, &item_18, 0x18);
    return context;
}
