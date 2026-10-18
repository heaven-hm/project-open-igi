// IGI native provenance: HumanPlayer_PlayFootstep_5 - 0x0040CCC0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0040CCC0-HumanPlayer_PlayFootstep_5.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "HumanPlayer_PlayFootstep_5.hpp"
int* HumanPlayer_PlayFootstep_5(int* context, void* arg2)

{
    void* data_val = AI_CombatTarget_21(arg2);

    if (!(context[2] & 1))
        return context;

    AI_CombatTarget_20(arg2);
    AI_CheckHearing_20(arg2);
    AI_TakeCover_22(*(arg2 + 0x4ec));
    AI_UpdateOrders_20(arg2);
    AI_SelectTarget_20(arg2);
    AI_UpdatePatrol_21(arg2, nullptr);
    Human_ViewCam(arg2, 1);
    AI_SelectTarget_19(arg2);
    AI_CheckVisibility_21(arg2, Menu_UpdateLanguage_23(data_val));
    return HumanPlayer_PlaySound_5(arg2, 6);
}
