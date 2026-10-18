// IGI native provenance: Generator_ResetTimer - 0x0043B690 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0043B690-Generator_ResetTimer.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Generator_ResetTimer.h"

int Generator_ResetTimer(void* self, int args)

{
    (*(((Script_ParseEnumReal32_8() * 0x180 + FX_TrailUpdate()) << 2) + &g_a96ae0))(self, args);
    Script_Isexists(self + 0x240, self);
    return 0;
}
