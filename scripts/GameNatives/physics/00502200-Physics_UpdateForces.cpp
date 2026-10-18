// IGI native provenance: Physics_UpdateForces - 0x00502200 - CFG score 91.7% (PASS; structural only); runtime behavior not verified.
// Evidence: 00502200-Physics_UpdateForces.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Physics_UpdateForces.hpp"
int* Physics_UpdateForces(int context, int arg2, int arg3)

{
    int* result = QTask_DeleteTask_2(Script_ExecuteLevelScript_4(arg2), g_a7a620);
    int i_1 = arg3;

    if (i_1)
    {
        int i;

        do
        {
            result = QTask_TriggerEvent_2(result, g_a7a620);
            i = i_1;
            i_1 -= 1;
        } while (i != 1);
    }

    memcpy(context, &result[0x3e], 0x18);
    return result;
}
