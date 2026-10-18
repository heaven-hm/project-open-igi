// IGI native provenance: Display_ClearSurfaces_11 - 0x00496860 - CFG score 93.9% (PASS; structural only); runtime behavior not verified.
// Evidence: 00496860-Display_ClearSurfaces_11.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Display_ClearSurfaces_11.h"
int Display_ClearSurfaces_11(int context, int arg2, int arg3)

{
    if (g_6840e9)
    {
        int* data_1 = Display_RestoreDevice_10(context, arg2);

        if (data_1)
        {
            int result = g_6840f8;

            if (result)
            {
                Display_SetDisplayMode_11(result, &g_6840f8);
                *(result + 0xc) = data_1;
                *(result + 8) = arg3;
                int data_2 = data_1[5];

                if (data_2 > 3)
                {
                    Display_SetGammaRamp_8(result, &g_6840f4);
                    return result;
                }

                switch (data_2)
                {
                    case 0:
                    case 2:
                    {
                        short entity_3 = data_1[6];
                        short data_3 = *(data_1 + 0x16);
                        short item_36_1 = entity_3;
                        short item_2a_1 = data_3;
                        int data_6;
                        int temp_2;
                        temp_2 = HIGHD(data_3 * entity_3);
                        data_6 = LOWD(data_3 * entity_3);
                        int dest_1 = data_1[7];
                        int item_34_1 = dest_1;
                        short data_8 = (data_6 + (temp_2 & 7)) >> 3;
                        int temp_6 = data_8 * data_1[7];
                        int item_30_1 = data_8 * dest_1;
                        int item_24;
                        memset(&item_24, 0, 0x24);
                        short item_38;
                        short* item_14_1 = &item_38;
                        short item_28_1 = 0x12;
                        item_38 = 1;
                        item_24 = 0x24;
                        int item_20_1 = 0x100e8;
                        int* data_16 = g_67a498;
                        int item_1c_1 = temp_6 * 0xfa0 / 0x3e8;

                        if ((*(*data_16 + 0xc))(data_16, &item_24, result + 0x10, 0))
                            Display_SetGammaRamp_8(result, &g_6840f8);
                        else
                        {
                            Display_ClearSurfaces_10(result + 0x40, data_1[6]);
                            int* data_19 = QFile_ReadWrite(data_1[0xa], "r");
                            *(result + 0x18) = data_19;

                            if (data_19)
                            {
                                Sys_ExecuteBytecode_4(data_19, 0x14, 0);
                                Sys_ResolveSymbol_4(*(result + 0x18));
                                *(result + 0x3c) = 0;
                                memset(result + 0x20, 0, 0x14);
                                *(result + 0x38) = 0;
                                *(result + 0x1c) = g_c28c5c;
                                int data_25;
                                int temp_12;
                                temp_12 = HIGHD(0x10624dd3 * data_1[7] * 0xfa0);
                                data_25 = LOWD(0x10624dd3 * data_1[7] * 0xfa0);
                                int data_27 = temp_12 >> 6;
                                int data_29;
                                int temp_13;
                                temp_13 = HIGHD(data_27 + (data_27 >> 0x1f));
                                data_29 = LOWD(data_27 + (data_27 >> 0x1f));
                                *(result + 0x34) = (data_29 - temp_13) >> 1;
                                int data_34;
                                int temp_14;
                                temp_14 = HIGHD(0x10624dd3 * data_1[7] * 0xfa0);
                                data_34 = LOWD(0x10624dd3 * data_1[7] * 0xfa0);
                                int temp_15 = temp_14 >> 6;
                                Display_RestoreDevice_11(result, (temp_15 + (temp_15 >> 0x1f)) * 9 / 0xa);
                                Display_SetGammaRamp_8(result, &g_6840f4);
                                return result;
                            }

                            int* dest_3 = *(result + 0x10);
                            (*(*dest_3 + 8))(dest_3);
                            Display_SetGammaRamp_8(result, &g_6840f8);
                        }
                        break;
                    }
                    case 1:
                    case 3:
                    {
                        int* data_38 = g_67a498;

                        if (!(*(*data_38 + 0x14))(data_38, data_1[9], result + 0x10))
                        {
                            Display_SetGammaRamp_8(result, &g_6840f4);
                            return result;
                        }

                        Display_SetGammaRamp_8(result, &g_6840f8);
                        break;
                    }
                }
            }
        }
    }

    return 0;
}
