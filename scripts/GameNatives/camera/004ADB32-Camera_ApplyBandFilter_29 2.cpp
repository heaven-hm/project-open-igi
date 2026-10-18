// IGI native provenance: Camera_ApplyBandFilter_29 - 0x004ADB32 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004ADB32-Camera_ApplyBandFilter_29.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Camera_ApplyBandFilter_29.h"
int Camera_ApplyBandFilter_29(char* context, int arg2, int arg3)

{
    int result = arg2;
    int* context_val = arg3;
    int item_8 = 0x404e;
    *context_val = 0;
    context_val[1] = 0;
    context_val[2] = 0;

    if (result > 0)
    {
        arg3 = result;
        int i;

        do
        {
            int item_14 = *context_val;
            int item_10;
            int* dest_1 = &item_10;
            int* source_2 = &context_val[1];
            *dest_1 = *source_2;
            dest_1[1] = source_2[1];
            Camera_SetFieldOfView_29(context_val);
            Camera_SetFieldOfView_29(context_val);
            Camera_UpdateTransform_29(context_val, &item_14);
            Camera_SetFieldOfView_29(context_val);
            item_10 = 0;
            int item_c_1 = 0;
            item_14 = *context;
            result = Camera_UpdateTransform_29(context_val, &item_14);
            context = &context[1];
            i = arg3;
            arg3 -= 1;
        } while (i != 1);
    }

    while (!context_val[2])
    {
        int entity_1 = context_val[1];
        context_val[2] = entity_1 >> 0x10;
        int data_4 = *context_val;
        result = data_4 << 0x10;
        item_8 += 0xfff0;
        context_val[1] = data_4 >> 0x10 | entity_1 << 0x10;
        *context_val = result;
    }

    while (!(context_val[2] & 0x8000))
    {
        result = Camera_SetFieldOfView_29(context_val);
        item_8 += 0xffff;
    }

    result = item_8;
    *(context_val + 0xa) = result;
    return result;
}
