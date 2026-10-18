// IGI native provenance: PhysicsObj_LoadLodQsc - 0x004EDAA0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004EDAA0-PhysicsObj_LoadLodQsc.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "PhysicsObj_LoadLodQsc.hpp"

void* PhysicsObj_LoadLodQsc(void* self)

{
    g_a4ee88 = *(self + 0x20) * 0.0199999996f;
    g_a4ee80 = *(self + 0x24) * 0.0199999996f;
    g_a4ee84 = *(self + 0x28) * 0.0199999996f;
    g_a4ee94 = *(self + 0x2c) * 0.0199999996f;
    g_a4ee8c = *(self + 0x30) * 0.0199999996f;
    g_a4ee90 = *(self + 0x34) * 0.0199999996f;
    g_54835c = *(self + 0x38);
    g_a4ee98 = *(self + 0x3c) * 4096.0f;
    return self;
}
