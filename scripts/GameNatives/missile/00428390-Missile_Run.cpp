// IGI native provenance: Missile_Run - 0x00428390 - CFG score 94.7% (PASS; structural only); runtime behavior not verified.
// Evidence: 00428390-Missile_Run.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "00428390-Missile_Run.hpp"
int Missile_Run(int* context)

{
    void* flagP = *(context[0x6d] + 0x2d4);
    int* data_1 = context[0x218];

    if (data_1)
    {
        int data_2 = *data_1;

        if (data_2)
            memcpy(&context[0x20a], data_2 + 0x20, 0x18);
    }

    int data_3 = *(flagP + 0x11c);

    if (!data_3)
    {
        void* item_ac_1 = &context[0x20a];
        Menu_UpdateLanguage_19(context);
    }
    else if (data_3 == 1)
        Menu_FormatScreen_19(context);
    else if (data_3 == 2)
        Menu_DrawWidget_19(context, &context[0x20a]);
    else if (data_3 == 3)
        Menu_HandleInput_19(context, &context[0x20a]);
    else if (data_3 == 4)
        Menu_PlayMusic_19(context);

    long double fpu7 = context[0x219];
    long double f0 = 0.0f;
    fpu7 - f0;

    if (!(*(&((fpu7 < f0 ? 1 : 0) << 8 | (FCMP_UO(fpu7, f0) ? 1 : 0) << 0xa
            | (fpu7 == f0 ? 1 : 0) << 0xe) + 1) & 0x40) && *(flagP + 0x118))
        Menu_LeaveMenus_19(context);

    Menu_UpdateLanguage_20(context);
    Menu_RequestScreen_19(context);
    VehicleChild_Dispatch(context);
    int* i = context[2];

    if (*i && i)
    {
        int temp_2 = g_afa7e0;

        do
        {
            int* source_2 = *i;
            void* data_10;

            if (!source_2)
                data_10 = nullptr;
            else
            {
                int data_7 = *source_2;
                int data_8 = -(data_7);
                data_10 = (data_8 - data_8) & source_2;
            }

            *((temp_2 << 2) + &g_afa6e0) = data_10;
            int temp_3 = temp_2 + 1;
            int data_11 = 0;
            g_afa7e0 = temp_3;
            data_11 = i[7];
            int data_12 = *((data_11 << 2) + &g_a970e0);

            if (data_12)
            {
                data_12(i);
                temp_3 = g_afa7e0;
            }

            i = *((temp_3 << 2) + &g_afa6dc);
            temp_2 = temp_3 - 1;
            g_afa7e0 = temp_2;
        } while (i);
    }

    int result = context[0x6e];
    int item_98;
    void item_90;
    double item_88[0x3];
    int item_70[0x16];

    if (!result)
    {
        if (!(*(g_bc2360 + 0x68) & 0x1f))
        {
            memcpy(&item_88, &context[8], 0x18);
            int* item_8c_2 = context;
            item_98 = 0;
            int item_94_2 = 0xc14f4000;
            DynObj_LookupHashItem_10(Level_StartMission_4(), &item_70, &item_88, 1, &item_98, 0xc, &item_90,
                0xffffffff);
            int result_1;
            result = result_1;

            if (!result)
                return QTask_FindType(context);
        }

        return result;
    }

    int data_13 = 0;
    char* entity_2 = context + 0x1d6;

    while (!*entity_2)
    {
        data_13 += 1;
        entity_2 = &entity_2[0x38];

        if (data_13 >= 0x16)
            break;
    }

    memcpy(&item_88, &context[8], 0x18);
    int* item_8c_1 = context;
    item_98 = 0;
    int item_94_1 = 0xc0c80000;
    DynObj_LookupHashItem_10(Level_StartMission_4(), &item_70, &item_88, 1, &item_98, 0xc, &item_90, 0xffffffff);
    FX_SmokeEmit(&item_88, 1, 1, 0x40a00000, 0x46200000, 0x45e00000, 0x45c00000, 0xa, 5, &item_70,
        "1009_01_1", "explo_03_l", 0x15, 2, 0x47c80000, context[0x225]);
    return QTask_FindType(context);
}
