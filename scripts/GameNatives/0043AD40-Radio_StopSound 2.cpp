// IGI native provenance: Radio_StopSound - 0x0043AD40 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0043AD40-Radio_StopSound.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Radio_StopSound.h"

int Radio_StopSound(void* self)

{
    Script_Isexists(self + 0x38, self);
    return 0;
}
