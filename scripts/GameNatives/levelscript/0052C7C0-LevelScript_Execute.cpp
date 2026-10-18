// IGI native provenance: LevelScript_Execute - 0x0052C7C0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0052C7C0-LevelScript_Execute.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "LevelScript_Execute.hpp"

void* LevelScript_Execute(void* self)

{
    *(self + 0x68) = g_a44344;
    void* scratch_val = *(self + 0x68);
    *(self + 0x6c) = 0;
    *(self + 0x70) = 0;
    return DynObj_RegisterSymbol_8(scratch_val, self, 0);
}
