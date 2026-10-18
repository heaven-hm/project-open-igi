// IGI native provenance: AI_TestCandidate - 0x004898D0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004898D0-AI_TestCandidate.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004898D0-AI_TestCandidate.hpp"
int AI_TestCandidate(void* context, void* arg2, double* arg3, double* arg4, float arg5)

{
    if (!*(arg2 + 0x10))
    {
        long double fpu0;
        long double fpu1;
        long double fpu2;

        if (!AI_BuildEyeSnapshot(fpu0, fpu1, fpu2, arg4, arg3))
        {
            *(arg2 + 0x28) = 0;
            *(arg2 + 0x29) = 0;
        }
        else
            *(arg2 + 0x28) = 1;

        *(arg2 + 0x10) = *(context + 4) + 1;
    }

    if (*(arg2 + 0x28))
    {
        if (!*(arg2 + 0x14))
        {
            *(arg2 + 0x29) = AI_LineOfSight(arg5, arg4, **(arg2 + 8), arg3);
            *(arg2 + 0x14) = *(context + 8) + 1;
        }

        *(arg2 + 0x14) -= 1;
    }

    *(arg2 + 0x10) -= 1;
🚫🚫    int result = Sys_DispatchOpcode_11(arg4, arg3);
    /* unimplemented  {call Sys_DispatchOpcode_11} */
🚫🚫🚫    *(arg2 + 0x20) =
    /* unimplemented  {fstp qword [source_val+0x20], fp0} */
    return result;
}
