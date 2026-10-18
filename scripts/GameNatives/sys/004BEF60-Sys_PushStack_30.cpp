// IGI native provenance: Sys_PushStack_30 - 0x004BEF60 - CFG score 77.8% (FAIL; structural only); runtime behavior not verified.
// Evidence: 004BEF60-Sys_PushStack_30.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004BEF60-Sys_PushStack_30.hpp"
int* Sys_PushStack_30(int* context, int arg2, int arg3, int arg4)

{
    void* data_val = Mem_Alloc(g_a969c4, 4);
    context[1] = data_val;
    int entity_val = g_a969c4;
    memset(memset(data_val, 0, entity_val & 0xfffffffc), 0, entity_val & 3);
    int entity_4 = context[1];
    context[7] = arg2;
    context[4] = arg3;
    *context = entity_4;
    context[2] = 0;
    context[3] = 0;
    context[5] = 0;
    context[6] = arg4;
    return context;
}
