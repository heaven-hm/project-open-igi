// IGI native provenance: AI_AnimationDNotFound - 0x00453A30 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00453A30-AI_AnimationDNotFound.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "00453A30-AI_AnimationDNotFound.hpp"
int AI_AnimationDNotFound(void* context, void* arg2)

{
    int data_val = *(arg2 + 0xcfc);
    *(arg2 + 0x33d) = 0;
    *(arg2 + 0x696) = 0;

    if (data_val == 0xffffffff)
    {
        int data_6 = Human_StateAnimation(arg2, *(context + 0x10), 0.0f);
        *(context + 0x14) = sub_453ae0;
        return data_6;
    }

    if (Render_BindTexture_11(arg2 + 0x68, 0, data_val))
    {
        int data_3 = Bones_PlayAnimation(arg2 + 0x68, 0, *(arg2 + 0xcfc), 0, 0.0f);
        *(context + 0x14) = sub_453ae0;
        return data_3;
    }

    int item_c_1 = *(arg2 + 0xcfc);
    Warning_Show("Animation #%d not found");
    int data_5 = Human_StateAnimation(arg2, *(context + 0x10), 0.0f);
    *(context + 0x14) = sub_453ae0;
    return data_5;
}
