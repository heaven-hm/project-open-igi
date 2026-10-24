// IGI native provenance: Vec3_Distance - 0x004B32B0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004B32B0-Sys_DispatchOpcode_11.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004B32B0-Vec3_Distance.hpp"
long double Vec3_Distance(double* context, double* arg2)

{
    long double fpu_val_7_1 = *context - *arg2;
    long double fpu_val_6_1 = context[1] - arg2[1];
    long double fpu_val_5_1 = context[2] - arg2[2];
    return sqrtl(fpu_val_5_1 * fpu_val_5_1 + fpu_val_6_1 * fpu_val_6_1 + fpu_val_7_1 * fpu_val_7_1);
}
