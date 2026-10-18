// IGI native provenance: SpriteParticleSystemDynCubeObj_Init - 0x00531190 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00531190-SpriteParticleSystemDynCubeObj_Init.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "00531190-SpriteParticleSystemDynCubeObj_Init.hpp"
int SpriteParticleSystemDynCubeObj_Init()

{
    int item_4 = 0;
    short result = TaskType_Register("SpriteParticleSystemDynCubeObj", 0xd8, Particle_EmitParticles_4(), 0);
    g_a84608 = result;
    g_a84610 = 1;
    return result;
}
