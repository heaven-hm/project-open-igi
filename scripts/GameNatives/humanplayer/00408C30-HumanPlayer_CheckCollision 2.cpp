// IGI native provenance: HumanPlayer_CheckCollision - 0x00408C30 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00408C30-HumanPlayer_CheckCollision.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "HumanPlayer_CheckCollision.h"
int HumanPlayer_CheckCollision(void* context, void* arg2)

{
    *(arg2 + 0x33d) = 0;
    Render_TransformVertices_10(arg2 + 0x68, 0.0f);
    Human_StateAnimation(arg2, *(context + 0x10), 0.200000003f);
    int result = HumanPlayer_ApplyRecoil_9(arg2, 3);
    *(context + 0x14) = HumanPlayer_ApplyRecoil;
    return result;
}
