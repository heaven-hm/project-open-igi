// IGI native provenance: Camera_E000 - 0x004A72CE - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004A72CE-Camera_E000.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Camera_E000.h"
char* Camera_E000(double* context, char* arg2, void* arg3, int arg4)

{
    void* context_val;
    int* source_val;

    if (!g_9361c4)
    {
        int saved_index_2;
        int saved_index_1 = saved_index_2;
        int saved_index_3 = saved_index_2;
        saved_index_3 = *context;
        Camera_SetFieldOfView_26();
        context_val = arg3;
        source_val = &g_936238;
        int data_3 = 0;
        data_3 = g_936238 == 0x2d;
        void* entity_4 = nullptr;
        entity_4 = context_val > 0;
        Camera_UpdateTransform_26(entity_4 + &arg2[data_3], context_val + 1, &g_936238);
    }
    else
    {
        context_val = arg3;
        int* data_val = g_9361c0;
        int entity_val = 0;
        source_val = data_val;
        entity_val = context_val > 0;
        void* entity_1 = nullptr;
        entity_1 = *data_val == 0x2d;
        Camera_ApplyBandFilter_11(entity_1 + arg2, entity_val);
    }

    char* data_4 = arg2;

    if (*source_val == 0x2d)
    {
        *data_4 = 0x2d;
        data_4 = &data_4[1];
    }

    if (context_val > 0)
    {
        *data_4 = data_4[1];
        data_4 = &data_4[1];
        *data_4 = g_544280;
    }

    void* const entity_7 = nullptr;
    entity_7 = !g_9361c4;
    char* data_5 = Camera_ApplyLineFilter_20(entity_7 + data_4 + context_val, "e+000");

    if (arg4)
        *data_5 = 0x45;

    if (*source_val[3] != 0x30)
    {
        int context_1 = source_val[1];
        int context_2 = context_1 - 1;

        if (context_1 - 1 < 0)
        {
            context_2 = -(context_2);
            data_5[1] = 0x2d;
        }

        if (context_2 >= 0x64)
        {
            data_5[2] += context_2 / 0x64;
            context_2 = context_2 % 0x64;
        }

        if (context_2 >= 0xa)
        {
            data_5[3] += context_2 / 0xa;
            context_2 = context_2 % 0xa;
        }

        data_5[4] += context_2;
    }

    return arg2;
}
