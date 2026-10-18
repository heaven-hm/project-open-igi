// IGI native provenance: SpriteParticleSystemDynCubeObj_OnTick - 0x005312D0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 005312D0-SpriteParticleSystemDynCubeObj_OnTick.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "SpriteParticleSystemDynCubeObj_OnTick.h"
int SpriteParticleSystemDynCubeObj_OnTick(void* context, void* arg2, double* arg3, float arg4)

{
    *(arg2 + 0x74) = arg4;
    return WorldObject_CommitTransform(context, arg2, arg3, arg4);
}
