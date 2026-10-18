// IGI native provenance: Terrain_UpdateWaterWaves_7 - 0x00517F00 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00517F00-Terrain_UpdateWaterWaves_7.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Terrain_UpdateWaterWaves_7.hpp"
int* Terrain_UpdateWaterWaves_7(int* context, int arg2, int* arg3, int arg4, int arg5, int arg6, int* arg7)

{
    int data_val;
    int temp_val;
    int dest_val;

    if (!arg7)
    {
        temp_val = arg5;
        dest_val = 0;
        data_val = 0;
        arg5 = temp_val;
        arg6 = arg6;
    }
    else
    {
        temp_val = arg5;
        dest_val = *arg7;
        int context_1 = arg7[2];
        data_val = arg7[1];
        arg5 = context_1;
        int source_2 = temp_val - dest_val;

        if (context_1 >= source_2)
            arg5 = source_2;

        int entity_val = arg7[3];
        int context_3 = arg6 - data_val;

        arg6 = entity_val < context_3 ? entity_val : context_3;
    }

    int context_4 = 0;
    int* result = ((data_val * temp_val + dest_val) << 4) + arg4;

    if (arg3 > 0)
    {
        int* source_4 = context;
        int* result_1 = result;
        arg7 = result_1;

        do
        {
            int temp_2 = 0;

            if (arg2 > 0)
            {
                int* data_4 = source_4;

                do
                {
                    int entity_2;

                    if (temp_2 >= arg5 || context_4 >= arg6)
                        entity_2 = 0;
                    else
                        entity_2 = *(result_1 - source_4 + data_4);

                    *data_4 = entity_2;
                    temp_2 += 1;
                    data_4 = &data_4[1];
                } while (temp_2 < arg2);
            }

            context_4 += 1;
            source_4 = &source_4[arg2];
            result = arg3;
            result_1 = &arg7[temp_val];
            arg7 = result_1;
        } while (context_4 < result);
    }

    return result;
}
