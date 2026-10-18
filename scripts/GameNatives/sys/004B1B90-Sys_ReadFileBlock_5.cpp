// IGI native provenance: Sys_ReadFileBlock_5 - 0x004B1B90 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004B1B90-Sys_ReadFileBlock_5.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004B1B90-Sys_ReadFileBlock_5.hpp"
int Sys_ReadFileBlock_5(int context, int arg2)

{
    int result = arg2(context);

    if (result)
        *(result * 0x8c + &g_943708) = 1;

    return result;
}
