// IGI native provenance: Menu_PlayMusic_22 - 0x0042E870 - CFG score 91.7% (PASS; structural only); runtime behavior not verified.
// Evidence: 0042E870-Menu_PlayMusic_22.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Menu_PlayMusic_22.h"
int* Menu_PlayMusic_22(long double context , int arg2)

{
    int* result = Script_FormatString_6(arg2 + 0x978);

    if (result)
    {
        int result_1 = *(arg2 + 0x91d);
        result = Script_SpawnNewTask_7(arg2 + 0x978, arg2);
        result = context;
        *(arg2 + 0x91d) = result;

        if (result)
        {
            memcpy(arg2 + 0xa18, arg2 + 0x20, 0x18);
            *(arg2 + 0xa28) = *(arg2 + 0xa28) + 8192.0;
            memcpy(arg2 + 0xa30, arg2 + 0x70, 0x28);
            *(arg2 + 0xa88) = 0;
            result = Car_DetectionSweep(arg2 + 0xa90, arg2 + 0xa18, arg2, 1);
        }

        result = *(arg2 + 0x91d);

        if (!result)
        {
            result = result_1;

            if (result)
            {
                FX_TrailUpdate_13(arg2 + 0xa90);
                return FX_DebrisSpawn_13(arg2 + 0xa90, 1, 4, AI_CombatTarget_22(), 4, 8);
            }
        }
    }

    return result;
}
