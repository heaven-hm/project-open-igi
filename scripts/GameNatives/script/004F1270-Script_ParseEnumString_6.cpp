// IGI native provenance: Script_ParseEnumString_6 - 0x004F1270 - CFG score 90.5% (PASS; structural only); runtime behavior not verified.
// Evidence: 004F1270-Script_ParseEnumString_6.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004F1270-Script_ParseEnumString_6.hpp"
int Script_ParseEnumString_6(char* context, void* arg2)

{
    unsigned int data_1 = Script_EvaluateCondition_5();
    int entity_val = 0;
    entity_val = *(arg2 + 0x1c);

    if (!*(((data_1 * 0x180 + entity_val) << 2) + &g_a96ae0))
    {
        int saved_index_2 = Script_EvaluateCondition_4(arg2);
        int saved_index_1 = saved_index_2;
        saved_index_2 = *(arg2 + 0x1c);
        int item_4c_2 = QTask_RegisterEventCallback_2(saved_index_2);
        return sprintf(context, "%s_%d");
    }

    unsigned int data_6 = Script_EvaluateCondition_5();
    int data_7 = 0;
    data_7 = *(arg2 + 0x1c);
    void item_40;
    (*(((data_6 * 0x180 + data_7) << 2) + &g_a96ae0))(arg2, &item_40);
    void* dest_val = &item_40;
    int i = 0xffffffff;

    while (i)
    {
        bool flag_0_1 = 0 != *dest_val;
        dest_val += 1;
        i -= 1;

        if (!flag_0_1)
            break;
    }

    int entity_2 = ~i;
    int source_2;
    int dest_3;
    dest_3 = memcpy(context, dest_val - entity_2, entity_2 & 0xfffffffc);
    memcpy(dest_3, source_2, entity_2 & 3);
    return 0;
}
