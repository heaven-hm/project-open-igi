// IGI native provenance: LevelFlow_LoadParameters - 0x00489290 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00489290-LevelFlow_LoadParameters.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "LevelFlow_LoadParameters.h"
int LevelFlow_LoadParameters(long double context , void* arg2)

{
    char result = Script_ParseEnumReal32_5();

    if (result)
    {
        int* i = *(arg2 + 8);
        *(arg2 + 0x11c) = *(arg2 + 0x61);

        if (*i && i)
        {
            int temp_1 = g_afa7e0;

            do
            {
                int* source_1 = *i;
                void* data_4;

                if (!source_1)
                    data_4 = nullptr;
                else
                {
                    int data_1 = *source_1;
                    int data_2 = -(data_1);
                    data_4 = (data_2 - data_2) & source_1;
                }

                *((temp_1 << 2) + &g_afa6e0) = data_4;
                int temp_2 = temp_1 + 1;
                int data_5 = 0;
                g_afa7e0 = temp_2;
                data_5 = i[7];
                int data_6 = *((data_5 << 2) + &g_a970e0);

                if (data_6)
                {
                    data_6(i);
                    temp_2 = g_afa7e0;
                }

                i = *((temp_2 << 2) + &g_afa6dc);
                temp_1 = temp_2 - 1;
                g_afa7e0 = temp_1;
            } while (i);
        }

        if (Script_FormatString_6(arg2 + 0x7c))
        {
            Script_SpawnNewTask_7(arg2 + 0x7c, arg2);
            *(arg2 + 0x60) = context;
        }

        if (Script_FormatString_6(arg2 + 0xcc))
        {
            Script_SpawnNewTask_7(arg2 + 0xcc, arg2);
🚫🚫🚫            char item_8_2 = /* char item_8_2 = (unimplemented  {fistp qword [stack_ptr+0x10], fp0}).b */;
            /* unimplemented  {fistp qword [stack_ptr+0x10], fp0} */
            *(arg2 + 0x61) = item_8_2;
        }

        if (*(arg2 + 0x61))
            *(arg2 + 0x60) = 0;

        *(arg2 + 0x68) += 1;
        char temp_7 = g_c28c65;
        char entity_4 = g_c28c64 & 0x80;
        g_5c8aa8 = g_c28c6d & 0x80;
        result = g_c28c66 & 0x80;
        g_5c8aa9 = entity_4;
        entity_4 = g_c28c67;
        g_5c8aaa = temp_7 & 0x80;
        temp_7 = g_c28c68;
        g_5c8aab = result;
        result = g_c28c69 & 0x80;
        g_5c8aac = entity_4 & 0x80;
        entity_4 = g_c28c6a;
        g_5c8aad = temp_7 & 0x80;
        temp_7 = g_c28c6b;
        g_5c8aae = result;
        result = g_c28c6c & 0x80;
        g_5c8aaf = entity_4 & 0x80;
        g_5c8ab0 = temp_7 & 0x80;
        g_5c8ab1 = result;
    }

    return result;
}
