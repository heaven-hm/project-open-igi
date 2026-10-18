// IGI native provenance: SoundSys_EvaluateTrigger_15 - 0x004EC0E0 - CFG score 66.7% (FAIL; structural only); runtime behavior not verified.
// Evidence: 004EC0E0-SoundSys_EvaluateTrigger_15.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "SoundSys_EvaluateTrigger_15.h"
int SoundSys_EvaluateTrigger_15(int context, int arg2, int arg3)

{
    int temp_3 = g_a5ef84;
    int result = 0;

    if (temp_3 <= 0)
        return result;

    int* entity_1 = &g_a5ec80;

    do
    {
        if (*entity_1 == context)
        {
            int data_1 = result * 0xc;
            int item_c = arg2;
            int item_4_1 = arg3;
            int item_8_1 = *(data_1 + &g_a5ec88);
            int temp_1 = 0;
            temp_1 = *(data_1 + 0xa5ec84);
            return (*(((temp_1 + *g_54db80 * 0x180) << 2) + &g_a96ae0))(0, &item_c);
        }

        result += 1;
        entity_1 = &entity_1[3];
    } while (result < temp_3);

    return result;
}
