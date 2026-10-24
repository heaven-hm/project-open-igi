// IGI native provenance: DynObj_CreateMagicObject_3 - 0x004C0E60 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004C0E60-DynObj_CreateMagicObject_3.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "DynObj_CreateMagicObject_3.h"
int DynObj_CreateMagicObject_3(int* context, int* arg2, char arg3)

{
    void item_2c8;
    int result = Sys_PushStack_26(&item_2c8, arg2[6]);

    if (result)
    {
        int result_1;
        result = result_1;

        if (result == 9 || result == 0xc)
        {
            char item_2cd = arg3;
            int item_2cc = 0;
            void item_2bf;

            if (result != 9)
                item_2cc = DynObj_FindSymbol_3(&item_2bf) + 0xfffffffb - *arg2;
            else
            {
                void item_2c0;
                void** data_1 = Sys_ReadFileBlock_26(arg2, &item_2c0);

                if (!data_1)
                    arg2[1] = 1;
                else
                {
                    data_1[2] = *arg2 + 1;
                    data_1[3] = *arg2 + 5;
                }
            }
            *context = 5;
            Sys_CloseFileStream_4(arg2[4], &item_2cd, 1);
            return Sys_CloseFileStream_4(arg2[4], &item_2cc, 4);
        }
    }

    arg2[1] = 1;
    return result;
}
