// IGI native provenance: Sys_CloseFileStream_11 - 0x004B3190 - CFG score 66.7% (FAIL; structural only); runtime behavior not verified.
// Evidence: 004B3190-Sys_CloseFileStream_11.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004B3190-Sys_CloseFileStream_11.hpp"
double* Sys_CloseFileStream_11(int context, double* arg2, double* arg3)

{
    double item_18 = arg3[2] * arg2[1] - arg2[2] * arg3[1];
    double item_10 = -((arg3[2] * *arg2 - arg2[2] * *arg3));
    double item_8 = *arg2 * arg3[1] - *arg3 * arg2[1];
    memcpy(context, &item_18, 0x18);
    return arg2;
}
