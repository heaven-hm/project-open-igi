// IGI native provenance: AI_CheckVisibility_9 - 0x00456960 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00456960-AI_CheckVisibility_9.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AI_CheckVisibility_9.h"
int AI_CheckVisibility_9(void* context, void* arg2)

{
    int data_val = *(arg2 + 0xd04);
    *(arg2 + 0x696) = 0;

    if (data_val == 0xffffffff)
    {
        int data_6 = Human_StateAnimation(arg2, 2, 0.0f);
        *(arg2 + 0xd04) = 0;
        *(context + 0x14) = sub_456a10;
        return data_6;
    }

    if (Render_BindTexture_11(arg2 + 0x68, 0, data_val))
    {
        int data_3 = FX_DebrisSpawn_14(arg2, *(arg2 + 0xd04), 0, 0.0f);
        *(arg2 + 0xd04) = 0;
        *(context + 0x14) = sub_456a10;
        return data_3;
    }

    int item_c_1 = *(arg2 + 0xd04);
    Warning_Show("Animation #%d not found");
    int data_5 = Human_StateAnimation(arg2, 2, 0.0f);
    *(arg2 + 0xd04) = 0;
    *(context + 0x14) = sub_456a10;
    return data_5;
}
