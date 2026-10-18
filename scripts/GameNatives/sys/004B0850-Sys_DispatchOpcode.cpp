// IGI native provenance: Sys_DispatchOpcode - 0x004B0850 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004B0850-Sys_DispatchOpcode.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004B0850-Sys_DispatchOpcode.hpp"
int Sys_DispatchOpcode(int* context, int* arg2, int* arg3, char* arg4, int arg5, int arg6)

{
    if (!Camera_UpdateViewCone_33(0x938280, 0x9402e0, arg4))
        return 0;

    return Sys_ExecuteBytecode(context, arg2, arg3, arg5, 0x938280, 0x9402e0, arg6);
}
