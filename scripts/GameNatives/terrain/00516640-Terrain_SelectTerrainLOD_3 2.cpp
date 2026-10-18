// IGI native provenance: Terrain_SelectTerrainLOD_3 - 0x00516640 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00516640-Terrain_SelectTerrainLOD_3.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Terrain_SelectTerrainLOD_3.h"
int Terrain_SelectTerrainLOD_3()

{
    int item_10 = 0;
    item_10 = 0x10;
    int item_c = 0;
    int item_c_1 = 0x10000;
    int item_8 = 0;
    int item_4 = 0;

    if (!g_bcad74)
        int item_c_2 = 0x10800;

    int* data_1 = g_5ca110;
    int item_8_1 = 0x1c4;
    int item_4_1 = 0x8000;
    (*(*data_1 + 0x14))(data_1, &item_10, &g_a7f890, 0);
    return Sys_FreeMemory_2(&g_a7f878, 0x2ef4, 0x80, 0);
}
