// IGI native provenance: Binocular_LoadParameters - 0x00470760 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00470760-Binocular_LoadParameters.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Binocular_LoadParameters.hpp"
int* Binocular_LoadParameters(void* context, int arg2)

{
    (*((Weapon_ComputeAccuracy_5() << 2) + &g_a96ae0))(context, arg2);
    sprintf(context + 0x104, "WEAPON_ID_BINOCULARS");
    *(context + 0x1b0) = 0;
    *(context + 0x1b4) = 0;
    *(context + 0x1b8) = 0;
    *(context + 0x1bc) = 0;
    int* result = QTask_LinkEventHandler_2(Weapon_UpdateRecoil_2(context));
    *(context + 0x1c4) = 0;
    *(context + 0x1c0) = result;
    return result;
}
