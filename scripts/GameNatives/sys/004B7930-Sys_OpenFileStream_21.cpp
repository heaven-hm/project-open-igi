// IGI native provenance: Sys_OpenFileStream_21 - 0x004B7930 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004B7930-Sys_OpenFileStream_21.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004B7930-Sys_OpenFileStream_21.hpp"
int Sys_OpenFileStream_21(float context, float arg2, float arg3, float arg4, float arg5, float arg6, int arg7, int arg8, int arg9, int arg10)

{
    int* context_val = context;
    int* dest_val = context_val[2];
    int data_val = Sys_CloseFileStream_17(dest_val);
    int data_1 = Sys_ExecuteBytecode_17(dest_val);
    float item_20 = arg3;
    int data_3 = *context_val;
    int source_val = 0;

    if (!data_3)
    {
        int entity_1 = 0;
        int item_18_1 = 0;
        arg3 = 0.0f;

        if (*(context_val + 6) > 0)
        {
            int source_2 = arg9 & 0x80;
            int item_10_1 = source_2;
            int data_15;

            do
            {
                if (source_2)
                {
                    int data_5;
                    int temp_2;
                    temp_2 = HIGHD(*(context_val[3] + (entity_1 << 1)));
                    data_5 = LOWD(*(context_val[3] + (entity_1 << 1)));
                    context = arg2 - ((data_5 - temp_2) >> 1);
                }
                else if (!(arg9 & 0x100))
                    context = arg2;
                else
                    context = arg2 - *(context_val[3] + (entity_1 << 1));

                int item_1c_1 = 0;

                if (context_val[1] > 0)
                {
                    bool flag_6_1;

                    do
                    {
                        char data_8 = *(arg3 + context_val + 0x10);

                        if (data_8)
                        {
                            Sys_AllocateMemory_15(dest_val, context, item_20, arg4, arg5, arg6, arg7, arg8, data_8 - 1,
                                arg9, arg10);
                            entity_1 = item_18_1;
                        }

                        context = data_val + context;
                        arg3 += 1;
                        flag_6_1 = item_1c_1 + 1 < context_val[1];
                        item_1c_1 += 1;
                    } while (flag_6_1);
                    source_2 = item_10_1;
                }

                data_15 = *(context_val + 6);
                entity_1 += 1;
                item_18_1 = entity_1;
                item_20 = data_1 + item_20;
            } while (entity_1 < data_15);

            return data_15;
        }
    }
    else if (data_3 == 1)
    {
        data_3 = Sys_DispatchOpcode_17(dest_val);
        int item_14_1 = data_3;
        int item_18_2 = 0;
        arg3 = 0.0f;

        if (*(context_val + 6) > 0)
        {
            int entity_5 = arg9;
            int flagP3 = entity_5 & 0x80;
            int item_10_2 = flagP3;

            do
            {
                if (flagP3)
                {
                    int data_17;
                    int temp_9;
                    temp_9 = HIGHD(*(context_val[3] + (source_val << 1)));
                    data_17 = LOWD(*(context_val[3] + (source_val << 1)));
                    context = arg2 - ((data_17 - temp_9) >> 1);
                }
                else if (!(*(&entity_5 + 1) & 1))
                    context = arg2;
                else
                    context = arg2 - *(context_val[3] + (source_val << 1));

                float item_1c_2 = 0.0f;

                if (context_val[1] > 0)
                {
                    bool flag_7_1;

                    do
                    {
                        char data_20 = *(arg3 + context_val + 0x10);

                        if (!data_20)
                            context = item_14_1 + context;
                        else
                        {
                            unsigned int source_5 = data_20 - 1;
                            Sys_AllocateMemory_15(dest_val, context, item_20, arg4, arg5, arg6, arg7, arg8, source_5,
                                entity_5, arg10);
                            source_val = item_18_2;
                            entity_5 = arg9;
                            context = *(dest_val[2] + source_5 * 0x24 + 0x10) + context;
                        }

                        arg3 += 1;
                        flag_7_1 = item_1c_2 + 1 < context_val[1];
                        item_1c_2 += 1;
                    } while (flag_7_1);
                    flagP3 = item_10_2;
                }

                data_3 = *(context_val + 6);
                source_val += 1;
                item_18_2 = source_val;
                item_20 = data_1 + item_20;
            } while (source_val < data_3);
        }
    }

    return data_3;
}
