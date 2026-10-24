// IGI native provenance: ControlsMenu_LoadParameters - 0x004234E0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004234E0-ControlsMenu_LoadParameters.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004234E0-ControlsMenu_LoadParameters.hpp"
int ControlsMenu_LoadParameters(void* context)

{
    void* source_val = context;

    if (*(source_val + 0x35a))
    {
        Menu_FormatScreen_12(source_val);
        *(source_val + 0x35a) = 0;
    }

    if (!*(source_val + 0x359))
    {
        char* item_1c;
        void item_18;
        Menu_UpdateLanguage_14(source_val, *(source_val + 0x20), *(source_val + 0x24), *(source_val + 0x28), *(source_val + 0x2c),
            &item_18, &item_1c);

        if (*(source_val + 0x4b))
        {
            int data_27;
            int temp_10;
            temp_10 = HIGHD(0x66666667 * (item_1c - *(source_val + 0x24)));
            data_27 = LOWD(0x66666667 * (item_1c - *(source_val + 0x24)));
            int temp_11 = temp_10 >> 3;
            int data_30 = temp_11 + (temp_11 >> 0x1f) + *(source_val + 0x354);
            *(source_val + 0x35c) = data_30;
            int* dest_2 = *(source_val + (data_30 << 2) + 0x154);

            if (dest_2)
                Sys_FreeMemory_18(dest_2, Menu_LocalSS(*(source_val + 0x44), "Select a key or button"));

            Menu_LeaveMenus_4(*(source_val + 0x44));
            *(source_val + 0x359) = 1;
            SoundSys_AttachLipsync_8("menu_sel2", 0, 0);
        }
    }
    else
    {
        int data_1 = *g_c28f8c;
        context = 0;

        if (!(g_c28c63 & 0x80))
        {
            void* context_1 = *(source_val + 0x35c);
            int item_28;
            int i;

            for (i = 0; i < 0x7f; i += 1)
            {
                if (context)
                    goto label_4235ff;

                KEY_ANGLE(i, &item_28, nullptr);
                int entity_2 = item_28;
                int data_8 = entity_2 >> 0x1e & 3;

                if (data_8 == 1)
                {
                    if (*((entity_2 & 0x3fffffff) + 0xc28c62) & 0x80)
                        context = 1;
                }
                else if (data_8 == 3 && data_1 == (entity_2 & 0x3fffffff))
                    context = 1;

                int data_4 = entity_2 & 0x3fffffff;

                if (data_4 == 0xdb || data_4 == 0xdc)
                    context = 0;
            }

            if (context)
            {
            label_4235ff:
                int data_16;
                int temp_4;
                temp_4 = HIGHD(Sys_CheckHeap_13(0x57bc48));
                data_16 = LOWD(Sys_CheckHeap_13(0x57bc48));
                int item_40_3 = ((data_16 ^ temp_4) - temp_4) % 7 + 1;
                void item_14;
                Game_DataSymbolLoad(&item_14, "menu_k0%.1d");
                SoundSys_AttachLipsync_8(&item_14, 0, 0);
                item_28 = *(source_val + (context_1 << 2) + 0x254);
                Menu_UpdateLanguage_12(source_val, context_1, i - 1);
                void* i_1 = nullptr;
                void** context_2 = source_val + 0x254;

                do
                {
                    if (context_1 != i_1 && *context_2 == i - 1)
                        Menu_UpdateLanguage_12(source_val, i_1, item_28);

                    i_1 += 1;
                    context_2 = &context_2[1];
                } while (i_1 < 0x1b);

                if (context)
                    goto label_423685;
            }
        }
        else
        {
            void* data_3 = *(source_val + 0x35c);
            Menu_UpdateLanguage_12(source_val, data_3, *(source_val + (data_3 << 2) + 0x254));
            SoundSys_AttachLipsync_8("menu_cancel", 0, 0);
        label_423685:
            FX_ExplosionSpawn_17();
            Menu_UpdateLanguage_4(*(source_val + 0x44));
            *(source_val + 0x359) = 0;
            *(source_val + 0x35c) = 0xffffffff;
        }
    }

    return (*((Menu_PlayMusic_13() << 2) + &g_a970e0))(source_val);
}
