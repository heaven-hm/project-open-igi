// IGI native provenance: Collision_Reset - 0x00489F00 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00489F00-FX_ExplosionSpawn_15.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "00489F00-Collision_Reset.hpp"
void** Collision_Reset(void* context, float arg2)

{
    return Render_ClipMesh_9(context + 0x68, 1, arg2);
}
