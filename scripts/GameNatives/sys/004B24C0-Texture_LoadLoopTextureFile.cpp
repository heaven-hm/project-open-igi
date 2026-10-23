// IGI native provenance: Texture_LoadLoopTextureFile - 0x004B24C0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004B24C0-Texture_LoadLoopTextureFile.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004B24C0-Texture_LoadLoopTextureFile.hpp"
void Texture_LoadLoopTextureFile(void *context, void *arg2)
{
    Texture_ReadTextureFile(context, arg2, 0);
}
