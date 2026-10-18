// IGI native provenance: Terrain_SelectTerrainLOD_7 - 0x00517DD0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00517DD0-Terrain_SelectTerrainLOD_7.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Terrain_SelectTerrainLOD_7.h"
short* Terrain_SelectTerrainLOD_7(char* context, int arg2, short* arg3, short* arg4, int arg5, int arg6, int* arg7)

{
    int entity_val;
    int item_4 = entity_val;
    int data_val;
    int entity_1;
    int source_val;

    if (!arg7)
    {
        entity_1 = arg5;
        source_val = 0;
        data_val = 0;
        arg5 = entity_1;
        arg6 = arg6;
    }
    else
    {
        entity_1 = arg5;
        source_val = *arg7;
        int context_1 = arg7[2];
        data_val = arg7[1];
        arg5 = context_1;
        int dest_2 = entity_1 - source_val;

        if (context_1 >= dest_2)
            arg5 = dest_2;

        int temp_1 = arg7[3];
        int dest_4 = arg6 - data_val;

        arg6 = temp_1 < dest_4 ? temp_1 : dest_4;
    }

    arg7 = nullptr;
    short* result = &arg4[data_val * entity_1 + source_val];

    if (arg3 > 0)
    {
        int temp_4 = arg2;
        short* result_1 = result;
        char* source_1 = &context[2];
        arg4 = result_1;
        context = source_1;
        bool flag_0_1;

        do
        {
            int flagP1 = 0;

            if (temp_4 > 0)
            {
                do
                {
                    int* data_4;

                    if (flagP1 < arg5)
                        data_4 = arg7;

                    char data_6;
                    char entity_6;
                    char temp_6;
                    char context_2;

                    if (flagP1 >= arg5 || data_4 >= arg6)
                    {
                        data_6 = 0;
                        entity_6 = 0;
                        temp_6 = 0;
                        context_2 = 0;
                    }
                    else
                    {
                        data_4 = *result_1;
                        temp_6 = *(&data_4 + 1) & 0xf8;
                        entity_6 = data_4 >> 3 & 0xfc;
                        context_2 = -1;
                        data_6 = (data_4 & 0x1f) << 3;
                    }

                    source_1[1] = context_2;
                    *source_1 = temp_6;
                    source_1[0xffffffff] = entity_6;
                    source_1[0xfffffffe] = data_6;
                    flagP1 += 1;
                    result_1 = &result_1[1];
                    source_1 = &source_1[4];
                } while (flagP1 < arg2);

                temp_4 = arg2;
            }

            source_1 = &context[temp_4 << 2];
            result = arg3;
            result_1 = &arg4[entity_1];
            flag_0_1 = arg7 + 1 < result;
            arg7 += 1;
            arg4 = result_1;
            context = source_1;
        } while (flag_0_1);
    }

    return result;
}
