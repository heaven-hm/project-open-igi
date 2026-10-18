// IGI native provenance: PhysicsTerrain_Invoke25 - 0x004F1CF0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004F1CF0-PhysicsTerrain_Invoke25.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "PhysicsTerrain_Invoke25.hpp"

int PhysicsTerrain_Invoke25(int arg1)

{
    return DynObj_LinkDynamicCube(Sys_CloseFileStream_25(), PhysicsTerrain_Invoke3, arg1);
}
