// IGI native provenance: SkyTask_Init_2 - 0x0052C580 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0052C580-SkyTask_Init_2.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "0052C580-SkyTask_Init_2.hpp"
void* SkyTask_Init_2(void* self, int* args)

{
    return DynObj_RegisterSymbol_9(*args, self);
}
