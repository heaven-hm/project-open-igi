// IGI native provenance: Script_SpawnNewTask_24 - 0x004F9B80 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004F9B80-Script_SpawnNewTask_24.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004F9B80-Script_SpawnNewTask_24.hpp"
int* Script_SpawnNewTask_24(void* context, int arg2, int arg3, int arg4)

{
    Qhash_Init(1);
    int* result = Mem_Alloc(0x18, 4);
    Qhash_Reset();
    result[2] = arg2;
    result[3] = arg3;
    result[4] = arg4;
    result[5] = context;
    result[1] = 0;
    *result = 0;
    HeapList_Insert(context + 0x70, result);
    Script_GraphnodeLinklimitRe(Script_ExecuteLevelScript_23(context, result[2]), result);
    Script_GraphnodeLinklimitRe(Script_ExecuteLevelScript_23(context, result[3]), result);
    *(context + 0x60) += 1;
    return result;
}
