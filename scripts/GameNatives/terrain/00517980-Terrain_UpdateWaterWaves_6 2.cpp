// IGI native provenance: Terrain_UpdateWaterWaves_6 - 0x00517980 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00517980-Terrain_UpdateWaterWaves_6.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Terrain_UpdateWaterWaves_6.h"
int* Terrain_UpdateWaterWaves_6(int* context, int arg2)

{
    int* result = context;

    if (result)
    {
        while (true)
        {
            int* result_1 = *result;

            if (result_1)
                result = result_1;
            else
            {
                int* data_val = result[1];

                if (!data_val)
                    break;

                void* data_1 = *data_val;

                if (!data_1)
                    break;

                result = *(data_1 + 8);
            }

            if (!result)
                break;

            if (result[4] == arg2)
                return result;
        }
    }

    return 0;
}
