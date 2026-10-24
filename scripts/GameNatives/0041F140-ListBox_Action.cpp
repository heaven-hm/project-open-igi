// IGI native provenance: ListBox_Action - 0x0041F140 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0041F140-ListBox_Action.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "0041F140-ListBox_Action.hpp"
int* ListBox_Action(void* context)

{
    int* result = Script_FormatString_6(context + 0xc98);

    if (result)
        result = Script_SpawnNewTask_7(context + 0xc98, context);

    int* i = *(context + 8);

    if (*i && i)
    {
        int entity_1 = g_afa7e0;

        do
        {
            int* temp_1 = *i;
            void* data_3;

            if (!temp_1)
                data_3 = nullptr;
            else
            {
                int data_val = *temp_1;
                int data_1 = -(data_val);
                data_3 = (data_1 - data_1) & temp_1;
            }

            *((entity_1 << 2) + &g_afa6e0) = data_3;
            g_afa7e0 = entity_1 + 1;
            int data_4 = Menu_RequestScreen_14();
            int entity_3 = 0;
            entity_3 = i[7];
            result = *(((data_4 * 0x180 + entity_3) << 2) + &g_a96ae0);

            if (result)
                result = result(i);

            entity_1 = g_afa7e0 - 1;
            g_afa7e0 = entity_1;
            i = *((entity_1 << 2) + &g_afa6e0);
        } while (i);
    }

    return result;
}
