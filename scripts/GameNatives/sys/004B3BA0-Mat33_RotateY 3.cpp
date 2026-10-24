// IGI native provenance: Mat33_RotateY - 0x004B3BA0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004B3BA0-Sys_ExecuteBytecode_12.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004B3BA0-Mat33_RotateY.hpp"
float* Mat33_RotateY(float* context, float arg2)

{
    long double fp0;
    bool c1;
    bool c2;
    fp0 = __fsin(arg2);
    context[1] = 0;
    context[3] = 0;
    context[4] = 0x3f800000;
    context[5] = 0;
    context[7] = 0;
    context[9] = 0;
    long double fp0_1;
    bool c1_1;
    bool c2_1;
    fp0_1 = __fcos(arg2);
    *context = fp0_1;
    context[2] = fp0;
    context[6] = -(fp0);
    context[8] = fp0_1;
    return context;
}
