// IGI native provenance: FX_DebrisSpawn_14 - 0x00489EA0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00489EA0-FX_DebrisSpawn_14.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "FX_DebrisSpawn_14.h"
int FX_DebrisSpawn_14(int context, int arg2, int arg3, float arg4)

{
    return Bones_PlayAnimation(context + 0x68, 0, arg2, arg3, arg4);
}
