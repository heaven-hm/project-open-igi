// IGI native provenance: Gun_SpawnRound - 0x00474360 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00474360-Gun_SpawnRound.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Gun_SpawnRound.hpp"
void* Gun_SpawnRound(void* context, char arg2)

{
    void* result = Table_GetAt(context + 0x20c, 0);

    if (result)
    {
        void* data_1 = *(context + 0x208);
        char item_30 = arg2;
        char item_2f_1 = 0;
        int item_2c_1 = 0;
        int item_28_1 = 0;
        int item_24_1 = 0;
        int item_20_1 = 0;
        void* result_1 = nullptr;
        int item_c_1 = 0;

        if (!data_1)
            int item_8_2 = 0x3f800000;
        else
            int item_8_1 = *(data_1 + 0x4d4);

        void* item_4_1 = data_1;
        unsigned int data_3 = AI_AlertAllies_17();
        int entity_2 = 0;
        entity_2 = *(result + 0x1c);
        (*(((data_3 * 0x180 + entity_2) << 2) + &g_a96ae0))(result, &item_30);
        result = result_1;
        *(context + 0x1f0) = result;
        *(context + 0x1f4) = item_c_1;
    }

    return result;
}
