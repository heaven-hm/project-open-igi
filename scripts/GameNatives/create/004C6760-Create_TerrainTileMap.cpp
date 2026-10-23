// IGI native provenance: Create_TerrainTileMap - 0x004C6760 - CFG score 88.9% (PASS; structural only); runtime behavior not verified.
// Evidence: 004C6760-Create_TerrainTileMap.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004C6760-Create_TerrainTileMap.hpp"
int Create_TerrainTileMap(int context, float arg2 )

// Sig: void Create_TerrainTileMap(...)
// Note: Creates the TERRAIN TILE MAP for a level during level load (113-byte
// function). Engine-internal: called by the terrain loader, not meant for
// gameplay scripts.
{
    int item_2c = 0;
    float item_30 = arg2;
    int i = 0;
    int flagP = (Script_GetArg(item_30, item_2c) << 6) + 0xc30080;

    do
    {
        item_30 = arg2;
        *(i + flagP) = Script_GetArg(item_30, i + 1);
        i += 1;
    } while (i < 0x40);

    int item_18 = 0;
    int item_10 = 0;
    int item_c = 0;
    char* item_8 = &g_567c74;
    memcpy(context, &item_18, 0x18);
    return context;
}
