// IGI native provenance: Generator_CheckTimer - 0x0043B650 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0043B650-Generator_CheckTimer.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Generator_CheckTimer.h"

char* Generator_CheckTimer(void* self)

{
    (*((FX_TrailUpdate() << 2) + &g_a976e0))(self);
    return Script_CallNativeHandler_6(self + 0x240);
}
