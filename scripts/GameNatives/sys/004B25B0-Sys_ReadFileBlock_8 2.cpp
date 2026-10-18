// IGI native provenance: Sys_ReadFileBlock_8 - 0x004B25B0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004B25B0-Sys_ReadFileBlock_8.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Sys_ReadFileBlock_8.h"
int Sys_ReadFileBlock_8(int context, int arg2)

{
    int item_4 = arg2;
    int result = *((g_546fa0 << 2) + &g_a94e84);
    int item_8 = context;

    if (!result)
        return result;

    return result(&item_8);
}
