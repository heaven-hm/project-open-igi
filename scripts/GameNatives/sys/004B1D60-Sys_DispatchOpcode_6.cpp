// IGI native provenance: Sys_DispatchOpcode_6 - 0x004B1D60 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004B1D60-Sys_DispatchOpcode_6.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004B1D60-Sys_DispatchOpcode_6.hpp"
int Sys_DispatchOpcode_6(int context, int arg2)

{
    int result = context * 0x23;
    *((result << 2) + &g_9436e4) = arg2;
    return result;
}
