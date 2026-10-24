// IGI native provenance: CombatPlayAnimation - 0x0048DEC0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0048DEC0-CombatPlayAnimation.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "CombatPlayAnimation.h"
int CombatPlayAnimation(void* context, int* arg2)

{
    int item_c = *(arg2[0xa] + 8);
    char* item_10 = "Playing animation - %d";
    int item_14 = 0xffffffff;
    void* item_18 = context;
    char data_1 = Locomotion_GetState(context, 3);

    if (data_1)
    {
        char* item_c_1 = "In progress";
        int item_10_1 = 0xffffffff;
        void* item_14_1 = context;
        return data_1;
    }

    if (*(arg2[0xa] + 0xc))
        return AI_CombatTarget_2(context, arg2);

    char data_3 = Locomotion_GetState(context, 0);

    if (!data_3)
    {
        char* item_c_2 = "Waiting for standstate";
        int item_10_3 = 0xffffffff;
        void* item_14_2 = context;
        return data_3;
    }

    char* item_c_3 = "Requesting action";
    int item_10_4 = 0xffffffff;
    void* item_14_3 = context;
    void* data_5 = AI_WriteChannel(context, 3, *(arg2[0xa] + 8));
    *(arg2[0xa] + 0xc) = 1;
    return data_5;
}
