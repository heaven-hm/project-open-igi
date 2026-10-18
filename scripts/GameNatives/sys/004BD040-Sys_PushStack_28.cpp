// IGI native provenance: Sys_PushStack_28 - 0x004BD040 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004BD040-Sys_PushStack_28.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004BD040-Sys_PushStack_28.hpp"
char* Sys_PushStack_28(float context, int arg2, char* arg3, int arg4)

{
    *arg3 = 0;
    Sys_UnknownDataType_2(arg3, arg4, context, arg2, 3);
    return arg3;
}
