// IGI native provenance: Terrain_LoadTexturePackage - 0x005175A0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 005175A0-Terrain_LoadTexturePackage.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "005175A0-Terrain_LoadTexturePackage.hpp"
void Terrain_LoadTexturePackage(void *context, const char *arg2)
{
    Terrain_LoadTexturesAndPalettes(context, arg2);
}
