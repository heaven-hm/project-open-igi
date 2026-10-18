// IGI native provenance: DynObj_RemoveSymbol_9 - 0x004C9750 - CFG score 83.3% (FAIL; structural only); runtime behavior not verified.
// Evidence: 004C9750-DynObj_RemoveSymbol_9.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004C9750-DynObj_RemoveSymbol_9.hpp"
unsigned int DynObj_RemoveSymbol_9(int context, int* arg2, double* arg3, unsigned int arg4, char arg5, int arg6, int arg7)

{
    DynObj_ParseConfigFile_6();
    int context_val = context;
    int data_val = *(context_val + 0xc);
    double item_2c;
    memcpy(&item_2c, arg3, 0x18);
    double item_44;
    memcpy(&item_44, arg3, 0x18);
    int item_50 = data_val + 0x20;
    int item_4c = context_val;
    int item_68 = 0x40000000;
    int item_64 = 0x1e;
    int item_60 = 0;
    int item_5c = 0;
    int item_58 = 0;
    int item_54 = 0;
    char item_f4[0x80];
    double item_3c;
    double item_34;
    double item_24;
    double item_1c;

    if (arg4 > 0)
    {
        long long item_c;
        *(&item_c + 4) = arg4;
        long double fpu_val_7_1 = item_1c;
        int* source_2 = arg2;
        double* temp_1 = &arg3[2];
        memset(memset(&item_f4, 0x1010101, arg4 & 0xfffffffc), 1, arg4 & 3);
        unsigned int i_1 = arg4;
        unsigned int i;

        do
        {
            long double fpu_val_6_1 = item_44;
            long double f0_1 = temp_1[-2];
            fpu_val_6_1 - f0_1;

            if (!(*(&((fpu_val_6_1 < f0_1 ? 1 : 0) << 8
                | (FCMP_UO(fpu_val_6_1, f0_1) ? 1 : 0) << 0xa
                | (fpu_val_6_1 == f0_1 ? 1 : 0) << 0xe | 0x3800) + 1) & 1))
            {
                long double fpu_val_6_2 = item_2c;
                long double f1_1 = temp_1[-2];
                fpu_val_6_2 - f1_1;

                if (!(*(&((fpu_val_6_2 < f1_1 ? 1 : 0) << 8
                    | (FCMP_UO(fpu_val_6_2, f1_1) ? 1 : 0) << 0xa
                    | (fpu_val_6_2 == f1_1 ? 1 : 0) << 0xe | 0x3800) + 1) & 0x41))
                {
                    int dest_3 = *(temp_1 - 0xc);
                    item_2c = temp_1[-2];
                    *(&item_2c + 4) = dest_3;
                }
            }
            else
            {
                int dest_2 = *(temp_1 - 0xc);
                item_44 = temp_1[-2];
                *(&item_44 + 4) = dest_2;
            }

            long double fpu_val_6_3 = item_3c;
            long double f2_1 = temp_1[-1];
            fpu_val_6_3 - f2_1;

            if (!(*(&((fpu_val_6_3 < f2_1 ? 1 : 0) << 8
                | (FCMP_UO(fpu_val_6_3, f2_1) ? 1 : 0) << 0xa
                | (fpu_val_6_3 == f2_1 ? 1 : 0) << 0xe | 0x3800) + 1) & 1))
            {
                long double fpu_val_6_4 = item_24;
                long double f3_1 = temp_1[-1];
                fpu_val_6_4 - f3_1;

                if (!(*(&((fpu_val_6_4 < f3_1 ? 1 : 0) << 8
                    | (FCMP_UO(fpu_val_6_4, f3_1) ? 1 : 0) << 0xa
                    | (fpu_val_6_4 == f3_1 ? 1 : 0) << 0xe | 0x3800) + 1) & 0x41))
                {
                    item_24 = temp_1[-1];
                    *(&item_24 + 4) = *(temp_1 - 4);
                }
            }
            else
            {
                item_3c = temp_1[-1];
                *(&item_3c + 4) = *(temp_1 - 4);
            }

            long double fpu_val_6_5 = item_34;
            long double f5_1 = *temp_1;
            fpu_val_6_5 - f5_1;

            if (!(*(&((fpu_val_6_5 < f5_1 ? 1 : 0) << 8
                | (FCMP_UO(fpu_val_6_5, f5_1) ? 1 : 0) << 0xa
                | (fpu_val_6_5 == f5_1 ? 1 : 0) << 0xe | 0x3800) + 1) & 1))
            {
                long double f6_1 = *temp_1;
                fpu_val_7_1 - f6_1;

                if (!(*(&((fpu_val_7_1 < f6_1 ? 1 : 0) << 8
                        | (FCMP_UO(fpu_val_7_1, f6_1) ? 1 : 0) << 0xa
                        | (fpu_val_7_1 == f6_1 ? 1 : 0) << 0xe | 0x3800) + 1) & 0x41))
                    fpu_val_7_1 = *temp_1;
            }
            else
            {
                item_34 = *temp_1;
                *(&item_34 + 4) = *(temp_1 + 4);
            }

            *source_2 = 0;
            temp_1 = &temp_1[3];
            source_2 = &source_2[1];
            i = i_1;
            i_1 -= 1;
        } while (i != 1);
        item_1c = fpu_val_7_1;
        context_val = context;
    }

    int data_8 = *(context_val + 0xde0);
    int context_2 = *(context_val + 0xde4);
    int temp_2 = *(context + 0xde8);
    double item_1c_1 = item_1c - 2.0;
    long long item_c_2 = item_24 - 2.0;
    int dest_7 = item_c_2;
    *(&item_c_2 + 4) = temp_2;
    *(&item_1c_1 + 4) = item_1c_1 + temp_2;
    int data_13 = ((item_34 + 2.0 + *(&item_c_2 + 4)) ^ *(&item_1c_1 + 4))
        | ((item_3c + 2.0 + context_2) ^ (dest_7 + context_2))
        | ((item_44 + 2.0 + data_8) ^ (item_2c - 2.0 + data_8));
    *(&item_c_2 + 4) = data_13;
    int eflags;
    unsigned int result;
    result = __bsr_gprv_memv(*(&item_c_2 + 4));
    *(&item_c_2 + 4) = result;
    int source_5 = 0x1e - *(&item_c_2 + 4);
    double* context_5;

    if (!(arg7 & 0x7fffffff))
        context_5 = arg3;
    else
    {
        context_5 = arg3;
        int* item_f8_1 = arg2;
        char item_74_1 = arg5;
        unsigned int item_fc_1 = arg4;
        double* item_100 = context_5;
        int item_70_1 = arg6;
        int item_6c_1 = 0;
        result = DynObj_ParseConfigFile_11(context, 0xb, &item_100, context_5, arg7);
    }

    if (arg7 & 0x80000000)
    {
        result = context;

        if (*(result + 0xc))
        {
            if (source_5 > 0x10)
                source_5 = 0x10;

            int* item_118_2 = &item_68;
            arg5 = DynObj_RegisterSymbol_10(result, &item_68, context_5, source_5);
            result = arg4;
            int dest_10 = 0;

            if (result > 0)
            {
                int* source_6 = arg2;

                do
                {
                    void* data_17 = *source_6;

                    if (item_f4[dest_10])
                    {
                        long double fpu_val_7_7;
                        long double f4_1;

                        if (data_17)
                        {
                            fpu_val_7_7 = *(data_17 + 0xc);
                            f4_1 = 0.0f;
                            fpu_val_7_7 - f4_1;
                        }

                        if (!data_17 || *(&((fpu_val_7_7 < f4_1 ? 1 : 0) << 8
                                | (FCMP_UO(fpu_val_7_7, f4_1) ? 1 : 0) << 0xa
                                | (fpu_val_7_7 == f4_1 ? 1 : 0) << 0xe) + 1) & 0x40)
                            *source_6 = DynObj_FindSymbol_10(&item_68, context_5, arg5);
                    }
                    else if (!data_17)
                        *source_6 = *(context + 0xd3c);

                    result = arg4;
                    dest_10 += 1;
                    source_6 = &source_6[1];
                    context_5 = &context_5[3];
                } while (dest_10 < result);
            }
        }
    }

    return result;
}
