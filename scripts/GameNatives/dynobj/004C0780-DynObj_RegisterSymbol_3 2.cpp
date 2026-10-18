// IGI native provenance: DynObj_RegisterSymbol_3 - 0x004C0780 - CFG score 90.5% (PASS; structural only); runtime behavior not verified.
// Evidence: 004C0780-DynObj_RegisterSymbol_3.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "DynObj_RegisterSymbol_3.h"
int DynObj_RegisterSymbol_3(int context, int arg2, int arg3, void* arg4, int* arg5)

{
    int item_4 = arg3;
    int* flagP = arg5;
    int dest_val = 0;
    int* context_val = flagP[2];

    if (*context_val)
    {
        void* dest_1 = &context_val[2];
        int i = 0xffffffff;

        while (i)
        {
            bool flag_0_1 = 0 != *dest_1;
            dest_1 += 1;
            i -= 1;

            if (!flag_0_1)
                break;
        }

        int entity_val = ~i;
        int source_2;
        int dest_4;
        dest_4 = memcpy(arg4 + 8, dest_1 - entity_val, entity_val & 0xfffffffc);
        memcpy(dest_4, source_2, entity_val & 3);
        *(arg4 + 0x2c4) = context_val[0xb1];
        HeapList_Remove(context_val);
        Sys_ExecuteBytecode_2(context_val);
        return 1;
    }

    int i_1;

    do
    {
        int* data_2 = flagP[1];

        if (**data_2)
        {
            arg5 = nullptr;
            int* j_1 = **flagP;
            int* j = *j_1;

            if (j)
            {
                do
                {
                    if (Sys_AllocateMemory(&item_4, nullptr, *flagP[1], &j_1[0x42], &j_1[0xa42], 0))
                    {
                        int data_7 = item_4;

                        if (data_7 >= dest_val)
                        {
                            dest_val = data_7;
                            arg5 = j_1;

                            if (j_1[0x1242])
                                break;
                        }
                    }

                    j_1 = j;
                    j = *j;
                } while (j);

                if (arg5)
                {
                    int data_10 = dest_val;

                    if (dest_val >= 0x2bc)
                        data_10 = 0x2bc;

                    Camera_ApplyLineFilter_5(arg4 + 8, *flagP[1], data_10);
                    int data_12 = dest_val;

                    if (dest_val >= 0x2bc)
                        data_12 = 0x2bc;

                    *(data_12 + arg4 + 8) = 0;
                    int* data_13 = flagP[1];
                    *data_13 += dest_val;
                    *(arg4 + 0x2c4) = arg5[0x1243];
                    return 1;
                }
            }

            break;
        }

        i_1 = Sys_CheckHeap_30(data_2);
    } while (i_1);
    return 0;
}
