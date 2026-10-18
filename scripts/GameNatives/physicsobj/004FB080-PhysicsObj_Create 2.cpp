// IGI native provenance: PhysicsObj_Create - 0x004FB080 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004FB080-PhysicsObj_Create.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "PhysicsObj_Create.h"

char* PhysicsObj_Create(int* self)

{
    g_a77258 = 0xffffffff;
    g_a77254 = 0xffffffff;
    self[3] = 0;
    return Script_ParseEnumString_24(self, &g_a77258, 0);
}
