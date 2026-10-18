// IGI native provenance: DynObj_RegisterSymbol_10 - 0x004C9A30 - CFG score 83.3% (FAIL; structural only); runtime behavior not verified.
// Evidence: 004C9A30-DynObj_RegisterSymbol_10.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "DynObj_RegisterSymbol_10.h"
int DynObj_RegisterSymbol_10(void* context, int* arg2, double* arg3 , int arg4)

{
    int* context_val = arg2;
    void* item_40 = context_val[6];
    int temp_val = *(context_val[7] + 0xc);
    arg2 = context_val[1];
    int item_20 = context_val[4];
    double* entity_3 = arg3;
    int item_28 = *context_val;
    int item_44 = context_val[2];
    int item_24 = context_val[3];
    int dest_val = *entity_3;
    int item_1c = context_val[5];
    arg3 = *(context + 0x20);
    int source_val = entity_3[1];
    int flagP = entity_3[2];
    long double fpu_val_7_3 = *entity_3;
    long double f0 = 0.0;
    fpu_val_7_3 - f0;

    if (*(&((fpu_val_7_3 < f0 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_7_3, f0) ? 1 : 0) << 0xa
        | (fpu_val_7_3 == f0 ? 1 : 0) << 0xe) + 1) & 1)
    {
        long double fpu_val_7_4 = dest_val;
        long double f2_1 = *entity_3;
        fpu_val_7_4 - f2_1;

        if (!(*(&((fpu_val_7_4 < f2_1 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_7_4, f2_1) ? 1 : 0) << 0xa
                | (fpu_val_7_4 == f2_1 ? 1 : 0) << 0xe) + 1) & 0x40))
            dest_val -= 1;
    }

    long double fpu_val_7_5 = entity_3[1];
    long double f1 = 0.0;
    fpu_val_7_5 - f1;

    if (*(&((fpu_val_7_5 < f1 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_7_5, f1) ? 1 : 0) << 0xa
        | (fpu_val_7_5 == f1 ? 1 : 0) << 0xe) + 1) & 1)
    {
        long double fpu_val_7_6 = source_val;
        long double f4_1 = entity_3[1];
        fpu_val_7_6 - f4_1;

        if (!(*(&((fpu_val_7_6 < f4_1 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_7_6, f4_1) ? 1 : 0) << 0xa
                | (fpu_val_7_6 == f4_1 ? 1 : 0) << 0xe) + 1) & 0x40))
            source_val -= 1;
    }

    long double fpu_val_7_7 = entity_3[2];
    long double f3 = 0.0;
    fpu_val_7_7 - f3;

    if (*(&((fpu_val_7_7 < f3 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_7_7, f3) ? 1 : 0) << 0xa
        | (fpu_val_7_7 == f3 ? 1 : 0) << 0xe) + 1) & 1)
    {
        long double fpu_val_7_8 = flagP;
        long double f5_1 = entity_3[2];
        fpu_val_7_8 - f5_1;

        if (!(*(&((fpu_val_7_8 < f5_1 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_7_8, f5_1) ? 1 : 0) << 0xa
                | (fpu_val_7_8 == f5_1 ? 1 : 0) << 0xe) + 1) & 0x40))
            flagP -= 1;
    }

    int dest_1 = dest_val + *(context + 0xde0);
    int source_1 = source_val + *(context + 0xde4);
    int flagP1 = flagP + *(context + 0xde8);
    int dest_2 = dest_1 ^ 0x40000000;
    int source_2 = source_1 ^ 0x40000000;
    int flagP2 = flagP1 ^ 0x40000000;
    int item_3c_1 = dest_2;
    int item_38_1 = source_2;
    int item_34_1 = flagP2;
    DynObj_ParseConfigFile_6();
    int result = 0;
    int result_1 = 0;

    if (arg4 > 0)
    {
        int* entity_6 = arg2;
        void* item_30_1 = entity_6 - 1;

        while (true)
        {
            int source_4 = source_2 >> (entity_6 & 0x1f) & 1;
            int dest_4 = dest_2 >> (entity_6 & 0x1f) & 1;
            int flagP4 = flagP2 >> (arg2 & 0x1f) & 1;
            int entity_10 =
                *(((((source_4 * 2) | dest_4) + (item_44 << 3)) << 2) + &g_548060) | flagP4 << 2;

            if (!(*(item_40 + 0x18) & 1 << (entity_10 & 0x1f)))
            {
                result = result_1;
                break;
            }

            if (g_b97d20 && arg3)
            {
                if (arg3[7] & 0x80)
                {
                    void* data_13 = *(arg3 + 0x2c);
                    context = data_13;

                    while (true)
                    {
                        if (*(data_13 + 0x5c) & 0x80)
                        {
                            int entity_11 = 0;
                            entity_11 = *(data_13 + 0x1c);
                            (*((entity_11 << 2) + &g_a9c4e0))(data_13, *(data_13 + 0x48));
                            data_13 = context;
                        }

                        void* data_14 = *(data_13 + 0x58);
                        context = data_14;

                        if (!data_14)
                            break;

                        data_13 = context;
                    }
                }

                char data_15 = item_30_1;
                double* temp_17 = *(arg3 + (((flagP1 >> ((data_15 - 1) & 0x1f) & 4)
                    | (source_1 >> (data_15 & 0x1f) & 2) | (dest_1 >> (arg2 & 0x1f) & 1)) << 2) + 0xc);
                arg3 = temp_17;
            }

            int temp_19 = item_28 >> 1;
            item_28 = temp_19;
            arg2 -= 1;
            item_30_1 -= 1;
            item_24 -= (-(dest_4) ^ temp_19) + dest_4;
            item_20 -= (-(source_4) ^ temp_19) + source_4;
            int temp_20 = 0;
            temp_20 = *(entity_10 + item_40 + 0x10);
            item_1c -= (-(flagP4) ^ temp_19) + flagP4;

            item_44 = item_44 >= 4 ? ((item_44 - temp_20) & 3) + ((temp_20 ^ item_44) & 4)
                : ((item_44 + temp_20) & 3) + (temp_20 & 4);

            int temp_24 = 0;
            temp_24 = *(item_40 + (entity_10 << 1));
            result = result_1 + 1;
            item_40 = (temp_24 << 5) + temp_val;
            result_1 = result;

            if (result >= arg4)
                break;

            dest_2 = item_3c_1;
            source_2 = item_38_1;
            flagP2 = item_34_1;
            entity_6 = arg2;
        }
    }

    context_val[2] = item_44;
    *context_val = item_28;
    context_val[1] = arg2;
    context_val[3] = item_24;
    context_val[4] = item_20;
    context_val[5] = item_1c;
    context_val[6] = item_40;
    context_val[8] = arg3;

    if (g_b97df8 > 6)
        g_b97df8 = 6;

    return result;
}
