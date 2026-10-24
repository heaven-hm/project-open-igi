// IGI native provenance: ListBox_OnEvent - 0x0041F030 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0041F030-ListBox_OnEvent.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "ListBox_OnEvent.h"
int* ListBox_OnEvent(void* context)

{
    Menu_UpdateLanguage_9(context);
    g_57bbd8 = context;

    if (Script_FormatString_6(context + 0xbf8))
        Script_SpawnNewTask_7(context + 0xbf8, context);

    g_57bbd8 = 0;
    int* result = Script_FormatString_6(context + 0xc48);

    if (result)
    {
        Script_SpawnNewTask_7(context + 0xc48, context);
        int entity_1 = *(context + 0xbe0);
🚫🚫🚫        int item_8_1 = /* int item_8_1 = (unimplemented  {fistp qword [stack_ptr+0x8], fp0}).d */;
        /* unimplemented  {fistp qword [stack_ptr+0x8], fp0} */
        result = nullptr;

        if (entity_1 > 0)
        {
            int* temp_1 = context + 0xe0;

            do
            {
                if (item_8_1 == *temp_1)
                {
                    *(context + 0xbe4) = result;
                    *(context + 0xbe8) = item_8_1;
                    break;
                }

                result += 1;
                temp_1 = &temp_1[0xb];
            } while (result < entity_1);
        }
    }

    int* i = *(context + 8);

    if (*i && i)
    {
        int entity_2 = g_afa7e0;

        do
        {
            int* temp_2 = *i;
            void* data_4;

            if (!temp_2)
                data_4 = nullptr;
            else
            {
                int data_1 = *temp_2;
                int data_2 = -(data_1);
                data_4 = (data_2 - data_2) & temp_2;
            }

            *((entity_2 << 2) + &g_afa6e0) = data_4;
            g_afa7e0 = entity_2 + 1;
            int data_5 = Menu_HandleInput_13();
            int entity_4 = 0;
            entity_4 = i[7];
            result = *(((data_5 * 0x180 + entity_4) << 2) + &g_a96ae0);

            if (result)
                result = result(i);

            entity_2 = g_afa7e0 - 1;
            g_afa7e0 = entity_2;
            i = *((entity_2 << 2) + &g_afa6e0);
        } while (i);
    }

    return result;
}
