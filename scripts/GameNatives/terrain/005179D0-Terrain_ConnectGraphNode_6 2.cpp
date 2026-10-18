// IGI native provenance: Terrain_ConnectGraphNode_6 - 0x005179D0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 005179D0-Terrain_ConnectGraphNode_6.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Terrain_ConnectGraphNode_6.h"
int Terrain_ConnectGraphNode_6(char* context)

{
    int entity_val = 0;
    int temp_val = 0;
    *(&entity_val + 1) = *context;
    temp_val = context[2];
    entity_val = context[1];
    int temp_1 = 0;
    temp_1 = context[3];
    return (entity_val << 8 | temp_val) << 8 | temp_1;
}
