// IGI native provenance: Terrain_SelectTerrainLOD_9 - 0x00518720 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00518720-Terrain_SelectTerrainLOD_9.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Terrain_SelectTerrainLOD_9.hpp"
int Terrain_SelectTerrainLOD_9(int context)

{
    int dest_val = 0;
    int result;

    for (int* i = &g_a800fc; i < &g_a801fc; )
    {
        result = 1 << (dest_val & 0x1f);

        if (context & result)
        {
            result = Sys_CloseFileStream_23(*i);
            i[-1] = 0;
            *i = 0;
        }

        i = &i[2];
        dest_val += 1;
    }

    return result;
}
