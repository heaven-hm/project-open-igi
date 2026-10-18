// IGI native provenance: Terrain_RenderWaterSurface_7 - 0x00517FD0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00517FD0-Terrain_RenderWaterSurface_7.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Terrain_RenderWaterSurface_7.h"
void* Terrain_RenderWaterSurface_7(void* context, int arg2, int arg3, int arg4, char* arg5, int arg6, int* arg7, int arg8, int* arg9)

{
    int* temp_12 = arg9;
    int data_val;
    int entity_val;
    int* context_val;

    if (!temp_12)
    {
        context_val = arg7;
        entity_val = 0;
        data_val = 0;
        arg9 = context_val;
        arg8 = arg8;
    }
    else
    {
        context_val = arg7;
        entity_val = *temp_12;
        int* dest_1 = temp_12[2];
        data_val = temp_12[1];
        arg9 = dest_1;
        int* source_2 = context_val - entity_val;

        if (dest_1 >= source_2)
            arg9 = source_2;

        int temp_val = temp_12[3];
        int dest_3 = arg8 - data_val;

        arg8 = temp_val < dest_3 ? temp_val : dest_3;
    }

    int flagP = arg4;
    void* item_8;

    if (flagP)
        item_8 = data_val * context_val + entity_val + flagP;

    int temp_5 = 0;
    int item_4 = 0;
    char* entity_1 = data_val * context_val + entity_val + arg5;
    void* result = context;
    arg5 = entity_1;
    context = result;

    if (arg3 > 0)
    {
        do
        {
            int dest_5 = 0;

            if (arg2 > 0)
            {
                char* data_4 = result + 3;
                void* flagP2 = item_8 - entity_1;

                do
                {
                    if (dest_5 >= arg9 || temp_5 >= arg8)
                    {
                        *data_4 = 0;
                        data_4[0xffffffff] = 0;
                        data_4[0xfffffffe] = 0;
                        data_4[0xfffffffd] = 0;
                    }
                    else
                    {
                        char temp_7;

                        if (!arg4)
                        {
                            char temp_8 = *entity_1;
                            char f0_1 = temp_8;
                            temp_8 = -(temp_8);
                            temp_7 = (temp_8 - temp_8) & 0xff;
                        }
                        else
                            temp_7 = *(entity_1 + flagP2);

                        *data_4 = temp_7;
                        int temp_9 = 0;
                        temp_9 = *entity_1;
                        data_4[0xffffffff] = *(arg6 + (temp_9 << 2) + 2);
                        int context_3 = 0;
                        context_3 = *entity_1;
                        data_4[0xfffffffe] = *(arg6 + (context_3 << 2) + 1);
                        int context_5 = 0;
                        context_5 = *entity_1;
                        data_4[0xfffffffd] = *(arg6 + (context_5 << 2));
                        temp_5 = item_4;
                    }

                    dest_5 += 1;
                    entity_1 = &entity_1[1];
                    data_4 = &data_4[4];
                } while (dest_5 < arg2);

                context_val = arg7;
                flagP = arg4;
                entity_1 = arg5;
                result = context;
            }

            if (flagP)
                item_8 += context_val;

            entity_1 += context_val;
            temp_5 += 1;
            result += arg2 << 2;
            arg5 = entity_1;
            context = result;
            item_4 = temp_5;
        } while (temp_5 < arg3);
    }

    return result;
}
