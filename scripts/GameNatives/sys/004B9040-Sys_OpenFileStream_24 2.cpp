// IGI native provenance: Sys_OpenFileStream_24 - 0x004B9040 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004B9040-Sys_OpenFileStream_24.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Sys_OpenFileStream_24.h"
int Sys_OpenFileStream_24(long double context , char* arg2, void* arg3)

{
    int data_val = *(arg3 + 4);
    int data_3;

    if (data_val == 1)
    {
        int data_11 = *(arg3 + 0xc);

        if (data_11 != 1)
        {
            if (data_11 == 2)
            {
                int data_15 = Sys_ReadFileBlock_24(arg2);
                **(arg3 + 0x18) = data_15;
                return data_15;
            }

            int data_14 = Sys_ReadFileBlock_24(arg2);
            **(arg3 + 0x18) = data_14;
            return data_14;
        }

        data_3 = Sys_ReadFileBlock_24(arg2);
        **(arg3 + 0x18) = data_3;
    }
    else
    {
        if (data_val == 2)
        {
            long double fpu0;
            long double fpu1;
            int data_10 = Sys_DispatchOpcode_24(fpu0, fpu1, arg2);
            **(arg3 + 0x18) = context;
            return data_10;
        }

        data_3 = data_val - 3;

        if (data_val == 3)
        {
            int data_5 = *(arg3 + 0xc) - 1;
            void item_800;
            char* data_6 = Sys_SyntaxErrorInStringE(arg2, &item_800, 0x800);
            char* data_8 = Camera_ApplyLineFilter_5(*(arg3 + 0x18), data_6, data_5);
            *(*(arg3 + 0x18) + *(arg3 + 0xc) - 1) = 0;
            return data_8;
        }
    }

    return data_3;
}
