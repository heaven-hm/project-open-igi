// IGI native provenance: Script_SpawnNewTask_31 - 0x004FE120 - CFG score 66.7% (FAIL; structural only); runtime behavior not verified.
// Evidence: 004FE120-Script_SpawnNewTask_31.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Script_SpawnNewTask_31.h"
int Script_SpawnNewTask_31(void* context, int arg2, int* arg3, int arg4, int arg5, int arg6, int arg7, short arg8, int arg9)

{
    int data_val = *(context + 0xf0);

    if (data_val < *(context + 0xf8))
    {
        void* data_1 = *(*(context + 0x6c) + (data_val << 2));
        memcpy(data_1, arg2, 0x18);
        memcpy(data_1 + 0x40, arg2, 0x18);
        *(data_1 + 0x18) = *arg3;
        *(data_1 + 0x1c) = arg3[1];
        *(data_1 + 0x20) = arg3[2];
        *(data_1 + 0x24) = g_a7a4dc;
        int temp_5 = (*g_a7a4dc + 1) & 0x8000000f;

        if (temp_5 < 0)
            temp_5 = ((temp_5 - 1) | 0xfffffff0) + 1;

        g_a7a4dc = temp_5;
        *(data_1 + 0x26) = arg8;
        *(data_1 + 0x30) = arg4;
        *(data_1 + 0x38) = arg6;
        *(data_1 + 0x3c) = arg7;
        *(data_1 + 0x2c) = arg5;
        *(data_1 + 0x28) = 0;
        *(data_1 + 0x2a) = 0;

        if (arg9)
        {
            *(data_1 + 0x34) = 1.0f / arg9;
            int data_3 = *(context + 0xf0) + 1;
            *(context + 0xf0) = data_3;
            return data_3;
        }

        *(data_1 + 0x34) = 0x3f800000;
        data_val = *(context + 0xf0) + 1;
        *(context + 0xf0) = data_val;
    }

    return data_val;
}
