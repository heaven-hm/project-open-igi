// IGI native provenance: Env_ActivateEntity_5 - 0x00444B30 - CFG score 83.3% (FAIL; structural only); runtime behavior not verified.
// Evidence: 00444B30-Env_ActivateEntity_5.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Env_ActivateEntity_5.h"
double* Env_ActivateEntity_5(double* context, double* arg2, int arg3)

{
    memcpy(arg3, &context[0xe], 0x28);
    double* result = WeaponType_UpdateAnim(arg2, context, 0);
    *arg2 = context[4] + *arg2;
    arg2[1] = context[5] + arg2[1];
    arg2[2] = context[6] + arg2[2];
    return result;
}
