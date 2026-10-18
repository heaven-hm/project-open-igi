// IGI native provenance: Script_SpawnNewTask_7 - 0x004F16E0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004F16E0-Script_SpawnNewTask_7.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Script_SpawnNewTask_7.h"
int Script_SpawnNewTask_7(int* context, void* arg2)

{
    int item_8 = 0;
    int item_4 = 0;
    int result;

    if (!context[1])
        result = Script_Isexists(context, arg2);
    else
    {
        Script_ParseEnumInt32_7(context);
        result = context[1];

        if (!result)
            result = Script_Isexists(context, arg2);
    }

    int source_1 = context[2];

    if (!source_1)
        return result;

    long double fpu0;
    long double fpu1;
    return Sys_DispatchOpcode_24(fpu0, fpu1, source_1);
}
