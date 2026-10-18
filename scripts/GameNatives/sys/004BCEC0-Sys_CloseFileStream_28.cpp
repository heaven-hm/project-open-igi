// IGI native provenance: Sys_CloseFileStream_28 - 0x004BCEC0 - CFG score 91.7% (PASS; structural only); runtime behavior not verified.
// Evidence: 004BCEC0-Sys_CloseFileStream_28.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004BCEC0-Sys_CloseFileStream_28.hpp"
int Sys_CloseFileStream_28(int* context, int* arg2, float* arg3)

{
    int* data_7 = context;
    int entity_1 = *(data_7[0xa] + (data_7[7] << 2));
    int* item_30;

    if (entity_1 == 1)
    {
        int* saved_target = nullptr;
        item_30 = nullptr;
        Sys_UnknownDataType_3(data_7, arg2, item_30, saved_target);
        *arg3 = *arg2;
        return 1;
    }

    if (entity_1 == 2)
    {
        int* saved_index_1 = nullptr;
        item_30 = nullptr;
        Sys_UnknownDataType_3(data_7, arg3, item_30, saved_index_1);
        *arg2 = *arg3;
        return 2;
    }

    int* saved_index_2 = &context;
    item_30 = data_7;

    if (Sys_OpenFileStream_28(item_30, saved_index_2))
    {
        int* data_4 = context;
        int entity_3 = *data_4;

        if (!entity_3 || entity_3 == 2)
        {
            int temp_2 = data_4[6];
            item_30 = nullptr;
            int item_18;
            int data_5 = data_4[4](&item_18, temp_2, item_30);
            int result = context[1];
            memcpy(&item_18, data_5, 0x18);

            if (result == 1)
            {
                long double fpu_val_7_2 = item_18;
                *arg2 = item_18;
                *arg3 = fpu_val_7_2;
                return result;
            }

            if (result == 2)
            {
                double item_10;
                *arg2 = item_10;
                *arg3 = item_10;
                return result;
            }
        }
    }

    return 0;
}
