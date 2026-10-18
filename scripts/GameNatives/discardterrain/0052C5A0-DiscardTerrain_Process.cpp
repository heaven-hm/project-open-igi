// IGI native provenance: DiscardTerrain_Process - 0x0052C5A0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0052C5A0-DiscardTerrain_Process.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "0052C5A0-DiscardTerrain_Process.hpp"
int DiscardTerrain_Process(int* context, void* arg2)

{
    DynObj_LinkDynamicCube_8(context[0x1a], context, &context[0x3c], 4096.0f, 4);
    return DynObj_UpdateDynTree_8(*(arg2 + 4), context, &context[0x4e], &context[0x42], &context[0x48]);
}
