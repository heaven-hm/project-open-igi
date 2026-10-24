// IGI native provenance: Graph_Action - 0x004FAEE0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004FAEE0-Graph_Action.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Graph_Action.h"
int* Graph_Action(int* context)

{
    int* dest_val = context;
    void* source_val = dest_val[1];
    context = *(source_val + 0x58);
    void* data_1 = *(source_val + 0x6c);

    if (data_1)
    {
        Sys_ExecuteBytecode_2(data_1);
        *(source_val + 0x6c) = 0;
    }

    Script_ParseEnumString_24(dest_val, &context, 0);
    *(source_val + 0x6c) = Mem_Alloc(Script_UndefinedGraphnodeda(source_val) * context, 4);
    Script_ParseEnumString_23(source_val, context);
    int* result = context;
    *(source_val + 0x58) = result;
    return result;
}
