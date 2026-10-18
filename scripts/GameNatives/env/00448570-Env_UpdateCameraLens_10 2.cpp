// IGI native provenance: Env_UpdateCameraLens_10 - 0x00448570 - CFG score 83.3% (FAIL; structural only); runtime behavior not verified.
// Evidence: 00448570-Env_UpdateCameraLens_10.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Env_UpdateCameraLens_10.h"
double* Env_UpdateCameraLens_10(double* context, double* arg2, int arg3)

{
    memcpy(arg3, &context[0xe], 0x28);
    double* result = WeaponType_UpdateAnim(arg2, context, 1);
    *arg2 = context[4] + *arg2;
    arg2[1] = context[5] + arg2[1];
    arg2[2] = context[6] + arg2[2];
    return result;
}
