// IGI native provenance: Sys_AllocateMemory_20 - 0x004B7540 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004B7540-Sys_AllocateMemory_20.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004B7540-Sys_AllocateMemory_20.hpp"
void Sys_AllocateMemory_20(int* context, int* arg2, void* arg3)

{
    if (!arg3)
        return;

    if (context)
        *context = *(arg3 + 4);

    if (arg2)
        *arg2 = *(arg3 + 8);
}
