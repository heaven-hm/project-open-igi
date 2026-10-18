// IGI native provenance: MenuUI_Invoke6_41E420 - 0x0041E420 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0041E420-MenuUI_Invoke6_41E420.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "MenuUI_Invoke6_41E420.h"

void* MenuUI_Invoke6_41E420(void* arg1)

{
    void* dest_ptr = *(arg1 + 0x44);

    if (Script_FormatString_6(arg1 + 0xac))
    {
        void* var_10_1 = arg1;
        int eax_1 = Menu_HandleInput_4(arg1 + 0xac);
        int eax_2 = -(eax_1);
        *(arg1 + 0xa4) = -((eax_2 - eax_2));
    }

    int* eax_5 = Menu_LocalSS(dest_ptr, &data_53b688);
    Sys_FreeMemory_18(*(arg1 + 0x9c), eax_5);
    int* eax_7 = Menu_LocalSS(dest_ptr, &data_53b684);
    void* result = Sys_FreeMemory_18(*(arg1 + 0xa0), eax_7);
    int* i = *(arg1 + 8);

    if (*i && i)
    {
        int afa7e0_1 = g_afa7e0;

        do
        {
            int* edx_1 = *i;
            void* eax_11;

            if (!edx_1)
                eax_11 = nullptr;
            else
            {
                int eax_8 = *edx_1;
                int eax_9 = -(eax_8);
                eax_11 = (eax_9 - eax_9) & edx_1;
            }

            *((afa7e0_1 << 2) + &g_afa6e0) = eax_11;
            g_afa7e0 = afa7e0_1 + 1;
            int eax_12 = Menu_HandleInput_13();
            int eax_13 = 0;
            eax_13 = i[7];
            result = *(((eax_12 * 0x180 + eax_13) << 2) + &g_a96ae0);

            if (result)
                result = result(i);

            afa7e0_1 = g_afa7e0 - 1;
            g_afa7e0 = afa7e0_1;
            i = *((afa7e0_1 << 2) + &g_afa6e0);
        } while (i);
    }

    return result;
}
