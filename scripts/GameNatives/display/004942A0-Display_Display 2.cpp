// IGI native provenance: Display_Display - 0x004942A0 - CFG score 87.2% (PASS; structural only); runtime behavior not verified.
// Evidence: 004942A0-Display_Display.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Display_Display.h"
int Display_Display(int context, int arg2, int arg3, int arg4, void* arg5, int* arg6)

{
    int context_val = arg3;

    if (context_val == 0xffffffff)
        context_val = *(arg5 + 4);

    int dest_val = arg4;

    if (dest_val == 0xffffffff)
        dest_val = *(arg5 + 8);

    int data_2;

    if (FX_FragmentSimulate_23())
    {
        ShowCursor(0);
        int entity_1 = context;

        if (entity_1 == 0xffffffff)
            entity_1 = *(arg5 + 0x10);

        int* data_1 = g_5ca10c;
        data_2 = (*(*data_1 + 0x54))(data_1, context_val, dest_val, entity_1, 0, 0);

        if (!data_2)
            goto label_494336;
    }
    else
    {
        ShowCursor(1);
        HDC hdc = CreateICA("Display", nullptr, nullptr, nullptr);
        GetDeviceCaps(hdc, 0xc);
        DeleteDC(hdc);
    label_494336:
        g_5ca0d8 = context_val;
        g_5ca044 = dest_val;
        int item_a0;

        if (!FX_FragmentSimulate_23())
        {
            memset(&item_a0, 0, 0x7c);
            int* data_10 = g_5ca10c;
            int item_c8_9 = 0;
            item_a0 = 0x7c;
            int item_9c_2 = 1;
            int item_38_2 = 0x80200;
            data_2 = (*(*data_10 + 0x18))(data_10, &item_a0, &g_5ca0fc, item_c8_9);

            if (!data_2)
            {
                int item_94_1 = g_5ca0d8;
                int* data_12 = g_5ca0fc;
                int item_98_1 = g_5ca044;
                int item_9c_3 = 7;
                int item_38_3 = 0x82000;
                int item_58 = 0x20;
                int item_54_1 = 0x40;
                int source_3 = (*(*data_12 + 0x54))(data_12, &item_58);

                if (source_3)
                {
                label_494662:
                    int* data_36 = g_5ca0fc;
                    (*(*data_36 + 8))(data_36);
                    return source_3;
                }

                int* data_14 = g_5ca10c;
                int data_15 = (*(*data_14 + 0x18))(data_14, &item_a0, &g_5ca100, source_3);

                if (data_15)
                {
                    int* data_16 = g_5ca0fc;
                    (*(*data_16 + 8))(data_16);
                    return data_15;
                }

            label_4944c4:
                memset(&item_a0, 0, 0x7c);
                int i = 4;
                int* dest_1 = &g_534a10;
                int* source_5 = arg6;
                bool flag_3_1 = true;

                while (i)
                {
                    int f0_1 = *source_5;
                    int f1_1 = *dest_1;
                    flag_3_1 = f0_1 == f1_1;
                    source_5 = &source_5[1];
                    dest_1 = &dest_1[1];
                    i -= 1;

                    if (f0_1 != f1_1)
                        break;
                }

                int item_9c_4 = 0x1007;
                bool flag_5_1;

                if (!flag_3_1)
                {
                    int i_1 = 4;
                    int* dest_2 = &g_5349f0;
                    int* source_6 = arg6;
                    flag_5_1 = false;

                    while (i_1)
                    {
                        int f2_1 = *source_6;
                        int f3_1 = *dest_2;
                        flag_5_1 = f2_1 != f3_1;
                        source_6 = &source_6[1];
                        dest_2 = &dest_2[1];
                        i_1 -= 1;

                        if (f2_1 != f3_1)
                            break;
                    }

                    int item_38_4 = 0x24000;
                }

                if (flag_3_1 || !flag_5_1)
                    int item_38_5 = 0x20800;

                int item_98_2 = g_5ca044;
                int item_94_2 = g_5ca0d8;
                void item_24;
                memset(&item_24, 0, 0x20);
                int* data_19 = g_5ca110;
                int item_4_1 = arg2;
                item_a0 = 0x7c;
                int data_20;
                int temp_7;
                data_20 = (*(*data_19 + 0x18))(data_19, arg6, Display_CreateSurface_6, &item_24);
                int item_18;

                if (item_18)
                {
                    if (data_20)
                    {
                        int* data_22 = g_5ca100;
                        (*(*data_22 + 8))(data_22);
                        int* data_23 = g_5ca0fc;
                        (*(*data_23 + 8))(data_23);
                        return data_20;
                    }

                    int* data_25 = g_5ca10c;
                    g_bcada4 = temp_7 - temp_7 + 1;
                    int item_c8_17 = 0;
                    memcpy(&item_58, &item_24, 0x20);
                    int data_26 = (*(*data_25 + 0x18))(data_25, &item_a0, &g_5ca104, item_c8_17);
                    int* data_27 = g_5ca100;
                    int entity_10 = *data_27;

                    if (data_26)
                    {
                        (*(entity_10 + 8))(data_27);
                        int* data_28 = g_5ca0fc;
                        (*(*data_28 + 8))(data_28);
                        return data_26;
                    }

                    source_3 = (*(entity_10 + 0xc))(data_27, g_5ca104);

                    if (!source_3)
                    {
                        int* data_31 = g_5ca110;
                        int item_b4;
                        source_3 = (*(*data_31 + 0x10))(data_31, arg6, g_5ca100, &item_b4);
                        g_5ca114 = item_b4;

                        if (!source_3)
                            return 0;

                        int* data_33 = g_5ca100;
                        (*(*data_33 + 0x20))(data_33, 0, g_5ca104);
                    }

                    int* data_34 = g_5ca104;
                    (*(*data_34 + 8))(data_34);
                    int* data_35 = g_5ca100;
                    (*(*data_35 + 8))(data_35);
                }
                else
                {
                    Error_Show("Couldn't find zbuffer format");

                    while (true)
                        /* nop */
                }

                goto label_494662;
            }
        }
        else
        {
            memset(&item_a0, 0, 0x7c);
            int item_c8_6 = 0;
            int item_8c_1 = *(arg5 + 0xc);
            int* data_5 = g_5ca10c;
            item_a0 = 0x7c;
            int item_9c_1 = 0x21;
            int item_38_1 = 0x82218;
            data_2 = (*(*data_5 + 0x18))(data_5, &item_a0, &g_5ca0fc, item_c8_6);

            if (!data_2)
            {
                int item_b0 = data_2;
                int item_ac_1 = data_2;
                item_b0 = 0x82004;
                int item_a8_1 = data_2;
                int item_a4_1 = data_2;
                int* data_6 = g_5ca0fc;
                int data_7 = (*(*data_6 + 0x30))(data_6, &item_b0, &g_5ca100);

                if (!data_7)
                    goto label_4944c4;

                int* data_8 = g_5ca0fc;
                (*(*data_8 + 8))(data_8);
                return data_7;
            }
        }
    }

    return data_2;
}
