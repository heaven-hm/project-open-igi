// IGI native provenance: Terrain_ConnectGraphNode_7 - 0x00518130 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00518130-Terrain_ConnectGraphNode_7.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Terrain_ConnectGraphNode_7.h"
double* Terrain_ConnectGraphNode_7(double* context, int arg2, int arg3, double arg4, double arg5, double arg6)

{
    *context = arg5;
    long double fpu_val_7_1 = arg2;
    long double fpu6 = arg4;
    context[1] = arg2;
    *(context + 0xc) = arg3;
    context[2] = fpu_val_7_1 + fpu_val_7_1 - (fpu6 + fpu6) + arg5 + arg6;
    long double fpu_val_6_4 = arg5;
    context[3] = arg4 * 3.0 - arg2 * 3.0 - (fpu_val_6_4 + fpu_val_6_4) - arg6;
    return context;
}
