// IGI native provenance: Script_SpawnNewTask_26 - 0x004FA660 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004FA660-Script_SpawnNewTask_26.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004FA660-Script_SpawnNewTask_26.hpp"
int Script_SpawnNewTask_26(void* context, int arg2)

{
    char item_8 = 6;
    int item_7 = arg2;
    Sys_ResolveSymbol_4(*(context + 4));
    int result = Sys_CloseFileStream_4(*(context + 4), &item_8, 5);
    result = 1;
    return result;
}
