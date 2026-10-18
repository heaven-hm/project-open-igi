// IGI native provenance: Sys_DispatchOpcode_3 - 0x004B1000 - CFG score 66.7% (FAIL; structural only); runtime behavior not verified.
// Evidence: 004B1000-Sys_DispatchOpcode_3.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004B1000-Sys_DispatchOpcode_3.hpp"
int Sys_DispatchOpcode_3(int context, int arg2, int arg3)

{
    return (*(((arg2 + context * 0x82) << 2) + &g_a950a4))(arg3);
}
