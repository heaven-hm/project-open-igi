// IGI native provenance: Qvm_SetOpcodeHandler - 0x004BD3D0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004BD3D0-Sys_AllocateMemory_29.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004BD3D0-Qvm_SetOpcodeHandler.hpp"
int Qvm_SetOpcodeHandler(int context, int arg2, int arg3)

{
    *(((arg2 + context * 6) << 2) + &g_a96108) = arg3;
    return arg3;
}
