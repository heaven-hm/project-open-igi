// IGI native provenance: TerrainLightMap_Init_2 - 0x004F82B0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004F82B0-TerrainLightMap_Init_2.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004F82B0-TerrainLightMap_Init_2.hpp"
void* TerrainLightMap_Init_2(void* self)

{
    bool cond:0 = Script_ParseEnumReal32_5();
    int counter_val = *(self + 0x108);

    if (cond:0)
    {
        if (counter_val)
        {
            int scratch_val = *(self + 0xd8);
            *(self + 0xe0) = 1;
            int b97d24_2 = g_b97d24;
            *((b97d24_2 << 2) + &data_b97b20) = scratch_val;
            g_b97d24 = b97d24_2 + 1;
            return b97d24_2 + 1;
        }
    }
    else if (counter_val)
    {
        int counter_val_1 = *(self + 0xd8);
        *(self + 0xe0) = 1;
        int b97d24_1 = g_b97d24;
        *((b97d24_1 << 2) + &data_b97b20) = counter_val_1;
        g_b97d24 = b97d24_1 + 1;
        return b97d24_1 + 1;
    }

    *(self + 0xe0) = 0;
    return self;
}
