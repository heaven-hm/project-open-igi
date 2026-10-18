// IGI native provenance: Terrain_UpdateWaterWaves_2 - 0x005127E0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 005127E0-Terrain_UpdateWaterWaves_2.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Terrain_UpdateWaterWaves_2.hpp"
int Terrain_UpdateWaterWaves_2(void* context)

{
    int item_c = 0x20;
    int item_10 = 0x20;
    int item_14 = *(context + 0x10) - 16.0f;
    int result = *(context + 0x14);
    int item_18 = *(context + 0xc) - 16.0f;
    int result_1 = result;
    return result;
}
