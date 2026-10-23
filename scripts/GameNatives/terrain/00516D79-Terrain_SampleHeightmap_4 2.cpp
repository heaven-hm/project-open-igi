// IGI native provenance: Terrain_SampleHeightmap_4 - 0x00516D79 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00516D79-Terrain_SampleHeightmap_4.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Terrain_SampleHeightmap_4.h"
unsigned int Terrain_SampleHeightmap_4(char* context, int arg2, unsigned int arg3, int* arg4)

{
    int entity_val;
    int item_8 = entity_val;
    unsigned int dest_1 = arg2 * arg3;
    unsigned int item_8_1 = dest_1;
    unsigned int context_val = dest_1;

    if (!dest_1)
        return 0;

    int* source_1 = arg4;

    if (!(source_1[3] & 0x10c))
        arg4 = 0x1000;
    else
        arg4 = source_1[6];

    unsigned int data_12;

    while (true)
    {
        int entity_2 = source_1[3] & 0x108;
        unsigned int data_3;

        if (entity_2)
            data_3 = source_1[1];

        if (entity_2 && data_3)
        {
            unsigned int dest_2 = context_val;

            if (context_val >= data_3)
                dest_2 = data_3;

            Camera_ResetMatrix_20(*source_1, context, dest_2);
            source_1[1] -= dest_2;
            *source_1 += dest_2;
            context_val -= dest_2;
            context = &context[dest_2];
            dest_1 = item_8_1;
        }
        else if (context_val < arg4)
        {
            int item_1c_3 = *context;

            if (_flsbuf(source_1) == 0xffffffff)
            {
                data_12 = dest_1;
                break;
            }

            context = &context[1];
            int* data_11 = source_1[6];
            context_val -= 1;
            arg4 = data_11;

            if (data_11 <= 0)
                arg4 = 1;
        }
        else
        {
            bool flag_3_1;

            if (entity_2)
                flag_3_1 = Camera_ApplyHandiCam_9(source_1);

            if (entity_2 && flag_3_1)
            {
                data_12 = dest_1;
                break;
            }

            unsigned int dest_4;

            dest_4 = !arg4 ? context_val : context_val - COMBINE(0, context_val) % arg4;

            int data_7 = Camera_ApplyHandiCam_23(source_1[4], context, dest_4);

            if (data_7 != 0xffffffff)
            {
                context = &context[data_7];
                context_val -= data_7;
            }

            if (data_7 == 0xffffffff || data_7 < dest_4)
            {
                source_1[3] |= 0x20;
                data_12 = item_8_1;
                break;
            }

            dest_1 = item_8_1;
        }

        if (!context_val)
            return arg3;
    }

    return COMBINE(0, data_12 - context_val) / arg2;
}
