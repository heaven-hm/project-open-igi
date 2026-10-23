// IGI native provenance: MenuManager_PopScreen - 0x00417710 - CFG score 88.9% (PASS; structural only); runtime behavior not verified.
// Evidence: 00417710-MenuManager_PopScreen.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "MenuManager_PopScreen.h"
int MenuManager_PopScreen(int context, void* arg2, float arg3)

// Sig: void MenuManager_PopScreen(int args)
// Note: Pops the idx screen off the menu stack (destroys it) revealing the one
// beneath. Param: args = pass-through argument observed in retail calls (pass
// 0). Pair with MenuManager_PushScreen.
{
    if (Script_GetArg(arg3, 0))
        Menu_UpdateLanguage(arg2);

    int item_10 = 0;
    int data_2 = *(arg2 + 0x27e8) - 1;
    int item_c = 0x3ff00000;
    *(arg2 + 0x27e8) = data_2;
    char* item_8 = &g_567c74;
    *(arg2 + 0x20) = *(arg2 + (data_2 << 2) + 0x27ec);
    *(arg2 + 0x2c) = 1;
    int item_18 = 1;
    *(arg2 + 0x28) = 6;
    memcpy(context, &item_18, 0x18);
    return context;
}
