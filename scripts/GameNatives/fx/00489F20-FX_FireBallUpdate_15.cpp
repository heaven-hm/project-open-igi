// IGI native provenance: FX_FireBallUpdate_15 - 0x00489F20 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00489F20-FX_FireBallUpdate_15.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "00489F20-FX_FireBallUpdate_15.hpp"
int FX_FireBallUpdate_15(void* context, int arg2, int arg3, float arg4)

{
    return Bones_PlayAnimation(*(context + 0x4ec) + 0x68, 0, arg2, arg3, arg4);
}
