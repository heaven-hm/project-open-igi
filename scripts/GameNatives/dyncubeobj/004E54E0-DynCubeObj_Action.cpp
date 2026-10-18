// IGI native provenance: DynCubeObj_Action - 0x004E54E0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004E54E0-DynCubeObj_Action.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "DynCubeObj_Action.hpp"
int DynCubeObj_Action(int* context)

{
    int* source_val = g_b019f4;
    int entity_val = *(*context + source_val);
    int data_2 = g_b019e8 - 1;
    g_b019e8 = data_2;

    if (entity_val != data_2)
    {
        int temp_1 = g_b019e4;
        int data_5 = *(temp_1 + (data_2 << 2)) * g_b019f0 + g_b019e0;
        int dest_2 = *(temp_1 + (entity_val << 2));
        int source_1 = *(source_val + data_5);
        *(temp_1 + (entity_val << 2)) = *(temp_1 + (source_1 << 2));
        *(g_b019e4 + (source_1 << 2)) = dest_2;
        *(g_b019f4 + data_5) = entity_val;
    }

    return Sys_ExecuteBytecode_2(context);
}
