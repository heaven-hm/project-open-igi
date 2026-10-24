// IGI native provenance: Qvm_SetGlobalState - 0x004B8A10 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004B8A10-Sys_PopStack_23.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004B8A10-Qvm_SetGlobalState.hpp"
int Qvm_SetGlobalState(int context)

{
    g_a965a0 = context;
    return context;
}
