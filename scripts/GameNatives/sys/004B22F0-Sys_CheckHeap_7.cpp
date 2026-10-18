// IGI native provenance: Sys_CheckHeap_7 - 0x004B22F0 - CFG score 83.3% (FAIL; structural only); runtime behavior not verified.
// Evidence: 004B22F0-Sys_CheckHeap_7.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004B22F0-Sys_CheckHeap_7.hpp"
int Struct_Clear36(int* context)

{
    Sys_DispatchOpcode_3(*context, g_546fac, context);
    memset(context, 0, 0x24);
    return 0;
}
