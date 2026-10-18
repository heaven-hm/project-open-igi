// IGI native provenance: AlarmControl_SetTrigger - 0x004409F0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004409F0-AlarmControl_SetTrigger.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004409F0-AlarmControl_SetTrigger.hpp"
int AlarmControl_SetTrigger(void* self, int args)

{
    (*(((Script_ParseEnumReal32_8() * 0x180 + Env_UpdateCameraLens_3()) << 2) + &g_a96ae0))(self,
        args);
    Script_Isexists(self + 0x2ac, self);
    Script_Isexists(self + 0x2fc, self);
    return 0;
}
