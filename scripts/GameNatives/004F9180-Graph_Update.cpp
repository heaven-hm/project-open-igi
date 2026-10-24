// IGI native provenance: Graph_Update - 0x004F9180 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004F9180-Graph_Update.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004F9180-Graph_Update.hpp"
int Graph_Update(void* context)

{
    int dest_val;
    dest_val = *(*(context + 0x14) + 0x1c);
    bool flag_0 = !TaskType_IsDerivedFrom(dest_val, DynObj_LookupHashItem_5());
    char data_3 = *(context + 0x64);
    double item_18[0x3];

    if (flag_0)
    {
        if (data_3)
        {
            Script_SpawnNewTask_23(&item_18, context);
            *(context + 0x64) = 0;
            Script_ParseEnumInt32_23(context, &item_18);
        }
    }
    else if (!data_3)
    {
        Script_SpawnNewTask_23(&item_18, context);
        *(context + 0x64) = 1;
        Script_ParseEnumInt32_23(context, &item_18);
    }
    int result = Script_EvaluateCondition_4(context);
    *(context + 0x50) = result;
    return result;
}
