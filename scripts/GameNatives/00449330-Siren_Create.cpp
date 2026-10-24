// IGI native provenance: Siren_Create - 0x00449330 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00449330-Siren_Create.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "00449330-Siren_Create.hpp"
char* Siren_Create(void* self)

{
    (*((FX_TrailUpdate() << 2) + &g_a976e0))(self);
    return Script_CallNativeHandler_6(self + 0x278);
}
