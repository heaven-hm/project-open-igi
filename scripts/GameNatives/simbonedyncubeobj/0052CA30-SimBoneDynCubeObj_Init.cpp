// IGI native provenance: SimBoneDynCubeObj_Init - 0x0052CA30 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0052CA30-SimBoneDynCubeObj_Init.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "SimBoneDynCubeObj_Init.hpp"
int SimBoneDynCubeObj_Init()

{
    int item_4 = 0;
    short result = TaskType_Register("SimBoneDynCubeObj", 0x140, Render_DrawRigidMesh_12(), 0);
    g_54f700 = result;
    return result;
}
