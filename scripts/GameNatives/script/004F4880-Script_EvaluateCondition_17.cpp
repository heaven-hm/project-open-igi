// IGI native provenance: Script_EvaluateCondition_17 - 0x004F4880 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004F4880-Script_EvaluateCondition_17.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004F4880-Script_EvaluateCondition_17.hpp"
int* Script_EvaluateCondition_17(int context, int* arg2, int* arg3, int* arg4, float arg5)

{
    int* result = Physics_CalculateSplineLength_7();
    int* result_1 = arg3;

    while (result_1[7] == result)
    {
        *(context + (*arg2 << 3)) = result_1;
        *(context + (*arg2 << 3) + 4) = *arg4;
        *arg4 = arg5 * result_1[0xe] + *arg4;
        *arg2 += 1;
        int* data_2 = QTask_DeleteTask_2(result_1, Physics_CalculateSplineLength_7());

        if (data_2)
            Script_EvaluateCondition_17(context, arg2, data_2, arg4, arg5);

        result = QTask_TriggerEvent_2(result_1, Physics_CalculateSplineLength_7());
        arg3 = result;

        if (!result)
            break;

        result_1 = result;
        result = Physics_CalculateSplineLength_7();
    }

    return result;
}
