// IGI native provenance: FX_RunPanicking - 0x0048E4A0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0048E4A0-FX_RunPanicking.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "FX_RunPanicking.h"
int FX_RunPanicking(void* context, int* arg2)

{
    char* item_10 = "Run panicking";
    int item_14 = 0xffffffff;
    void* item_18 = context;
    void* source_1 = arg2[0xa] + 8;

    if (*(source_1 + 0x20) != 0xffffffff)
    {
        AI_WriteChannel(context, 5, 1.0f);
        char result = AI_MoveToNode_3(context, *(source_1 + 0x20), context + 0x3eb0, 0);

        if (!result)
            return result;

        *(context + 0x3de8) = 0xffffffff;
    }

    return AI_CombatTarget_2(context, arg2);
}
