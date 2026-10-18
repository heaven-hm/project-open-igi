// IGI native provenance: AI_UpdateSensors_18 - 0x00463080 - CFG score 94.4% (PASS; structural only); runtime behavior not verified.
// Evidence: 00463080-AI_UpdateSensors_18.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AI_UpdateSensors_18.h"
int* AI_UpdateSensors_18(void* context)

{
    void* item_28 = context;
    int data_1 = *(context + 0xa0);
    void item_20;
    memcpy(&item_20, context + 0x20, 0x18);
    int item_8 = data_1;
    int i = 0;
    int* result = Level_LoadLevel_4();

    if (*result > 0)
    {
        int source_1 = 8;

        do
        {
            int temp_1 = 0;
            temp_1 = *(*(Level_LoadLevel_4() + source_1) + 0x1c);
            unsigned int data_4 = Level_LoadLevel_2();
            void** item_38 = &item_28;
            (*(((temp_1 + data_4 * 0x180) << 2) + &g_a96ae0))(*(Level_LoadLevel_4() + source_1),
                item_38);
            i += 1;
            source_1 += 4;
            result = Level_LoadLevel_4();
        } while (i < *result);
    }

    return result;
}
