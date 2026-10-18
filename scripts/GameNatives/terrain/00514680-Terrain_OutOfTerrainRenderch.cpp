// IGI native provenance: Terrain_OutOfTerrainRenderch - 0x00514680 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00514680-Terrain_OutOfTerrainRenderch.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Terrain_OutOfTerrainRenderch.hpp"
int* Terrain_OutOfTerrainRenderch(int context, int arg2)

{
    unsigned int temp_2 = COMBINE(0, arg2 * 0xb5af + context * 0x9d) % 0x1fd * 3;
    int* i_1 = *((temp_2 << 2) + &g_a7e080);

    for (int* i = *i_1; i; i = *i)
    {
        if (i_1[0xbbb] == context && i_1[0xbbc] == arg2)
            return i_1;

        i_1 = i;
    }

    int data_3 = g_a7f880;

    if (data_3 != g_a7f884)
    {
        int* source_3 = *(g_a7f87c + (data_3 << 2)) * g_a7f888 + g_a7f878;
        *(g_a7f88c + source_3) = data_3;
        g_a7f880 += 1;
        source_3[0xbba] = 0;
        source_3[0xbbb] = context;
        source_3[0xbbc] = arg2;
        source_3[1] = 0;
        *source_3 = 0;
        Camera_ApplyLineFilter_33((temp_2 << 2) + &g_a7e080, source_3);
        return source_3;
    }

    Error_Show("Out of terrain renderchunks");

    while (true)
        /* nop */
}
