// IGI native provenance: AIAction_KickGrenade - 0x0044D960 - CFG score 90.5% (PASS; structural only); runtime behavior not verified.
// Evidence: 0044D960-AIAction_KickGrenade.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AIAction_KickGrenade.h"
int AIAction_KickGrenade(int context, int arg2 )

// Sig: void AIAction_KickGrenade(int args)
// Note: Kicks a nearby live grenade away (retail soldier behaviour). Param:
// args = grenade/context reference from the event. Use in grenade-event
// handlers.
{
    void* data_val = Env_CheckAmbientArea_12();
    int data_2 = Script_GetArg(arg2, 0);

    if (**(data_val + 0x28))
    {
        int* data_3 = AI_CheckVisibility_2(g_5bda58);
        memcpy(&data_3[2], data_val + 0x10, 0x18);
        void* item_30 = **(data_val + 0x28);
        data_3[0xa] = QTask_LinkEventHandler_2(item_30);
        AI_SelectTarget(sub_48e1e0, data_3, data_2);
    }

    int item_18 = 1;
    int item_10 = 0;
    int item_c = 0x3ff00000;
    char* item_8 = &g_567c74;
    memcpy(context, &item_18, 0x18);
    return context;
}
