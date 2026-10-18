// IGI native provenance: Terrain_SelectTerrainLOD_10 - 0x0051A160 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0051A160-Terrain_SelectTerrainLOD_10.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Terrain_SelectTerrainLOD_10.h"
int Terrain_SelectTerrainLOD_10(float context, int arg2)

{
    int result = 0;
    int source_val = *(context + 0x1f8);

    if (source_val <= 0)
        return result;

    int* temp_1 = context + 0x68;

    while (*temp_1 != arg2)
    {
        result += 1;
        temp_1 = &temp_1[1];

        if (result >= source_val)
            return result;
    }

    int i = result;

    if (result < source_val - 1)
    {
        int* data_val = context + (result << 2) + 0x68;

        do
        {
            i += 1;
            *data_val = data_val[1];
            data_val = &data_val[1];
        } while (i < *(context + 0x1f8) - 1);
    }

    *(context + (*(context + 0x1f8) << 2) + 0x64) = 0;
    *(context + 0x1f8) -= 1;
    return Terrain_RenderWaterSurface_10(context);
}
