// IGI native provenance: Sys_OpenFileStream_10 - 0x004B2980 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004B2980-Sys_OpenFileStream_10.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004B2980-Sys_OpenFileStream_10.hpp"
long double Sys_OpenFileStream_10(float* context, float* arg2)

{
    return context[1] * arg2[1] + *context * *arg2;
}
