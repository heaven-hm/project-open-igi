// IGI native provenance: Sys_CloseFileStream_29 - 0x004BD880 - CFG score 95.2% (PASS; structural only); runtime behavior not verified.
// Evidence: 004BD880-Sys_CloseFileStream_29.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004BD880-Sys_CloseFileStream_29.hpp"
void* Sys_CloseFileStream_29(int* context)

{
    char* context_val = context;
    Sys_ExecuteBytecode_29(context_val);

    if (!Sys_OpenFileStream_28(context_val, &context))
        goto label_4bd99c;

    int* entity_1 = context;
    void* result;

    if (*entity_1 != 1)
    {
        int temp_6 = *(context_val + 0x14);
        *(context_val + 0x20) = 9;
        *(context_val + 0x10) = temp_6;
    label_4bd99c:
        result = *(context_val + 0x20);

        if (!result)
        {
            int data_10 = *(context_val + 0x14);
            *(context_val + 0x20) = 0xa;
            *(context_val + 0x10) = data_10;
            return Sys_DispatchOpcode_29(context_val);
        }
    }
    else
    {
        int flagP1 = *(context_val + 0x14);
        int source_1 = *(context_val + 0x1c);
        void* data_1 = *context_val + flagP1;
        int saved_source_8 = *(context_val + 0x18);
        int temp_2 = *(data_1 + 1);
        void* item_1c_1 = data_1 + 5;
        int item_24_1 = 0;
        int item_28_1 = 1;
        int item_2c = temp_2;
        char* item_20_1 = context_val;
        int temp_3 = entity_1[6];
        int saved_source_7 = saved_source_8;
        int item_18;
        int data_3 = entity_1[4](&item_18, temp_3, &item_2c);
        result = *(context_val + 0x20);
        memcpy(&item_18, data_3, 0x18);

        if (!result)
        {
            *(context_val + 0x2c) = 0;
            *(context_val + 0x1c) = source_1;
            int saved_source_6 = saved_source_7;
            *(context_val + 0x14) = flagP1 + (temp_2 << 2) + 5;
            int* temp_5 = context;
            *(context_val + 0x18) = saved_source_6;
            int data_5 = temp_5[1];

            if (data_5 == 3)
            {
                int item_8;
                return Sys_ReadFileBlock_28(context_val, item_8, data_5);
            }

            if (data_5 != 2)
                return Sys_PushStack_29(context_val, item_18);

            int saved_source_2 = saved_source_6;
            double item_10;
            return Sys_PopStack_29(context_val, item_10);
        }
    }

    return result;
}
