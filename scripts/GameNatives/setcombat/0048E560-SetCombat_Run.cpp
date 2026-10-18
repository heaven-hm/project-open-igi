// IGI native provenance: SetCombat_Run - 0x0048E560 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0048E560-SetCombat_Run.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "SetCombat_Run.hpp"
int* SetCombat_Run(void* context, int* arg2)

{
    void* dest_val = *(context + 0x14);
    char* item_30 = "Set Combat";
    int item_34 = 0xffffffff;
    void* item_38 = context;
    void* source_1 = arg2[0xa] + 8;
    int item_1c = 7;
    int item_18 = AI_CombatTarget_19(dest_val);
    int item_14 = 0;
    int item_10 = 0;
    void* item_c = source_1;
    int item_4 = 0;
    int item_8 = 0;
    int* data_1 = *(source_1 + 0x1c);

    if (data_1)
    {
        int data_2 = *data_1;

        if (data_2)
            int item_4_1 = data_2;
    }

    int* source_2 = *(source_1 + 0x18);

    if (source_2)
    {
        int source_3 = *source_2;

        if (source_3)
            int item_8_1 = source_3;
    }

    AI_TakeCover(context, &item_1c);
    return AI_CombatTarget_2(context, arg2);
}
