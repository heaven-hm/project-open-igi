// IGI native provenance: Terrain_UpdateWaterWaves_4 - 0x00516B20 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00516B20-Terrain_UpdateWaterWaves_4.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Terrain_UpdateWaterWaves_4.h"
char* Terrain_UpdateWaterWaves_4(char* context, char* arg2, char* arg3, char* arg4, char* arg5)

{
    int entity_val;
    int item_8 = entity_val;
    char* result_1 = nullptr;
    char* result_2 = context;

    if (Camera_UpdateViewCone_20(result_2) >= 1 && result_2[1] == 0x3a)
    {
        if (arg2)
        {
            Terrain_BuildMipMaps_4(arg2, result_2, 2);
            arg2[2] = 0;
        }

        result_2 = &result_2[2];
    }
    else if (arg2)
        *arg2 = 0;

    arg2 = nullptr;
    char* result = result_2;
    char* source_2 = 0xff;
    context = result;

    if (*result_2)
    {
        do
        {
            char entity_2 = *result;

            if (*(entity_2 + 0xc32321) & 4)
                result = &result[1];
            else if (entity_2 == 0x2f || entity_2 == 0x5c)
                arg2 = &result[1];
            else if (entity_2 == 0x2e)
                result_1 = result;

            result = &result[1];
        } while (*result);

        context = result;
    }

    if (*result_2 && arg2)
    {
        if (arg3)
        {
            char* dest_2 = arg2 - result_2;

            if (dest_2 >= 0xff)
                dest_2 = 0xff;

            Terrain_BuildMipMaps_4(arg3, result_2, dest_2);
            *(dest_2 + arg3) = 0;
            result = context;
        }

        result_2 = arg2;
    }
    else if (arg3)
        *arg3 = 0;

    if (!result_1 || result_1 < result_2)
    {
        if (arg4)
        {
            char* data_5 = result - result_2;

            if (data_5 < 0xff)
                source_2 = data_5;

            Terrain_BuildMipMaps_4(arg4, result_2, source_2);
            *(source_2 + arg4) = 0;
        }

        result = arg5;

        if (result)
            *result = 0;
    }
    else
    {
        if (arg4)
        {
            char* dest_4 = result_1 - result_2;

            if (dest_4 >= 0xff)
                dest_4 = 0xff;

            Terrain_BuildMipMaps_4(arg4, result_2, dest_4);
            *(dest_4 + arg4) = 0;
            result = context;
        }

        if (arg5)
        {
            char* data_4 = result - result_1;

            if (data_4 < 0xff)
                source_2 = data_4;

            result = Terrain_BuildMipMaps_4(arg5, result_1, source_2);
            *(source_2 + arg5) = 0;
        }
    }

    return result;
}
