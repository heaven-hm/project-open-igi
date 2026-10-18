// IGI native provenance: Camera_ResetMatrix_10 - 0x004A74B0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004A74B0-Camera_ResetMatrix_10.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Camera_ResetMatrix_10.h"
char* Camera_ResetMatrix_10(double* context, char* arg2, int arg3, int arg4)

{
    int entity_val;
    int item_14 = entity_val;
    int item_18 = entity_val;
    item_18 = *context;
    Camera_SetFieldOfView_26();
    g_9361c0 = &g_936238;
    g_9361c8 = g_93623c - 1;
    void* entity_3 = nullptr;
    entity_3 = g_936238 == 0x2d;
    char* source_val = entity_3 + arg2;
    Camera_UpdateTransform_26(source_val, arg3, &g_936238);
    void* data_val = g_9361c0;
    bool entity_7 = g_9361c8 < *(data_val + 4) - 1;
    g_9361cc = entity_7;
    int data_2 = *(data_val + 4) - 1;
    g_9361c8 = data_2;

    if (data_2 < 0xfffffffc || data_2 >= arg3)
        return Camera_UpdateTransform_11(context, arg2, arg3, arg4);

    if (entity_7)
    {
        char i;

        do
        {
            i = *source_val;
            source_val = &source_val[1];
        } while (i);

        source_val[0xfffffffe] &= i;
    }

    return Camera_SetFieldOfView_11(context, arg2, arg3);
}
