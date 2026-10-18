// IGI native provenance: PhysicsObj_InitLod - 0x004EDB30 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004EDB30-PhysicsObj_InitLod.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "PhysicsObj_InitLod.hpp"

int PhysicsObj_InitLod(void* self)

{
    *(self + 0x30) = 0;
    *(self + 0x34) = 0;
    *(self + 0x38) = 0;
    *(self + 0x3c) = 0;
    return Game_DataSymbolLoad(self + 0x20, &g_script_ret_type);
}
