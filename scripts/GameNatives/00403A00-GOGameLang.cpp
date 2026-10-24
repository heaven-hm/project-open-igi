// IGI native provenance: GO_GameLang - 0x00403A00 - CFG score 91.7% (PASS; structural only); runtime behavior not verified.
// Evidence: 00403A00-GO_GameLang.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "GO_GameLang.h"
int GO_GameLang(int context, float arg2 )

// Sig: int GO_GameLang(int language)
// Note: Sets game language for the current profile. Param: language = language
// index (retail ships English=0 and German builds use their own index). Use in
// config scripts. Returns 1.
{
    int entity_val;
    int temp_val;

    if (Camera_UpdateTransform_6(&g_5c8c20, temp_val, entity_val, &g_5c8c20, "USA"))
    {
        *(g_bc2380 * 0x564 + &g_bc1e28) = Script_GetArg(arg2, 0);
        int data_11 = g_bc2380 * 0x159;

        if (*((data_11 << 2) + &g_bc1e28) == 5)
            *((data_11 << 2) + &g_bc1e28) = 0;
    }
    else
        *(g_bc2380 * 0x564 + &g_bc1e28) = 5;

    int item_18 = 0;
    int item_10 = 0;
    int item_c = 0;
    char* item_8 = &g_567c74;
    memcpy(context, &item_18, 0x18);
    return context;
}
