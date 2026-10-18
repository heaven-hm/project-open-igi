// IGI native provenance: Sys_FreeMemory - 0x004B0670 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004B0670-Sys_FreeMemory.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Sys_FreeMemory.h"
int Sys_FreeMemory(int context, int arg2, int arg3)

{
    return *((arg3 << 8) + context + arg2);
}
