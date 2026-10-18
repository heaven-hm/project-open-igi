// IGI native provenance: SlideBar_Action - 0x0041FF30 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0041FF30-SlideBar_Action.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "SlideBar_Action.h"
int SlideBar_Action(long double context , int arg2)

{
    if (Script_FormatString_6(arg2 + 0x1f8))
    {
        Script_SpawnNewTask_7(arg2 + 0x1f8, arg2);
        *(arg2 + 0x248) = context;
    }

    int result = Script_FormatString_6(arg2 + 0xb8);

    if (result)
    {
        if (*(arg2 + 0x248))
        {
            Script_SpawnNewTask_7(arg2 + 0xb8, arg2);
🚫🚫🚫            int item_8_2 = /* int item_8_2 =
                (unimplemented  {fistp qword [stack_ptr+0x10], fp0}).d */;
            /* unimplemented  {fistp qword [stack_ptr+0x10], fp0} */
            result = Menu_PlayMusic_10(arg2, item_8_2);
        }
        else
        {
            result = Script_SpawnNewTask_7(arg2 + 0xb8, arg2);
🚫🚫🚫            *(arg2 + 0xa0) =
            /* unimplemented  {fstp qword [source_val+0xa0], fp0} */
        }
    }

    int* i = *(arg2 + 8);

    if (*i && i)
    {
        int entity_2 = g_afa7e0;

        do
        {
            int* temp_1 = *i;
            void* data_6;

            if (!temp_1)
                data_6 = nullptr;
            else
            {
                int data_3 = *temp_1;
                int data_4 = -(data_3);
                data_6 = (data_4 - data_4) & temp_1;
            }

            *((entity_2 << 2) + &g_afa6e0) = data_6;
            g_afa7e0 = entity_2 + 1;
            int data_7 = Menu_HandleInput_13();
            int data_8 = 0;
            data_8 = i[7];
            result = *(((data_7 * 0x180 + data_8) << 2) + &g_a96ae0);

            if (result)
                result = result(i);

            entity_2 = g_afa7e0 - 1;
            g_afa7e0 = entity_2;
            i = *((entity_2 << 2) + &g_afa6e0);
        } while (i);
    }

    return result;
}
