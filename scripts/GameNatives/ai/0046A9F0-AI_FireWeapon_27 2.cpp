// IGI native provenance: AI_FireWeapon_27 - 0x0046A9F0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0046A9F0-AI_FireWeapon_27.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AI_FireWeapon_27.h"
int* AI_FireWeapon_27(void* context, void* arg2)

{
    void* dest_val = context;
    AI_CheckSector_26(dest_val);
    int* result = nullptr;
    result = *(arg2 + 0x1c);

    if (*((result << 2) + &g_a994e0))
    {
        unsigned int data_1 = AI_AlertAllies_27();
        int temp_1 = 0;
        temp_1 = *(arg2 + 0x1c);
        result = *(((data_1 * 0x180 + temp_1) << 2) + &g_a96ae0);

        if (result)
        {
            unsigned int data_3 = AI_AlertAllies_27();
            int entity_4 = 0;
            entity_4 = *(arg2 + 0x1c);
            result = (*(((data_3 * 0x180 + entity_4) << 2) + &g_a96ae0))(arg2, &context);
            result = context;

            if (result)
            {
                result = QTask_LinkEventHandler_2(arg2);
                *(dest_val + 0x90) = result;
            }
        }
    }

    return result;
}
