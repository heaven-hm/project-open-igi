// IGI native provenance: Menu_CaraiErrorInGraphDNo - 0x00425B80 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00425B80-Menu_CaraiErrorInGraphDNo.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Menu_CaraiErrorInGraphDNo.hpp"
double* Menu_CaraiErrorInGraphDNo(double* context, void* arg2, int* arg3, int arg4, void* arg5)

{
    double* result = Physics_ApplyImpulse_8(&arg3, arg2, arg3);
    int* entity_val = arg3;

    if (!entity_val)
        return result;

    int data_val = *entity_val;

    if (data_val == arg4)
        return Script_CallNativeHandler_23(context, entity_val);

    int* data_1 = Physics_CheckHullCollision_8(arg2, data_val, arg4);

    if (data_1)
        return Script_CallNativeHandler_23(context, Script_ExecuteLevelScript_23(arg2, *data_1));

    int item_c_3 = arg4;
    int item_10_3 = *arg3;
    int item_14_3 = *(arg5 + 0x20);
    return Warning_Show("CarAI: Error in graph %d. No link between nodes %d and %d.");
}
