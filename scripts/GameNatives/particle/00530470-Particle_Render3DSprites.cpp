// IGI native provenance: Particle_Render3DSprites - 0x00530470 - CFG score 66.7% (FAIL; structural only); runtime behavior not verified.
// Evidence: 00530470-Particle_Render3DSprites.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Particle_Render3DSprites.hpp"
short* Particle_Render3DSprites(void* context, short arg2)

{
    *memset(context, 0, 0x3004) = 0;
    *(context + 0x3004) = arg2;
    short* result = context + arg2 * 6;
    *result = 0x800;
    result[2] = 0;
    result[1] = 0;
    return result;
}
