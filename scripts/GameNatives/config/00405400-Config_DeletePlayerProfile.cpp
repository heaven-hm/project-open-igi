// IGI native provenance: Config_DeletePlayerProfile - 0x00405400 - CFG score 75.0% (FAIL; structural only); runtime behavior not verified.
// Evidence: 00405400-Config_DeletePlayerProfile.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "00405400-Config_DeletePlayerProfile.hpp"
int Config_DeletePlayerProfile(int context, float arg2 )

// Sig: int Config_DeletePlayerProfile(int index)
// Note: Deletes a player profile. Param: index = 0-based profile index to
// delete. RETURNS 1 on success, 0 on failure. Do not delete the last remaining
// profile; refresh your list box afterwards.
{
    int data_val = Script_GetArg(arg2, 0);
    int entity_val = g_bc2380;
    int item_18;

    if (data_val >= entity_val)
    {
        item_18 = 0;
        int item_10_1 = 0;
        int item_c_1 = 0;
        char* item_8_1 = &g_567c74;
        memcpy(context, &item_18, 0x18);
        return context;
    }

    int temp_val = g_bc2384;

    if (temp_val >= data_val)
        g_bc2384 = temp_val - 1;

    int data_1 = data_val + 1;

    if (data_1 < entity_val)
    {
        void* temp_2 = &(&g_bc2388)[data_1 * 0x159];

        do
        {
            data_1 += 1;
            memcpy(temp_2 - 0x564, temp_2, 0x564);
            entity_val = g_bc2380;
            temp_2 += 0x564;
        } while (data_1 < entity_val);
    }

    g_bc2380 = entity_val - 1;
    item_18 = 1;
    int item_10 = 0;
    int item_c = 0x3ff00000;
    char* item_8 = &g_567c74;
    memcpy(context, &item_18, 0x18);
    return context;
}
