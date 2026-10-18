// IGI native provenance: Menu_CaraiErrorInGraphDNo_2 - 0x00425C10 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00425C10-Menu_CaraiErrorInGraphDNo_2.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Menu_CaraiErrorInGraphDNo_2.hpp"
int* Menu_CaraiErrorInGraphDNo_2(int* context, void* arg2, int arg3, void* arg4)

{
    Physics_ApplyImpulse_8(&context, arg2, context);
    int* data_val = context;

    if (data_val)
    {
        int entity_1 = *data_val;

        if (entity_1 != arg3)
        {
            int* data_1 = Physics_CheckHullCollision_8(arg2, entity_1, arg3);

            if (data_1)
                return Script_ExecuteLevelScript_23(arg2, *data_1);

            int item_c_3 = arg3;
            int item_10_3 = *context;
            int item_14_2 = *(arg4 + 0x20);
            Warning_Show("CarAI: Error in graph %d. No link between nodes %d and %d.");
            return context;
        }
    }

    return data_val;
}
