// IGI native provenance: Camera_ApplyLineFilter_26 - 0x004ACC4D - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004ACC4D-Camera_ApplyLineFilter_26.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Camera_ApplyLineFilter_26.h"
int* Camera_ApplyLineFilter_26(int* context, int* arg2)

{
    int entity_val;
    int item_8 = entity_val;
    int data_val;
    data_val = *(arg2 + 6);
    int entity_3 = data_val >> 4 & 0x7ff;
    int temp_1 = *arg2;
    unsigned int context_val = entity_3;
    int data_3 = arg2[1] & 0xfffff;
    int item_8_1 = 0x80000000;
    int dest_1;

    if (context_val)
    {
        dest_1 = context_val == 0x7ff ? 0x7fff : entity_3 + 0x3c00;

        goto label_4accc6;
    }

    int* result;

    if (data_3 || temp_1)
    {
        dest_1 = entity_3 + 0x3c01;
        item_8_1 = 0;
    label_4accc6:
        result = context;
        int entity_7 = temp_1 >> 0x15 | data_3 << 0xb | item_8_1;
        result[1] = entity_7;
        *result = temp_1 << 0xb;

        while (!(0x80000000 & entity_7))
        {
            int temp_3 = *result;
            int context_3 = temp_3 >> 0x1f | (entity_7 * 2);
            *result = temp_3 * 2;
            result[1] = context_3;
            dest_1 += 0xffff;
            entity_7 = context_3;
        }

        result[2] = (data_val & 0x8000) | dest_1;
    }
    else
    {
        result = context;
        result[1] = 0;
        *result = 0;
        result[2] = 0;
    }

    return result;
}
