// IGI native provenance: Menu_PlayMusic_5 - 0x004192E0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004192E0-Menu_PlayMusic_5.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Menu_PlayMusic_5.h"
float* Menu_PlayMusic_5(float* context, float arg2, float arg3, int arg4, int arg5)

{
    long double fp0;
    bool c1;
    bool c2;
    fp0 = __fcos(arg2);
    context[4] = arg4;
    context[5] = arg5;
    context[6] = arg3;
    long double fpu_val_7_1 = fp0 * arg3;
    *context = fpu_val_7_1;
    long double fp0_1;
    bool c1_1;
    bool c2_1;
    fp0_1 = __fsin(arg2);
    context[1] = fp0_1 * arg3;
    context[2] = -(fp0_1) * arg3;
    context[3] = fpu_val_7_1;
    return context;
}
