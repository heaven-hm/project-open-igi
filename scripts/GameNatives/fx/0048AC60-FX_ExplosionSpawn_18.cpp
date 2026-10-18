// IGI native provenance: FX_ExplosionSpawn_18 - 0x0048AC60 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0048AC60-FX_ExplosionSpawn_18.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "0048AC60-FX_ExplosionSpawn_18.hpp"
int FX_ExplosionSpawn_18(void* context, void* arg2)

{
    Patrol_CombatStart(context, arg2);
    *(arg2 + 0x18) = sub_48ac90;
    *(context + 0x3db0) = 0;
    *(context + 0x3db4) = 0;
    return 0;
}
