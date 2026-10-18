// IGI native provenance: Sys_PopStack_20 - 0x004B76F0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004B76F0-Sys_PopStack_20.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004B76F0-Sys_PopStack_20.hpp"
int Sys_PopStack_20(void* context, int arg2, int arg3, char arg4)

{
    *(*(context + 4) * arg3 + context + arg2 + 0x10) = arg4;
    return arg2;
}
