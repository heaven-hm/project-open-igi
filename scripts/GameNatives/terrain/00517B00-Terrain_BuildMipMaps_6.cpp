// IGI native provenance: Terrain_BuildMipMaps_6 - 0x00517B00 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00517B00-Terrain_BuildMipMaps_6.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Terrain_BuildMipMaps_6.hpp"
int Terrain_BuildMipMaps_6(int context, int* arg2, int* arg3, int arg4)

{
    int* entity_val = arg3;
    int result = arg4 >> 1;
    int context_1 = entity_val >> 1;
    int item_8 = context_1;

    if (result > 0)
    {
        int temp_1 = context;
        int source_1 = entity_val << 3;
        arg3 = arg2;
        int item_4_1 = source_1;
        int* dest_2 = &arg2[entity_val];
        context = result;
        arg2 = dest_2;
        bool flag_1_1;

        do
        {
            int* data_1 = arg3;
            int entity_1 = temp_1 + (context_1 << 2);
            arg4 = temp_1;
            int item_c_1 = entity_1;

            if (temp_1 < entity_1)
            {
                while (true)
                {
                    int entity_2 = *data_1;
                    int temp_2 = data_1[1];
                    int source_2 = *dest_2;
                    int data_4 = dest_2[1];
                    int dest_11 = (((source_2 & 0xff00) + (temp_2 & 0xff00) + (entity_2 & 0xff00)
                        + (data_4 & 0xff00) + 2) & 0x3fc00) | (((data_4 & 0xff0000) + (source_2 & 0xff0000)
                        + (temp_2 & 0xff0000) + (entity_2 & 0xff0000) + 2) & 0x3fc0000);
                    entity_1 = item_c_1;
                    int dest_14 = (dest_11 | (data_4 + source_2 + temp_2 + entity_2 + 2)) >> 2 | (((
                        data_4 >> 2 & 0x3fc00000) + (source_2 >> 2 & 0x3fc00000) + (temp_2 >> 2 & 0x3fc00000)
                        + (entity_2 >> 2 & 0x3fc00000) + 0x800000) & 0xff000000);
                    *arg4 = dest_14;
                    bool flag_3_1 = arg4 + 4 < entity_1;
                    arg4 += 4;

                    if (!flag_3_1)
                        break;

                    data_1 = &data_1[2];
                    dest_2 = &dest_2[2];
                }

                context_1 = item_8;
                source_1 = item_4_1;
            }

            temp_1 = entity_1;
            arg3 += source_1;
            dest_2 = arg2 + source_1;
            result = context - 1;
            flag_1_1 = context != 1;
            arg2 = dest_2;
            context = result;
        } while (flag_1_1);
    }

    return result;
}
