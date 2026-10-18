// IGI native provenance: Sys_PushStack_12 - 0x004B3C20 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004B3C20-Sys_PushStack_12.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004B3C20-Sys_PushStack_12.hpp"
float* Sys_PushStack_12(float* context, float arg2, int arg3, int arg4)

{
    context[4] = arg3;
    *context = arg2;
    context[8] = arg4;
    context[1] = 0;
    context[2] = 0;
    context[3] = 0;
    context[5] = 0;
    context[6] = 0;
    context[7] = 0;
    context[9] = 0;
    return context;
}
