// IGI native provenance: HumanPlayer_CheckCollision_7 - 0x0040F280 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0040F280-HumanPlayer_CheckCollision_7.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "HumanPlayer_CheckCollision_7.h"
int HumanPlayer_CheckCollision_7(void* context, int* arg2)

{
    char result = AI_CheckSector_22(arg2[0x13b]);

    if (result)
    {
        int* data_1 = HumanPlayer_UpdateMotion_11(arg2, arg2[0x353]);

        if (data_1)
            Script_ExecuteLevelScript_12(&arg2[0x131], nullptr, data_1);

        arg2[0x353] = 0xffffffff;
        HumanPlayer_CheckCollision_9(arg2, 1.40129846e-44.0f);
        result = AI_SelectTarget_22(arg2[0x13b], 0.0f, 0, 0x3d6e4bae, 0x3d082b3f);
        *(context + 0x14) = HumanPlayer_ApplyRecoil_7;
    }

    return result;
}
