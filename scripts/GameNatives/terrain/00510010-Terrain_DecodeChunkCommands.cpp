// IGI native provenance: Terrain_DecodeChunkCommands - 0x00510010 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00510010-Terrain_DecodeChunkCommands.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "00510010-Terrain_DecodeChunkCommands.hpp"
int Terrain_DecodeChunkCommands(void *context)
{
    TerrainChunkState *state = g_54eba0;
    int handled = 0;
    while (Terrain_NextCommand(state))
        handled++;
    return handled;
}
