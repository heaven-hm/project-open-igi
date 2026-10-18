// IGI native provenance: Level_TimeSpentRedrawing2F - 0x004163E0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004163E0-Level_TimeSpentRedrawing2F.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Level_TimeSpentRedrawing2F.hpp"
void* Level_TimeSpentRedrawing2F()

{
    int saved_target = 1;
    int saved_src = Flow_RequestEvent_2();
    int item_10;
    void item_c;
    QTask_CloseEvent_3(&item_10, &item_c);
    int saved_index_1 = StatusScreen_GetEvent();
    int saved_source_1 = SoundSys_StopSoundSample_9();
    int item_8;
    QTask_CloseEvent_3(&item_8, &item_c);

    if (item_10)
    {
        saved_source_1 = item_8 / item_10 * 100.0f;
        SoundSys_Update3DPosition_11("Time spent redrawing: %.2f%%\n");
    }

    if (WeaponType_GetConfig_2(0x19) && Level_InitTextures_5(0))
    {
        int dest_1 = 0;

        if (Weapons_CountGet() > 0)
        {
            int data_13;

            do
            {
                int* data_7 = HumanPlayer_TakeDamage_10(dest_1);
                int temp_1 = 0;
                item_8 = *data_7;
                int data_8 = data_7[0x192];
                temp_1 = data_8 <= 0;
                int item_4_1 = (temp_1 - 1) & data_8;
                int entity_2 = 0;
                entity_2 = *(Level_InitTextures_5(0) + 0x1c);
                int source_3 = *(((entity_2 + AI_UpdateOrders_29() * 0x180) << 2) + &g_a96ae0);

                if (source_3)
                    source_3(Level_InitTextures_5(0), &item_8);

                dest_1 += 1;
                data_13 = Weapons_CountGet();
            } while (dest_1 < data_13);
        }

        int data_14 = Weapon_FireBullet_8("AMMO_ID_M203");
        Weapon_EjectShell_8(Level_InitTextures_5(0) + 0x340, data_14, 0xa);
    }

    if (DEBUG_KEYSENABLEINVALID(0x13))
        Flow_SetState(5);

    void* result = DEBUG_KEYSENABLEINVALID(0x17);

    if (result)
    {
        result = g_57babc;
        *(result + 0xe1) ^= 1;
    }

    return result;
}
