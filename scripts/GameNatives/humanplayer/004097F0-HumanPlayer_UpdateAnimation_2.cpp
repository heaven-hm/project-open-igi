// IGI native provenance: HumanPlayer_UpdateAnimation_2 - 0x004097F0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004097F0-HumanPlayer_UpdateAnimation_2.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "HumanPlayer_UpdateAnimation_2.hpp"
int HumanPlayer_UpdateAnimation_2(void* context, void* arg2)

{
    *(arg2 + 0x33d) = 0;
    Render_TransformVertices_10(arg2 + 0x68, 0.0f);
    int item_10_1;
    float item_c;

    if (*(context + 0xc) != 0x1c)
    {
        item_c = 0.200000003f;
        item_10_1 = *(context + 0x10);
    }
    else
    {
        item_c = 0.0f;
        item_10_1 = *(context + 0x10);
    }

    Human_StateAnimation(arg2, item_10_1, item_c);
    int result = HumanPlayer_ApplyRecoil_9(arg2, 3);
    *(context + 0x14) = HumanPlayer_CheckCollision_2;
    return result;
}
