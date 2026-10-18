// IGI native provenance: Menu_FormatScreen_23 - 0x0042FDF0 - CFG score 83.3% (FAIL; structural only); runtime behavior not verified.
// Evidence: 0042FDF0-Menu_FormatScreen_23.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Menu_FormatScreen_23.hpp"
double* Menu_FormatScreen_23(double* context, double* arg2, int arg3)

{
    memcpy(arg3, &context[0xe], 0x28);
    double* result = WeaponType_UpdateAnim(arg2, context, 8);
    *arg2 = context[4] + *arg2;
    arg2[1] = context[5] + arg2[1];
    arg2[2] = context[6] + arg2[2];
    return result;
}
