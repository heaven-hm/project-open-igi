// IGI native provenance: Human_Action - 0x0045E3E0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0045E3E0-Human_Action.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Human_Action.hpp"
void Human_Action(void* context, char* arg2)

{
    if (context != *(arg2 + 4) && context != *(arg2 + 8) && context != *(arg2 + 0xc))
    {
        if (!AI_CheckHearing_19(context))
        {
            void* data_5 = *(arg2 + 0xc);
            bool flag_0_1;

            if (data_5)
                flag_0_1 = AI_CombatTarget_19(data_5) == AI_CombatTarget_19(context);

            if (!data_5 || !flag_0_1)
            {
                *arg2 = 1;
                return;
            }
        }
        else
        {
            long double fp0_1 = Sys_AllocateMemory_14(Level_SetMission_5());
            long double f0_1 = *(arg2 + 0x10);
            fp0_1 - f0_1;

            if (*(&((fp0_1 < f0_1 ? 1 : 0) << 8 | (FCMP_UO(fp0_1, f0_1) ? 1 : 0) << 0xa
                | (fp0_1 == f0_1 ? 1 : 0) << 0xe) + 1) & 0x41)
            {
                *arg2 = 1;
                return;
            }
        }
    }

    *arg2 = 0;
}
