// IGI native provenance: AI_FireWeapon_2 - 0x00451080 - CFG score 91.7% (PASS; structural only); runtime behavior not verified.
// Evidence: 00451080-AI_FireWeapon_2.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AI_FireWeapon_2.hpp"
int* AI_FireWeapon_2(void* context, void* arg2)

{
    int entity_val;
    int item_4 = entity_val;
    int* source_val = g_58a9d8;
    int* result = nullptr;

    for (int* i = *source_val; i; i = *i)
    {
        if (source_val[2])
        {
            void* data_3 = *source_val[3];

            if (arg2 && data_3 == arg2)
            {
                if (AI_UpdatePatrol_19(data_3) == AI_UpdatePatrol_19(arg2))
                {
                    result = source_val;

                    if (!source_val)
                        break;

                    int data_8 = *(context + 0x14);
                    source_val[source_val[0x4a] + 0x24] = 0xffffffff;
                    source_val[source_val[0x4a] + 4] = data_8;
                    source_val[0x4a] += 1;
                    memcpy(&source_val[0x44], DynObj_UpdateDynTree_4(arg2), 0x18);
                    return result;
                }

                result = nullptr;
            }
        }

        source_val = i;
    }

    int data_6 = 0;
    char* entity_1 = &g_57d840;

    while (*entity_1)
    {
        entity_1 = &entity_1[0x130];
        data_6 += 1;

        if (entity_1 >= &g_57e1c0)
            return result;
    }

    *(data_6 * 0x130 + 0x57d844) = QTask_LinkEventHandler_2(arg2);
    *(data_6 * 0x130 + 0x57d960) = 1;
    *(data_6 * 0x130 + 0x57d840) = 1;
    *(data_6 * 0x130 + 0x57d964) = 0;
    int entity_6 = *(context + 0x14);
    *(data_6 * 0x130 + 0x57d8c8) = 0xffffffff;
    *(data_6 * 0x130 + 0x57d848) = entity_6;
    memcpy(data_6 * 0x130 + 0x57d948, DynObj_UpdateDynTree_4(arg2), 0x18);
    *(data_6 * 0x130 + 0x57d83c) = 0;
    *(data_6 * 0x130 + 0x57d838) = 0;
    HeapList_Insert(&g_58a9d8, data_6 * 0x130 + 0x57d838);
    return data_6 * 0x130 + 0x57d838;
}
