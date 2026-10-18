// IGI native provenance: Sys_ReadFileBlock_20 - 0x004B7590 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004B7590-Sys_ReadFileBlock_20.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Sys_ReadFileBlock_20.h"
int* Sys_ReadFileBlock_20(int* context, int arg2)

{
    for (int* i = context; i; i = Matrix_InvalidateSlot(i))
    {
        if (*i == arg2)
            return i;
    }

    return nullptr;
}
