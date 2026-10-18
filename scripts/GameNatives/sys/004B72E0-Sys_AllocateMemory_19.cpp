// IGI native provenance: Sys_AllocateMemory_19 - 0x004B72E0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004B72E0-Sys_AllocateMemory_19.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004B72E0-Sys_AllocateMemory_19.hpp"
int Sys_AllocateMemory_19(int context, int arg2, int arg3)

{
    int* temp_4 = g_a43e50;
    int entity_val = *temp_4;
    *temp_4 = entity_val + 1;
    temp_4[entity_val * 3 + 3] = g_a43e54 + 0x943e4c;
    g_a43e54 += arg2;
    temp_4[entity_val * 3 + 2] = context;
    temp_4[entity_val * 3 + 4] = arg3;
    return temp_4[entity_val * 3 + 3];
}
