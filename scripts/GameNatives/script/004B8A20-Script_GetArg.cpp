// IGI native provenance: Script_GetArg - 0x004B8A20 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004B8A20-Script_GetArg.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004B8A20-Script_GetArg.hpp"
int Script_GetArg(float context, int arg2)

{
    if (!*(context + 4))
        return Sys_ReadFileBlock_24(*(*(context + 8) + (arg2 << 2)));

    int temp_val;
    return Sys_AllocateMemory_28(context, temp_val, arg2, context, arg2);
}
