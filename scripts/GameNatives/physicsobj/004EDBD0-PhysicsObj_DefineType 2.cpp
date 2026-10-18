// IGI native provenance: PhysicsObj_DefineType - 0x004EDBD0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004EDBD0-PhysicsObj_DefineType.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "PhysicsObj_DefineType.h"

void* PhysicsObj_DefineType(void* self)

{
    void* result = DynObj_Missing(self + 0x20);

    if (result)
        *(result + 0x24) = 0;

    return result;
}
