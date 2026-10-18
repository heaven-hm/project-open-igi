// IGI native provenance: Sys_FreeMemory_10 - 0x004B2950 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004B2950-Sys_FreeMemory_10.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004B2950-Sys_FreeMemory_10.hpp"
long double Sys_FreeMemory_10(float* context, float* arg2)

{
    long double fpu_val_7_1 = *context - *arg2;
    long double fpu_val_6_1 = context[1] - arg2[1];
    return sqrtl(fpu_val_6_1 * fpu_val_6_1 + fpu_val_7_1 * fpu_val_7_1);
}
